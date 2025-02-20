#include "motis/nigiri/railviz.h"

#include "boost/geometry/index/rtree.hpp"
#include "boost/iterator/function_output_iterator.hpp"

#include "utl/enumerate.h"
#include "utl/get_or_create.h"
#include "utl/to_vec.h"

#include "geo/detail/register_box.h"
#include "geo/polyline_format.h"

#include "nigiri/common/linear_lower_bound.h"
#include "nigiri/rt/frun.h"
#include "nigiri/rt/rt_timetable.h"
#include "nigiri/rt/run.h"
#include "nigiri/timetable.h"

#include "motis/core/conv/position_conv.h"
#include "motis/core/conv/trip_conv.h"
#include "motis/nigiri/extern_trip.h"
#include "motis/nigiri/location.h"
#include "motis/nigiri/resolve_run.h"
#include "motis/nigiri/tag_lookup.h"
#include "motis/nigiri/unixtime_conv.h"
#include "motis/path/path_zoom_level.h"

namespace n = nigiri;
namespace bgi = boost::geometry::index;
namespace mm = motis::module;
namespace fbs = flatbuffers;

using route_box = std::pair<geo::box, n::route_idx_t>;
using static_rtree = bgi::rtree<route_box, bgi::quadratic<16>>;

using rt_transport_box = std::pair<geo::box, n::rt_transport_idx_t>;
using rt_rtree = bgi::rtree<rt_transport_box, bgi::quadratic<16>>;

using int_clasz = decltype(n::kNumClasses);

namespace motis::nigiri {

struct route_geo_index {
  route_geo_index() = default;

  route_geo_index(n::timetable const& tt, n::clasz const clasz,
                  n::vector_map<n::route_idx_t, float>& distances) {
    auto values = std::vector<route_box>{};
    for (auto const [i, claszes] : utl::enumerate(tt.route_section_clasz_)) {
      auto const r = n::route_idx_t{i};
      if (claszes.at(0) != clasz) {
        continue;
      }

      auto bounding_box = geo::box{};
      for (auto const l : tt.route_location_seq_[r]) {
        bounding_box.extend(
            tt.locations_.coordinates_.at(n::stop{l}.location_idx()));
      }

      values.emplace_back(bounding_box, r);
      distances[r] = geo::distance(bounding_box.max_, bounding_box.min_);
    }
    rtree_ = static_rtree{values};
  }

  std::vector<n::route_idx_t> get_routes(geo::box const& b) const {
    std::vector<n::route_idx_t> routes;
    rtree_.query(bgi::intersects(b),
                 boost::make_function_output_iterator([&](route_box const& v) {
                   routes.emplace_back(v.second);
                 }));
    return routes;
  }

  static_rtree rtree_;
};

struct rt_transport_geo_index {
  rt_transport_geo_index() = default;

  rt_transport_geo_index(
      n::timetable const& tt, n::rt_timetable const& rtt, n::clasz const clasz,
      n::vector_map<n::rt_transport_idx_t, float>& distances) {
    auto values = std::vector<rt_transport_box>{};
    for (auto const [i, claszes] :
         utl::enumerate(rtt.rt_transport_section_clasz_)) {
      auto const rt_t = n::rt_transport_idx_t{i};
      if (claszes.at(0) != clasz) {
        continue;
      }

      auto bounding_box = geo::box{};
      for (auto const l : rtt.rt_transport_location_seq_[rt_t]) {
        bounding_box.extend(
            tt.locations_.coordinates_.at(n::stop{l}.location_idx()));
      }

      values.emplace_back(bounding_box, rt_t);
      distances[rt_t] = geo::distance(bounding_box.min_, bounding_box.max_);
    }
    rtree_ = rt_rtree{values};
  }

  std::vector<n::rt_transport_idx_t> get_rt_transports(
      geo::box const& b) const {
    std::vector<n::rt_transport_idx_t> rt_transports;
    rtree_.query(bgi::intersects(b), boost::make_function_output_iterator(
                                         [&](rt_transport_box const& v) {
                                           rt_transports.emplace_back(v.second);
                                         }));
    return rt_transports;
  }

