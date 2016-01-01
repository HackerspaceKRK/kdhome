import kdhome, kdtimer, kdstate
import os, subprocess
import time, select, os, sys

import requests, sys

def run(cmd):
    subprocess.Popen(cmd, shell=True)

class Light(kdhome.KDHome):

    isIdle = False

    userIdleTimer = None

    userState = None
    roomState = None

    roomSwitchTimers = {}
    rooms = ["KITCHEN" , "HARDROOM", "SOFTROOM", "CORRIDOR", "CHILLROOM"]

    def __init__(self):
        super().__init__()

    def run(self):
        while True:
            self.poll(500)

            self.process()

            sys.stdout.flush()
            sys.stderr.flush()

    def oneRoom(self, name):
       for room in self.rooms:
           if name == room:
               self.setOutput(room, 0)
           else:
               self.setOutput(room, 1)

    def leavingHS(self):
        self.oneRoom("CORRIDOR")
        self.setTimeout("leaving", 30, lambda: self.setOutput("CORRIDOR", 1))
    
    def onInputChangedEvent(self, id, name, value):
        if value == 0:
            if name in self.rooms:
               self.roomSwitchTimers[name] = time.time()
        elif value == 1:
            if name in self.rooms:
               released = time.time()
               pressed = self.roomSwitchTimers[name]
               diff = released - pressed
               if diff < 2:
                   self.toggleOutput(name) 
               elif diff > 2 and diff < 5:
                    self.leavingHS()
               elif diff >= 5 and diff < 10:
                    self.oneRoom(name)

    def onInitEvent(self, ev):
        print("INIT")
        dev = self.registerEthernetDevice(0, "10.12.5.10", 9999, "exp1")

        self.setInputName("exp1-1", "KITCHEN")
        self.setInputName("exp1-2", "HARDROOM")
        self.setInputName("exp1-3", "SOFTROOM")
        self.setInputName("exp1-4", "CORRIDOR")
        self.setInputName("exp1-5", "CHILLROOM")
        self.setInputName("exp1-6", "KLATKA")
        self.setInputName("exp1-7", "WC_R")
        self.setInputName("exp1-8", "WC_M")

        self.setOutputName("exp1-1", "KITCHEN")
        self.setOutputName("exp1-2", "HARDROOM")
        self.setOutputName("exp1-3", "SOFTROOM")
        self.setOutputName("exp1-4", "CORRIDOR")
        self.setOutputName("exp1-5", "CHILLROOM")
        self.setOutputName("exp1-6", "KLATKA")
        self.setOutputName("exp1-7", "WC_R")
        self.setOutputName("exp1-8", "WC_M")
        
        self.setTempName("exp1-0", "KORYTARZYK")

        #for atgl in ["KITCHEN" , "HARDROOM", "SOFTROOM", "CORRIDOR", "CHILLROOM"]:
        #        self.toggleOutput(atgl)
        return True


    def onIRPressedEvent(self, code):
        pass

    def onIRNewCodeEvent(self, code):
        pass


kd = Light()
kd.connect("rudy.at.hskrk.pl", 9999)
kd.reset()
kd.run()
