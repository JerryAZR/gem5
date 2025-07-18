from m5.objects.SimObject import SimObject
from m5.params import *

class HelloSimObject(SimObject):
    type = "HelloSimObject"
    cxx_header = "mylib/hello/hello_sim_object.hh"
    cxx_class = "gem5::HelloSimObject"

    num_hellos = Param.Int("Number of times to say Hello.")
    goodbye_object = Param.GoodByeSimObject("GoodByeSimObject to say goodbye after done saying hello.")

class GoodByeSimObject(SimObject):
    type = "GoodByeSimObject"
    cxx_header = "mylib/hello/goodbye_sim_object.hh"
    cxx_class = "gem5::GoodByeSimObject"
