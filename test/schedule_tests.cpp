#include "catch.hpp"

#include "coreir.h"
#include "schedule.h"

using namespace std;
using namespace CoreIR;

namespace ZSchedule {

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

    for (auto node : app.getNodeIds()) {
      bool foundNode = false;
      for (auto unit : result.units) {
        for (auto sched : unit.second.schedules) {
          for (auto n : sched.operations) {
            if (node == n) {
              foundNode = true;
              break;
            }
          }
        }
      }

      REQUIRE(foundNode);
    }
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
