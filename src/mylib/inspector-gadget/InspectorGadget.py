from m5.objects.ClockedObject import ClockedObject
from m5.params import *

class InspectorGadget(ClockedObject):
    type = "InspectorGadget"
    cxx_header = "mylib/inspector-gadget/inspector_gadget.hh"
    cxx_class = f"gem5::{type}"

    cpu_side_port = ResponsePort("ResponsePort to receive requests from CPU side.")
    mem_side_port = RequestPort("RequestPort to send received requests to memory side.")
    inspection_buffer_entries = Param.Int("Number of entries in the inspection buffer.")
    response_buffer_entries = Param.Int("Number of entries in the response buffer.")