#include <string>

namespace ZSchedule {

  class ComputeUnit {
  public:
    ComputeUnit(const std::string& opName, const int latency) {}
  };

  typedef int NodeId;
  
  class CDFG {
    int nextNode;
  public:

    CDFG() : nextNode(0) {}

    NodeId addNode(const ComputeUnit& cu) {
      int val = nextNode;
      nextNode++;
      return val;
    }

  };

  void asapSchedule();

}