  rt_rtree rtree_;
};

struct railviz::impl {
  impl(tag_lookup const& tags, n::timetable const& tt) : tags_{tags}, tt_{tt} {
    static_distances_.resize(tt_.route_location_seq_.size());
    for (auto c = int_clasz{0U}; c != n::kNumClasses; ++c) {
      static_geo_indices_[c] =
          route_geo_index{tt, n::clasz{c}, static_distances_};
    }
  }

  mm::msg_ptr get_trips(mm::msg_ptr const& msg) {
    using motis::railviz::RailVizTripsRequest;
    auto const* req = motis_content(RailVizTripsRequest, msg);

    auto runs = std::vector<n::rt::run>{};
    for (auto const t : *req->trips()) {
      auto const et = to_extern_trip(t);
      auto const r = resolve_run(tags_, tt_, et);
      if (!r.valid()) {
        LOG(logging::error) << "unable to find trip " << et.to_str();
        continue;
      }

      auto const seq =
          tt_.route_location_seq_[tt_.transport_route_[r.t_.t_idx_]];
      auto const stop_range = n::interval{
          n::stop_idx_t{0U}, static_cast<n::stop_idx_t>(seq.size())};
      for (auto const [from, to] : utl::pairwise(stop_range)) {
        auto copy = r;
        copy.stop_range_ = {from, to};
        runs.emplace_back(copy);
      }
    }
    return create_response(runs, false);
  }

  mm::msg_ptr get_trains(mm::msg_ptr const& msg) {
    using motis::railviz::RailVizTrainsRequest;
    auto const req = motis_content(RailVizTrainsRequest, msg);

    auto const start_time =
        n::unixtime_t{std::chrono::duration_cast<n::unixtime_t::duration>(
            std::chrono::seconds{req->start_time()})};
    auto const end_time =
        n::unixtime_t{std::chrono::duration_cast<n::unixtime_t::duration>(
            std::chrono::seconds{req->end_time()})};

    return get_trains(
        {start_time, end_time},
        geo::make_box({from_fbs(req->corner1()), from_fbs(req->corner2())}),
        req->zoom_bounds());
  }

  mm::msg_ptr get_trains(n::interval<n::unixtime_t> time_interval,
                         geo::box const& area, int const zoom_level) {
    auto runs = std::vector<n::rt::run>{};
    for (auto c = int_clasz{0U}; c != n::kNumClasses; ++c) {
      auto const sc = static_cast<service_class>(c);
      if (!path::should_display(sc, zoom_level,
                                std::numeric_limits<float>::infinity())) {
        continue;
      }

      for (auto const& rt_t : rt_geo_indices_[c].get_rt_transports(area)) {
        if (path::should_display(sc, zoom_level, rt_distances_[rt_t])) {
          add_rt_transports(rt_t, time_interval, area, runs);
        }
      }

      for (auto const& r : static_geo_indices_[c].get_routes(area)) {
        if (path::should_display(sc, zoom_level, static_distances_[r])) {
          add_static_transports(r, time_interval, area, runs);
        }
      }
    }
    return create_response(runs);
  }

