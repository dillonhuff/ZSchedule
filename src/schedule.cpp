#include "schedule.h"

#include <iostream>

#include "z3++.h"

using namespace std;
using namespace z3;

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

}
