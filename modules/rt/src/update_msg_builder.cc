#include "motis/rt/update_msg_builder.h"

#include <algorithm>
#include <tuple>

#include "utl/to_vec.h"
#include "utl/verify.h"

#include "motis/core/access/edge_access.h"
#include "motis/core/access/realtime_access.h"
#include "motis/core/access/time_access.h"
#include "motis/core/conv/event_type_conv.h"
#include "motis/core/conv/timestamp_reason_conv.h"
#include "motis/core/conv/trip_conv.h"

using namespace motis::module;
using namespace flatbuffers;

namespace motis::rt {

update_msg_builder::update_msg_builder(schedule const& sched,
                                       ctx::res_id_t const schedule_res_id)
    : sched_{sched}, schedule_res_id_{schedule_res_id} {}

void update_msg_builder::add_delay(delay_info const* di) {
  ++delay_count_;
  auto const& k = di->get_ev_key();

  if (!k.lcon_is_valid()) {
    return;
  }

  for (auto const& trp :
       *sched_.merged_trips_[get_lcon(k.route_edge_, k.lcon_idx_).trips_]) {
    delays_[trp].emplace_back(di);
  }
}

void update_msg_builder::trip_separated(trip const* trp) {
  separated_trips_.insert(trp);
}

void update_msg_builder::add_reroute(
    trip const* trp, mcd::vector<trip::route_edge> const& old_edges,
    lcon_idx_t const old_lcon_idx) {
  ++reroute_count_;
  if (auto it = previous_reroute_update_.find(trp);
      it != end(previous_reroute_update_)) {
    auto& prev = updates_[it->second];
    prev.intermediate_ = true;
  }
  updates_.emplace_back(rt_update_info{
      Content_RtRerouteUpdate,
      CreateRtRerouteUpdate(fbb_, to_fbs(sched_, fbb_, trp),
                            to_fbs_event_infos(old_edges, old_lcon_idx),
                            to_fbs_event_infos(*trp->edges_, trp->lcon_idx_))
          .Union()});
  previous_reroute_update_[trp] = updates_.size() - 1;
}

void update_msg_builder::add_free_text_nodes(
    trip const* trp, free_text const& ft, std::vector<ev_key> const& events) {
  auto const trip = to_fbs(sched_, fbb_, trp);
  auto const r = Range{0, 0};
  auto const free_text =
      CreateFreeText(fbb_, &r, ft.code_, fbb_.CreateString(ft.text_),
                     fbb_.CreateString(ft.type_));
  for (auto const& k : events) {
    updates_.emplace_back(rt_update_info{
        Content_RtFreeTextUpdate,
        CreateRtFreeTextUpdate(
            fbb_, trip,
            CreateRtEventInfo(
                fbb_,
                fbb_.CreateString(
                    sched_.stations_.at(k.get_station_idx())->eva_nr_),
                motis_to_unixtime(
                    sched_, k ? get_schedule_time(sched_, k) : INVALID_TIME),
                to_fbs(k.ev_type_)),
            free_text)
            .Union()});
  }
}

void update_msg_builder::add_track_nodes(ev_key const& k,
                                         std::string const& track,
                                         motis::time const schedule_time) {
  auto const trip =
      to_fbs(sched_, fbb_, sched_.merged_trips_[k.lcon()->trips_]->at(0));
  if (auto it = previous_track_update_.find(k);
      it != end(previous_track_update_)) {
    auto& prev = updates_[it->second];
    prev.intermediate_ = true;
  }
  updates_.emplace_back(rt_update_info{
      Content_RtTrackUpdate,
      CreateRtTrackUpdate(
          fbb_, trip,
          CreateRtEventInfo(
              fbb_,
              fbb_.CreateString(
                  sched_.stations_.at(k.get_station_idx())->eva_nr_),
              motis_to_unixtime(sched_, schedule_time), to_fbs(k.ev_type_)),
          fbb_.CreateString(track))
          .Union()});
  previous_track_update_[k] = updates_.size() - 1;
}

void update_msg_builder::add_station(node_id_t const station_idx) {
  auto const station = sched_.stations_.at(station_idx).get();
  updates_.emplace_back(rt_update_info{
      Content_RtStationAdded,
      CreateRtStationAdded(fbb_, fbb_.CreateString(station->eva_nr_),
                           fbb_.CreateString(station->name_))
          .Union()});
}

void update_msg_builder::expanded_trip_added(trip const* trp,
                                             expanded_trip_index const eti) {
  return expanded_trip_moved(trp, {}, eti);
}

void update_msg_builder::expanded_trip_moved(
    trip const* trp, std::optional<expanded_trip_index> const old_eti,
    std::optional<expanded_trip_index> const new_eti) {
  if (!old_eti && !new_eti) {
    return;
  }
  auto [etui, inserted] = get_or_insert_expanded_trip(trp);
  if (inserted) {
    etui.old_route_ = old_eti;
  }
  etui.new_route_ = new_eti;
}

void update_msg_builder::trip_formation_message(
    motis::ris::TripFormationMessage const* msg) {
  using motis::ris::TripFormationMessage;
  auto const uuid = msg->trip_id()->uuid()->str();
  if (auto it = previous_trip_formation_update_.find(uuid);
      it != end(previous_trip_formation_update_)) {
    auto& prev = updates_[it->second];
    prev.intermediate_ = true;
  }
  updates_.emplace_back(rt_update_info{
      Content_TripFormationMessage,
      motis_copy_table(TripFormationMessage, fbb_, msg).Union()});
  previous_trip_formation_update_[uuid] = updates_.size() - 1;
}

std::pair<expanded_trip_update_info&, bool>
update_msg_builder::get_or_insert_expanded_trip(trip const* trp) {
  if (auto it = expanded_trips_.find(trp); it != end(expanded_trips_)) {
    return {it->second, false};
  } else {
    return {expanded_trips_[trp], true};
  }
}

void update_msg_builder::reset() {
  fbb_.Clear();

  updates_.clear();
  expanded_trips_.clear();
  delays_.clear();
  separated_trips_.clear();
  previous_reroute_update_.clear();
  previous_trip_formation_update_.clear();
  previous_track_update_.clear();
  delay_count_ = 0;
  reroute_count_ = 0;
}

flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<RtEventInfo>>>
update_msg_builder::to_fbs_event_infos(
    mcd::vector<trip::route_edge> const& edges, lcon_idx_t const lcon_idx) {
  std::vector<flatbuffers::Offset<RtEventInfo>> events;
  for (auto const& e : edges) {
    utl::verify(e->type() == edge::ROUTE_EDGE, "invalid trip edge");
    events.emplace_back(CreateRtEventInfo(
        fbb_,
        fbb_.CreateString(
            sched_.stations_[e->from_->get_station()->id_]->eva_nr_),
        motis_to_unixtime(
            sched_, get_schedule_time(sched_, e, lcon_idx, event_type::DEP)),
        EventType_DEP));
    events.emplace_back(CreateRtEventInfo(
        fbb_,
        fbb_.CreateString(
            sched_.stations_[e->to_->get_station()->id_]->eva_nr_),
        motis_to_unixtime(
            sched_, get_schedule_time(sched_, e, lcon_idx, event_type::ARR)),
        EventType_ARR));
  }
  return fbb_.CreateVector(events);
}

void update_msg_builder::build_delay_updates() {
  for (auto& [trp, delays] : delays_) {
    auto const fbs_trip = to_fbs(sched_, fbb_, trp);
    std::stable_sort(
        begin(delays), end(delays),
        [&](delay_info const* a, delay_info const* b) {
          auto const a_station_idx = a->get_ev_key().get_station_idx();
          auto const b_station_idx = b->get_ev_key().get_station_idx();
          auto const a_is_dep = a->get_ev_key().is_departure();
          auto const b_is_dep = b->get_ev_key().is_departure();
          return std::tie(a->schedule_time_, a_station_idx, a_is_dep) <
                 std::tie(b->schedule_time_, b_station_idx, b_is_dep);
        });
    auto const separated = separated_trips_.find(trp) != end(separated_trips_);
    updates_.emplace_back(rt_update_info{
        Content_RtDelayUpdate,
        CreateRtDelayUpdate(
            fbb_, fbs_trip,
            fbb_.CreateVector(utl::to_vec(
                delays,
                [&](delay_info const* di) {
                  auto const& k = di->get_ev_key();
                  return CreateUpdatedRtEventInfo(
                      fbb_,
                      CreateRtEventInfo(
                          fbb_,
                          fbb_.CreateString(
                              sched_.stations_.at(k.get_station_idx())
                                  ->eva_nr_),
                          motis_to_unixtime(sched_, di->get_schedule_time()),
                          to_fbs(k.ev_type_)),
                      motis_to_unixtime(sched_, di->get_current_time()),
                      to_fbs(di->get_reason()));
                })),
            separated)
            .Union()});
  }
  delays_.clear();
}

inline RtExpandedTripIndex to_fbs(
    std::optional<expanded_trip_index> const& eti) {
  if (eti) {
    return {eti->route_index_, eti->index_in_route_};
  } else {
    return {0, 0};
  }
}

inline RtExpandedTripUpdateType get_expanded_trip_update_type(
    expanded_trip_update_info const& etui) {
  if (etui.old_route_ && etui.new_route_) {
    return RtExpandedTripUpdateType_TripUpdated;
  } else if (etui.old_route_) {
    return RtExpandedTripUpdateType_TripRemoved;
  } else if (etui.new_route_) {
    return RtExpandedTripUpdateType_TripAdded;
  } else {
    // should be handled before this point is reached
    throw utl::fail("update_msg_builder: invalid expanded trip update type");
  }
}

void update_msg_builder::build_expanded_trip_updates() {
  updates_.reserve(updates_.size() + expanded_trips_.size());
  for (auto const& [trp, etui] : expanded_trips_) {
    if (!etui.old_route_ && !etui.new_route_) {
      continue;
    }
    auto const update_type = get_expanded_trip_update_type(etui);
    auto const old_route = to_fbs(etui.old_route_);
    auto const new_route = to_fbs(etui.new_route_);
    updates_.emplace_back(rt_update_info{
        Content_RtExpandedTripUpdate,
        CreateRtExpandedTripUpdate(fbb_, trp->trip_idx_, update_type,
                                   &old_route, &new_route)
            .Union()});
  }
}

msg_ptr update_msg_builder::finish() {
  build_delay_updates();
  build_expanded_trip_updates();
  fbb_.create_and_finish(
      MsgContent_RtUpdates,
      CreateRtUpdates(
          fbb_,
          fbb_.CreateVector(utl::to_vec(updates_,
                                        [&](rt_update_info const& ui) {
                                          return CreateRtUpdate(
                                              fbb_, ui.content_type_,
                                              ui.content_, ui.intermediate_);
                                        })),
          schedule_res_id_)
          .Union(),
      "/rt/update", DestinationType_Topic);
  auto msg = make_msg(fbb_);
  reset();
  return msg;
}

}  // namespace motis::rt
