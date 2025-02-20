#pragma once

#include <iomanip>
#include <iostream>

#include "boost/uuid/uuid_io.hpp"

#include "motis/hash_set.h"

#include "motis/core/schedule/schedule.h"

#include "motis/core/access/bfs.h"
#include "motis/core/access/realtime_access.h"
#include "motis/core/access/trip_iterator.h"

#include "motis/core/debug/fbs.h"

namespace motis::debug {

struct station {
  friend std::ostream& operator<<(std::ostream& out, station const& s) {
    auto const& sched = s.sched_;
    auto const idx = s.idx_;
    auto const& st = sched.stations_[idx];
    out << "{idx=" << idx << ", eva=" << st->eva_nr_ << ", name=" << st->name_
        << "}";
    return out;
  }

  schedule const& sched_;
  uint32_t idx_;
};

struct trip_id {
  friend std::ostream& operator<<(std::ostream& out, trip_id const& t) {
    auto const& sched = t.sched_;
    auto const& id = t.id_;
    if (id == full_trip_id{}) {
      return out << "{expanded rule service trip}";
    }
    out << "{primary={station="
        << station{sched, static_cast<uint32_t>(id.primary_.station_id_)}
        << ", time=" << format_time(id.primary_.get_time())
        << ", train_nr=" << id.primary_.get_train_nr()
        << "}, secondary={station="
        << station{sched,
                   static_cast<uint32_t>(id.secondary_.target_station_id_)}
        << ", time=" << format_time(id.secondary_.target_time_) << ", line "
        << id.secondary_.line_id_ << "}}";
    return out;
  }

  schedule const& sched_;
  full_trip_id id_;
};

struct trip {
  friend std::ostream& operator<<(std::ostream& out, trip const& t) {
    auto const& sched = t.sched_;
    auto const* trp = t.trp_;
    out << "{"
        << "ptr=" << trp << ", idx=" << trp->trip_idx_
        << ", id=" << trip_id{sched, trp->id_} << ", uuid=" << trp->uuid_
        << ", edges=" << trp->edges_->size() << ", lcon_idx=" << trp->lcon_idx_
        << ", dbg=" << trp->dbg_.str() << ", fbs=" << to_fbs_json(sched, trp)
        << "}";
    return out;
  }

  schedule const& sched_;
  motis::trip const* trp_;
};

struct trip_with_sections {
  friend std::ostream& operator<<(std::ostream& out,
                                  trip_with_sections const& t) {
    auto const& sched = t.sched_;
    auto const* trp = t.trp_;

    auto const print_event = [&](ev_key const& ek) {
      auto const di = get_delay_info(sched, ek);
      out << "    " << (ek.is_departure() ? "dep" : "arr");
      out << "={time=" << format_time(ek.get_time())
          << ", current=" << format_time(di.get_current_time())
          << ", schedule=" << format_time(di.get_schedule_time())
          << ", reason=" << di.get_reason() << ", canceled=" << ek.is_canceled()
          << ", station=" << station{sched, ek.get_station_idx()} << "}";
    };

    out << "trip " << trip{sched, trp} << ":\n";
    auto sec_idx = 0U;
    auto last_time = time{0};
    auto last_merged_trips = 0U;
    for (auto const& sec : access::sections{trp}) {
      auto const kd = sec.ev_key_from();
      auto const ka = sec.ev_key_to();

      out << "  section " << std::setw(2) << sec_idx
          << ": route_edge={ptr=" << sec.get_route_edge()
          << ", node=" << kd.route_edge_.route_node_
          << ", edge_idx=" << kd.route_edge_.outgoing_edge_idx_
          << "}, lcons=" << sec.get_route_edge()->m_.route_edge_.conns_.size()
          << ", route=" << kd.get_node()->route_
          << ", merged_trips=" << sec.lcon().trips_ << " [ ";
      for (auto const& mt : *sched.merged_trips_[sec.lcon().trips_]) {
        out << cista::ptr_cast(mt) << "/" << mt->trip_idx_ << " ";
      }
      out << "]";
      if (sec_idx != 0U && last_merged_trips != sec.lcon().trips_) {
        out << "    (merged trips change)";
      }
      out << "\n";

      print_event(kd);
      if (kd.get_time() < last_time) {
        out << "    !!! time < previous arrival time !!!";
      }
      out << "\n";

      print_event(ka);
      if (ka.get_time() < kd.get_time()) {
        out << "    !!! time < previous departure time !!!";
      }
      out << "\n";

      for (auto const& te : sec.from_node()->incoming_edges_) {
        if (te->type() == edge::THROUGH_EDGE) {
          out << "    incoming through edge from:\n";
          for (auto const& re : te->from_->incoming_edges_) {
            if (!re->empty()) {
              auto const& through_lc =
                  re->m_.route_edge_.conns_.at(trp->lcon_idx_);
              out << "      merged_trips=" << through_lc.trips_ << " [ ";
              for (auto const& mt : *sched.merged_trips_[through_lc.trips_]) {
                out << cista::ptr_cast(mt) << "/" << mt->trip_idx_ << " ";
              }
              out << "] arriving from "
                  << station{sched, re->from_->get_station()->id_} << "\n";
            }
          }
        }
      }
      for (auto const& te : sec.to_node()->edges_) {
        if (te.type() == edge::THROUGH_EDGE) {
          out << "    outgoing through edge to:\n";
          for (auto const& re : te.to_->edges_) {
            if (!re.empty()) {
              auto const& through_lc =
                  re.m_.route_edge_.conns_.at(trp->lcon_idx_);
              out << "      merged_trips=" << through_lc.trips_ << " [ ";
              for (auto const& mt : *sched.merged_trips_[through_lc.trips_]) {
                out << cista::ptr_cast(mt) << "/" << mt->trip_idx_ << " ";
              }
              out << "] departing to "
                  << station{sched, re.to_->get_station()->id_} << "\n";
            }
          }
        }
      }

      out << "\n";
      ++sec_idx;
      last_time = ka.get_time();
      last_merged_trips = sec.lcon().trips_;
    }
    out << "\n";
    return out;
  }

  schedule const& sched_;
  motis::trip const* trp_;
};

struct rule_service_trip_with_sections {
  friend std::ostream& operator<<(std::ostream& out,
                                  rule_service_trip_with_sections const& t) {
    auto const& sched = t.sched_;
    auto const* main_trp = t.main_trp_;
    auto const lcon_idx = main_trp->lcon_idx_;

    if (main_trp->edges_->empty()) {
      out << "trip " << trip{sched, main_trp} << ": <no sections>\n";
      return out;
    }

    mcd::hash_set<::motis::trip const*> trips;
    auto const first_dep =
        ev_key{main_trp->edges_->front().get_edge(), lcon_idx, event_type::DEP};

    for (auto const& e : route_bfs(first_dep, bfs_direction::BOTH)) {
      auto const& lcon = e->m_.route_edge_.conns_.at(lcon_idx);
      for (auto const& trp : *sched.merged_trips_.at(lcon.trips_)) {
        trips.insert(cista::ptr_cast(trp));
      }
    }

    out << trip_with_sections{sched, main_trp};

    for (auto const* trp : trips) {
      if (trp != main_trp) {
        out << "related " << trip_with_sections{sched, trp};
      }
    }

    return out;
  }

  schedule const& sched_;
  motis::trip const* main_trp_;
};

}  // namespace motis::debug
