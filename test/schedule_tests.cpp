#include "catch.hpp"

#include "coreir.h"
#include "schedule.h"

using namespace std;
using namespace CoreIR;

namespace ZSchedule {

  Schedule createSchedule(const int cycleConstraint,
                          const std::map<string, int>& resourceCosts,
                          const int areaConstraint,
                          const CDFG app) {
    map<string, OpSchedule> sched;
    for (auto nodeId : app.getNodeIds()) {
      sched.insert({app.getNode(nodeId).getOpName(), {}});
    }
    return Schedule(sched);
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
    int areaConstraint = 200;

    Schedule result = createSchedule(cycleConstraint, computeCosts, areaConstraint, app);

    REQUIRE(result.units.size() == 3);
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