  mm::msg_ptr create_response(std::vector<n::rt::run> const& runs,
                              bool const x = true) const {
    geo::polyline_encoder<6> enc;

    mm::message_creator mc;

    auto stations = std::vector<fbs::Offset<Station>>{};
    auto known_stations = n::hash_set<n::location_idx_t>{};
    auto const add_station =
        [&](n::location_idx_t const l) -> n::location_idx_t {
      auto const type = tt_.locations_.types_.at(l);
      auto const p = tt_.locations_.parents_[l] != n::location_idx_t::invalid()
                         ? tt_.locations_.parents_[l]
                         : l;
      if (known_stations.insert(l).second) {
        auto const pos = to_fbs(tt_.locations_.coordinates_[l]);
        stations.emplace_back(CreateStation(
            mc,
            mc.CreateString(get_station_id(
                tags_, tt_, type == n::location_type::kGeneratedTrack ? p : l)),
            mc.CreateString(tt_.locations_.names_[p].view()), &pos));
      }
      return l;
    };

    auto polyline_indices_cache =
        n::hash_map<std::pair<n::location_idx_t, n::location_idx_t>,
                    std::int64_t>{};
    auto fbs_polylines = std::vector<fbs::Offset<fbs::String>>{
        mc.CreateString("") /* no zero, zero doesn't have a sign=direction */};
    auto const trains = utl::to_vec(runs, [&](n::rt::run const& r) {
      auto const fr = n::rt::frun{tt_, rtt_.get(), r};
      auto const from = fr[0];
      auto const to = fr[1];

      auto const from_l = add_station(from.get_location_idx());
      auto const to_l = add_station(to.get_location_idx());

      auto const key =
          std::pair{std::min(from_l, to_l), std::max(from_l, to_l)};
      auto const polyline_indices = std::vector<int64_t>{
          utl::get_or_create(
              polyline_indices_cache, key,
              [&] {
                enc.push(tt_.locations_.coordinates_.at(key.first));
                enc.push(tt_.locations_.coordinates_.at(key.second));
                fbs_polylines.emplace_back(mc.CreateString(enc.buf_));
                enc.reset();
                return static_cast<std::int64_t>(fbs_polylines.size() - 1U);
              }) *
          (key.first != from_l ? -1 : 1)};
      return motis::railviz::CreateTrain(
          mc, mc.CreateVector(std::vector{mc.CreateString(fr.name())}),
          static_cast<int>(fr.get_clasz()),
          fr.is_rt() ? rt_distances_[fr.rt_]
                     : static_distances_[tt_.transport_route_[fr.t_.t_idx_]],
          mc.CreateString(get_station_id(tags_, tt_, from_l)),
          mc.CreateString(get_station_id(tags_, tt_, to_l)),
          to_motis_unixtime(from.time(n::event_type::kDep)),
          to_motis_unixtime(to.time(n::event_type::kArr)),
          to_motis_unixtime(from.scheduled_time(n::event_type::kDep)),
          to_motis_unixtime(to.scheduled_time(n::event_type::kArr)),
          fr.is_rt() ? TimestampReason_FORECAST : TimestampReason_SCHEDULE,
          fr.is_rt() ? TimestampReason_FORECAST : TimestampReason_SCHEDULE,
          mc.CreateVector(std::vector{
              to_fbs(mc, nigiri_trip_to_extern_trip(
                             tags_, tt_, fr[0].get_trip_idx(), fr.t_))}),
          mc.CreateVector(polyline_indices));
    });

    auto extras = std::vector<std::uint64_t>{x ? fbs_polylines.size() - 1 : 1U};
    std::iota(begin(extras), end(extras), 1U);

    mc.create_and_finish(
        MsgContent_RailVizTrainsResponse,
        motis::railviz::CreateRailVizTrainsResponse(
            mc, mc.CreateVector(stations), mc.CreateVector(trains),
            mc.CreateVector(fbs_polylines), mc.CreateVector(extras))
            .Union());
    return mm::make_msg(mc);
  }

  void add_rt_transports(n::rt_transport_idx_t const rt_t,
                         n::interval<n::unixtime_t> const time_interval,
                         geo::box const& area, std::vector<n::rt::run>& runs) {
    auto const seq = rtt_->rt_transport_location_seq_[rt_t];
    auto const stop_indices =
        n::interval{n::stop_idx_t{0U}, static_cast<n::stop_idx_t>(seq.size())};
    for (auto const [from, to] : utl::pairwise(stop_indices)) {
      auto const box = geo::make_box(
          {tt_.locations_.coordinates_[n::stop{seq[from]}.location_idx()],
           tt_.locations_.coordinates_[n::stop{seq[to]}.location_idx()]});
      if (!box.overlaps(area)) {
        continue;
      }

      auto const active =
          n::interval{rtt_->unix_event_time(rt_t, from, n::event_type::kDep),
                      rtt_->unix_event_time(rt_t, to, n::event_type::kArr) +
                          n::unixtime_t::duration{1U}};
      if (active.overlaps(time_interval)) {
        runs.emplace_back(n::rt::run{.stop_range_ = {from, to}, .rt_ = rt_t});
      }
    }
  }

