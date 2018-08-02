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

    map<NodeId, expr> spaceVars;
    map<NodeId, expr> timeVars;

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

    expr maxTime = c.int_val(cycleConstraint);
    expr maxArea = c.int_val(areaConstraint);

    //s.add(10 >= x && x >= 0);
    s.check();

    model m = s.get_model();
    for (auto node : app.getNodeIds()) {
      cout << (space + to_string(node)) << " = " <<  m.eval(map_find(node, spaceVars)) << endl;
      cout << (time + to_string(node)) << " = " <<  m.eval(map_find(node, timeVars)) << endl;
    }
    cout << "Got schedule" << endl;

    return Schedule(sched);
  }

  
}
