#include "catch.hpp"

#include "coreir.h"
#include "schedule.h"

using namespace std;
using namespace CoreIR;

namespace ZSchedule {

  bool scheduleContainsAllNodes(const Schedule& sched, const CDFG& app) {
    for (auto node : app.getNodeIds()) {
      bool foundNode = false;
      for (auto unit : sched.units) {
        for (auto sched : unit.second.schedules) {
          for (auto n : sched.operations) {
            if (node == n) {
              foundNode = true;
              break;
            }
          }
        }
      }

      if (!foundNode) {
        return false;
      }
    }

    return true;
  }

  TEST_CASE("Schedule negate app") {
    CDFG app;

    ComputeUnit i("in", 0);
    NodeId in = app.addNode("in", i);

    ComputeUnit neg("neg", 0);
    NodeId negate = app.addNode("neg", neg);

    ComputeUnit o("out", 0);
    NodeId out = app.addNode("out", o);

    app.directedEdge(in, negate);
    app.directedEdge(negate, out);

    map<string, int> computeCosts{{"in", 1}, {"out", 1}, {"neg", 1}};
    int cycleConstraint = 1;
    int areaConstraint = 5;

    Schedule result = createSchedule(cycleConstraint, computeCosts, areaConstraint, app);

    REQUIRE(result.units.size() == 3);
    REQUIRE(scheduleContainsAllNodes(result, app));

  }

  TEST_CASE("Schedule two negates on the same unit") {
    CDFG app;

    ComputeUnit i("in", 0);
    ComputeUnit neg("neg", 0);
    ComputeUnit o("out", 0);

    NodeId in0 = app.addNode("in0", i);
    NodeId negate0 = app.addNode("neg0", neg);
    NodeId out0 = app.addNode("out0", o);

    NodeId in1 = app.addNode("in1", i);
    NodeId negate1 = app.addNode("neg1", neg);
    NodeId out1 = app.addNode("out1", o);
    
    app.directedEdge(in0, negate0);
    app.directedEdge(negate0, out0);

    app.directedEdge(in1, negate1);
    app.directedEdge(negate1, out1);
    
    map<string, int> computeCosts{{"in", 1}, {"out", 1}, {"neg", 1}};
    int cycleConstraint = 5;
    int areaConstraint = 3;

    Schedule result =
      createSchedule(cycleConstraint, computeCosts, areaConstraint, app);

    REQUIRE(result.units.size() == 3);
    REQUIRE(scheduleContainsAllNodes(result, app));
  }

  TEST_CASE("Vector add without enough resources for simultaneous addition") {
    CDFG app;

    ComputeUnit i("in", 0);
    ComputeUnit add("add", 0);
    ComputeUnit o("out", 0);

    NodeId in0 = app.addNode("in0", i);
    NodeId in1 = app.addNode("in1", i);
    NodeId a0 = app.addNode("a0", add);
    NodeId out = app.addNode("out0", o);

    app.directedEdge(in0, a0);
    app.directedEdge(in1, a0);
    app.directedEdge(a0, out);

    map<string, int> computeCosts{{"in", 8}, {"out", 1}, {"add", 1}};
    int cycleConstraint = 3;
    int areaConstraint = 10;

    Schedule result =
      createSchedule(cycleConstraint, computeCosts, areaConstraint, app);

    REQUIRE(result.units.size() == 3);
    REQUIRE(scheduleContainsAllNodes(result, app));
  }

  TEST_CASE("General width vector add experiment") {
    CDFG app;

    ComputeUnit i("in", 0);
    ComputeUnit add("add", 0);
    ComputeUnit o("out", 0);

    int lim = 100;
    for (int ind = 0; ind < lim; ind++) {
      NodeId in0 = app.addNode("in0_" + to_string(ind), i);
      NodeId in1 = app.addNode("in1_" + to_string(ind), i);
      NodeId a0 = app.addNode("a0_" + to_string(ind), add);
      NodeId out = app.addNode("out0_" + to_string(ind), o);

      app.directedEdge(in0, a0);
      app.directedEdge(in1, a0);
      app.directedEdge(a0, out);

    }

    map<string, int> computeCosts{{"in", 8}, {"out", 1}, {"add", 1}};
    int cycleConstraint = lim*3;
    int areaConstraint = 10;

    Schedule result =
      createSchedule(cycleConstraint, computeCosts, areaConstraint, app);

    REQUIRE(result.units.size() == 3);
    REQUIRE(scheduleContainsAllNodes(result, app));
  }
  
  // CoreIR::Module* buildModule(const STG& stg, CoreIR::Context* c) {
  //   Type* tp = c->Record({{"in", c->BitIn()->Arr(16)},
  //         {"out", c->Bit()->Arr(16)}});
  //   Module* m = c->getGlobal()->newModuleDecl("mod", tp);
  //   ModuleDef* def = m->newModuleDef();

  //   m->setDef(def);

  //   return m;
  // }

  // TEST_CASE("Pure DFG schedule") {
  //   CDFG app;

  //   ComputeUnit i("in", 0);
  //   NodeId in = app.addNode("in", i);

  //   ComputeUnit neg("neg", 0);
  //   NodeId negate = app.addNode("neg", neg);

  //   ComputeUnit o("out", 0);
  //   NodeId out = app.addNode("out", o);

  //   app.directedEdge(in, negate);
  //   app.directedEdge(negate, out);

  //   STG stg = asapSchedule(app);

  //   Context* c = newContext();
  //   Module* negRes = buildModule(stg, c);

  //   SimulatorState s(negRes);
  //   s.setValue("self.in", BitVector(16, 1));
  //   s.execute();

  //   REQUIRE(s.getBitVec("self.out") == BitVector(16, ~1));

  //   deleteContext(c);
  // }

}
