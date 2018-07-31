#include "catch.hpp"

#include "schedule.h"

namespace ZSchedule {

  TEST_CASE("Pure DFG schedule") {
    CDFG app;
    ComputeUnit operation("neg", 0);
    NodeId negate = app.addNode(operation);
    
    asapSchedule();
  }

}
