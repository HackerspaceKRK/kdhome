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

    def __init__(self):
        super().__init__()

    def run(self):
        while True:
            self.poll(500)

            self.process()

            sys.stdout.flush()
            sys.stderr.flush()

    def onInputChangedEvent(self, id, name, value):
        if value == 0:
            for atgl in ["KITCHEN" , "HARDROOM", "SOFTROOM", "CORRIDOR", "CHILLROOM"]:
                if name == atgl:
                    self.toggleOutput(atgl)

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

        return True


    def onIRPressedEvent(self, code):
        pass

    def onIRNewCodeEvent(self, code):
        pass


kd = Light()
kd.connect("localhost", 9999)
kd.reset()
kd.run()