  void add_static_transports(n::route_idx_t const r,
                             n::interval<n::unixtime_t> const time_interval,
                             geo::box const& area,
                             std::vector<n::rt::run>& runs) const {
    auto const is_active = [&](n::transport const t) -> bool {
      return (rtt_ == nullptr
                  ? tt_.bitfields_[tt_.transport_traffic_days_[t.t_idx_]]
                  : rtt_->bitfields_[rtt_->transport_traffic_days_[t.t_idx_]])
          .test(to_idx(t.day_));
    };

    auto const seq = tt_.route_location_seq_[r];
    auto const stop_indices =
        n::interval{n::stop_idx_t{0U}, static_cast<n::stop_idx_t>(seq.size())};
    auto const [start_day, _] = tt_.day_idx_mam(time_interval.from_);
    auto const [end_day, _1] = tt_.day_idx_mam(time_interval.to_);
    for (auto const [from, to] : utl::pairwise(stop_indices)) {
      auto const box = geo::make_box(
          {tt_.locations_.coordinates_[n::stop{seq[from]}.location_idx()],
           tt_.locations_.coordinates_[n::stop{seq[to]}.location_idx()]});
      if (!box.overlaps(area)) {
        continue;
      }

      auto const dep_times =
          tt_.event_times_at_stop(r, from, n::event_type::kDep);
      for (auto const [i, t_idx] :
           utl::enumerate(tt_.route_transport_ranges_[r])) {
        auto const day_offset = dep_times[i].days();
        for (auto day = start_day; day <= end_day; ++day) {
          auto const traffic_day = day - day_offset;
          auto const t = n::transport{t_idx, traffic_day};
          if (time_interval.overlaps(
                  {tt_.event_time(t, from, n::event_type::kDep),
                   tt_.event_time(t, to, n::event_type::kArr) +
                       n::unixtime_t::duration{1}}) &&
              is_active(t)) {
            runs.emplace_back(
                n::rt::run{.t_ = t,
                           .stop_range_ = {from, to},
                           .rt_ = n::rt_transport_idx_t::invalid()});
          }
        }
      }
    }
  }

  void update(std::shared_ptr<n::rt_timetable> const& rtt) {
    rtt_ = rtt;
    rt_distances_.resize(rtt_->rt_transport_location_seq_.size());
    for (auto c = int_clasz{0U}; c != n::kNumClasses; ++c) {
      rt_geo_indices_[c] =
          rt_transport_geo_index{tt_, *rtt_, n::clasz{c}, rt_distances_};
    }
  }

  tag_lookup const& tags_;
  n::timetable const& tt_;
  std::shared_ptr<n::rt_timetable> rtt_;
  std::array<route_geo_index, n::kNumClasses> static_geo_indices_;
  std::array<rt_transport_geo_index, n::kNumClasses> rt_geo_indices_;
  n::vector_map<n::route_idx_t, float> static_distances_;
  n::vector_map<n::rt_transport_idx_t, float> rt_distances_;
};

railviz::railviz(tag_lookup const& tags, n::timetable const& tt)
    : impl_{std::make_unique<impl>(tags, tt)} {}

mm::msg_ptr railviz::get_trains(mm::msg_ptr const& msg) const {
  return impl_->get_trains(msg);
}

mm::msg_ptr railviz::get_trips(mm::msg_ptr const& msg) const {
  return impl_->get_trips(msg);
}

void railviz::update(std::shared_ptr<n::rt_timetable> const& rtt) const {
  impl_->update(rtt);
}

railviz::~railviz() = default;

}  // namespace motis::nigiri