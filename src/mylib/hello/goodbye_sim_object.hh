#ifndef __MYLIB_HELLO_GOODBYE_SIM_OBJECT_HH__
#define __MYLIB_HELLO_GOODBYE_SIM_OBJECT_HH__

#include "params/GoodByeSimObject.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class GoodByeSimObject: public SimObject
{
  private:
    EventFunctionWrapper nextGoodByeEvent;
    void processNextGoodByeEvent();
  public:
    GoodByeSimObject(const GoodByeSimObjectParams& params);

    void sayGoodBye();
};

} // namespace gem5


#endif
