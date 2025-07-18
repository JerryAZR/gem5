#ifndef __MYLIB_HELLO_HELLO_SIM_OBJECT_HH__
#define __MYLIB_HELLO_HELLO_SIM_OBJECT_HH__

#include "params/HelloSimObject.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"
#include "mylib/hello/goodbye_sim_object.hh"

namespace gem5
{

class HelloSimObject: public SimObject
{
  private:
    int remainingHellosToPrintByEvent;
    EventFunctionWrapper nextHelloEvent;
    void processNextHelloEvent();
    GoodByeSimObject *goodByeObject;
  public:
    HelloSimObject(const HelloSimObjectParams& params);
    virtual void startup() override;
};

} // namespace gem5

#endif // __BOOTCAMP_HELLO_SIM_OBJECT_HELLO_SIM_OBJECT_HH__
