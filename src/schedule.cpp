#include "schedule.h"

#include <iostream>

#include "z3++.h"

using namespace std;
using namespace z3;
using namespace dbhc;

namespace ZSchedule {

  void emitVerilog(const STG& stg, const std::string& fileName) {
    
  }

  STG asapSchedule(const CDFG& app) {
    context c;
    optimize opt(c);
    params p(c);
    opt.set(p);
    expr x = c.int_const("x");
    expr y = c.int_const("y");
    opt.add(10 >= x && x >= 0);
    optimize::handle h1 = opt.maximize(x + 7);

    opt.check();

    cout << "x + 7 = " << opt.lower(h1) << endl;
    model m = opt.get_model();
    cout << "x = " << m.eval(x) << endl;

    cout << "Got schedule" << endl;

    return STG();
  }

  Schedule createSchedule(const int cycleConstraint,
                          const std::map<string, int>& resourceCosts,
                          const int areaConstraint,
                          const CDFG app) {
    map<string, OpSchedule> sched;
    for (auto nodeId : app.getNodeIds()) {
      sched.insert({app.getNode(nodeId).getOpName(), {}});
    }

    // Here: create schedule constraints
    context c;
    solver s(c);
    string time = "time_";
    string space = "space_";

    expr maxTime = c.int_val(cycleConstraint);
    expr maxArea = c.int_val(areaConstraint);
    
    map<NodeId, expr> spaceVars;
    map<NodeId, expr> timeVars;

    map<string, expr> resourceCounts;
    for (auto s : sched) {
      expr sVar = c.int_const(("rc_num_" + s.first).c_str());
      resourceCounts.insert({s.first, sVar});
    }

    // Create area cost
    expr areaCost = c.int_val(0);
    for (auto rc : resourceCounts) {
      areaCost = areaCost + c.int_val(map_find(rc.first, resourceCosts)) * rc.second;
    }

    cout << "Area constraint = " << areaCost << endl;
    
    s.add(maxArea >= areaCost);

    // Create spacetime vars
    for (auto node : app.getNodeIds()) {
      expr t = c.int_const((time + to_string(node)).c_str());
      expr s = c.int_const((space + to_string(node)).c_str());
      spaceVars.insert({node, s});
      timeVars.insert({node, t});
    }

    for (auto src : app.getNodeIds()) {

      auto srcTime = map_find(src, timeVars);

      for (auto dst : app.getReceivers(src)) {
        auto dstTime = map_find(dst, timeVars);
        s.add(srcTime <= dstTime);
      }
    }

    // Add positivity constraints
    for (auto node : app.getNodeIds()) {
      auto sv = map_find(node, spaceVars);
      auto tv = map_find(node, timeVars);

      s.add(sv >= 0);
      s.add(tv >= 0);
    }

    // Add space boundary constraints
    expr spaceBound = c.int_val(0);
    for (auto rc : resourceCounts) {
      string resourceName = rc.first;
      for (auto node : app.getNodeIds()) {
        if (app.getNode(node).getOpName() == resourceName) {
          auto nodeSpace = map_find(node, spaceVars);
          s.add(spaceBound <= nodeSpace);
          s.add(nodeSpace < spaceBound + rc.second);
        }
      }

      spaceBound = spaceBound + rc.second;
    }

    // Add overlap constraints
    for (auto node0 : app.getNodeIds()) {
      for (auto node1 : app.getNodeIds()) {
        if (node0 != node1) {
          string op0 = app.getNode(node0).getOpName();
          string op1 = app.getNode(node1).getOpName();
          if (op0 == op1) {
            auto sv0 = map_find(node0, spaceVars);
            auto sv1 = map_find(node1, spaceVars);

            auto tv0 = map_find(node0, timeVars);
            auto tv1 = map_find(node1, timeVars);

            s.add((sv0 != sv1) || (tv0 != tv1));
          }
        }
      }
    }

    cout << "Starting to check" << endl;
    // SAT?
    auto satRes = s.check();

    if (satRes == unsat) {
      cout << "NO VIABLE SCHEDULE" << endl;
      assert(false);
    }

    // Extract and print model
    model m = s.get_model();
    for (auto node : app.getNodeIds()) {
      cout << (space + to_string(node)) << " = " <<  m.eval(map_find(node, spaceVars)) << endl;
      cout << (time + to_string(node)) << "  = " <<  m.eval(map_find(node, timeVars)) << endl;
      cout << "-----" << endl;
    }

    for (auto rc : resourceCounts) {
      cout << rc.first << " count = " << m.eval(rc.second) << endl;
    }

    cout << "Solver found schedule, converting back to table representation" << endl;

    for (auto unit : sched) {
      auto& opSched = sched[unit.first];

      for (int space = 0; space < areaConstraint; space++) {

        UnitSchedule times;
        for (int time = 0; time < cycleConstraint; time++) {
          times.operations.push_back(0);
        }

        opSched.schedules.push_back(times);
      }
    }

    for (auto node : app.getNodeIds()) {
      string opName = app.getNode(node).getOpName();
      auto& opSched = sched[opName];

      cout << "opSched size = " << opSched.schedules.size() << endl;

      int spacePosition = m.eval(map_find(node, spaceVars)).get_numeral_int64();

      cout << "space position = " << spacePosition << endl;
      cout << "opSched size   = " << opSched.schedules.size() << endl;

      // if (opSched.schedules.size() <= spacePosition) {
      //   opSched.schedules[spacePosition] = {};
      // }

      cout << "Set space pos" << endl;
      
      auto& ops = opSched.schedules[spacePosition];

      int timePosition = m.eval(map_find(node, timeVars)).get_numeral_int64();
      cout << "Set time pos, to " << node << endl;

      ops.operations[timePosition] = node;
      cout << "Done with node" << endl;
    }

    cout << "Final schedule" << endl;
    for (auto unit : sched) {
      cout << "--- Unit" << endl;
      for (auto uSched : unit.second.schedules) {
        for (auto node : uSched.operations) {
          cout << node << " ";
        }
        cout << endl;
      }
    }

    return Schedule(sched);
  }

  
}
