#include "mylib/hello/goodbye_sim_object.hh"

#include <iostream>

#include "debug/GoodByeExampleFlag.hh"
#include "base/trace.hh"

gem5::GoodByeSimObject::GoodByeSimObject(const GoodByeSimObjectParams &params) :
    SimObject(params),
    nextGoodByeEvent([this](){processNextGoodByeEvent();},  name() + "nextGoodByeEvent")
{
}


void gem5::GoodByeSimObject::sayGoodBye() {
    panic_if(nextGoodByeEvent.scheduled(), "GoodByeSimObject::sayGoodBye called while nextGoodByeEvent is scheduled!");
    schedule(nextGoodByeEvent, curTick() + 500);
}

void gem5::GoodByeSimObject::processNextGoodByeEvent()
{
    std::cout << "tick: " << curTick() << "Bye!" << std::endl;
    DPRINTF(GoodByeExampleFlag, "%s: GoodBye from GoodByeSimObejct::processNextGoodByeEvent!\n", __func__);
}