#include "algorithm.h"

namespace ZSchedule {

  class ComputeUnit {

    std::string opName;

  public:
    ComputeUnit(const std::string& opName_, const int latency) : opName(opName_) {}

    std::string getName() const { return opName; }
  };

  typedef int NodeId;

  bool isEmpty(const NodeId id) {
    return id == 0;
  }

  class DFGNode {
    ComputeUnit compUnit;

  public:

    DFGNode(const ComputeUnit& c_) : compUnit(c_) {}

    std::string getOpName() const {
      return compUnit.getName();
    }
  };
  
  class CDFG {
    NodeId nextNode;

    std::map<NodeId, DFGNode> nodes;

  public:

    CDFG() : nextNode(1) {}

    NodeId addNode(const std::string& name, const ComputeUnit& cu) {
      int val = nextNode;
      nextNode++;

      nodes.insert({val, DFGNode(cu)});

      return val;
    }

    std::set<NodeId> getNodeIds() const {
      std::set<NodeId> nodeIds;
      for (auto n : nodes) {
        nodeIds.insert(n.first);
      }

      return nodeIds;
    }

    DFGNode getNode(const NodeId id) const {
      return dbhc::map_find(id, nodes);
    }

    void directedEdge(const NodeId src, const NodeId dst) {
    }

  };

  class STG {
  public:
    
  };

  STG asapSchedule(const CDFG& app);

  void emitVerilog(const STG& stg, const std::string& fileName);

  class UnitSchedule {
    std::vector<NodeId> operations;
  };

  class OpSchedule {
  public:
    std::vector<UnitSchedule> schedules;
  };

  class Schedule {
  public:
    // Schedule for each compute unit type
    std::map<std::string, OpSchedule> units;

    Schedule(const std::map<std::string, OpSchedule>& units_)  : units(units_) {}
  };

}
