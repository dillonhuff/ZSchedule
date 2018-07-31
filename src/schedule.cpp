#include "schedule.h"

#include <iostream>

#include "z3++.h"

using namespace std;
using namespace z3;

namespace ZSchedule {

  void asapSchedule() {
    context c;
    optimize opt(c);
    params p(c);
    opt.set(p);
    expr x = c.int_const("x");
    expr y = c.int_const("y");
    opt.add(10 >= x && x >= 0);
    opt.add(10 >= y && y >= 0);
    opt.add(x + y <= 11);
    optimize::handle h1 = opt.maximize(x);

    opt.check();

    cout << "x = " << opt.lower(h1) << endl;

    cout << "Got schedule" << endl;
  }

}
