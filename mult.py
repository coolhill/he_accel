import numpy as np
from pynq import allocate

from pynq import Overlay, Xlnk
import time
xlnk = Xlnk()
size = 6*1024

overlay = Overlay('/home/xilinx/oscar/supertest/mult.bit')
gsw = overlay.he_mult.gsw
deca = overlay.he_mult.decaFFT
tmpa = overlay.he_mult.tmpa
mult = overlay.he_mult.mult
deca1 = xlnk.cma_array(shape=(size,), dtype=np.cdouble)
gsw1 = xlnk.cma_array(shape=(size,), dtype=np.cdouble)
out = xlnk.cma_array(shape=(1024,), dtype=np.cdouble)

gsw1.physical_address
deca1.physical_address
out.physical_address
for i in range(1024):
    gsw1[i] = complex(i,0.0)
    deca1[i] = complex(i,0.0)

for i in range(1024):
    out[i] = complex(0.0,0.0)

print(out)
print(gsw1)
print(deca1)

start_time = time.time()
gsw.sendchannel.transfer(gsw1)
deca.sendchannel.transfer(deca1)
tmpa.recvchannel.transfer(out)
mult.write(0x00, 0x81)
deca.sendchannel.wait()
gsw.sendchannel.wait()
tmpa.recvchannel.wait()
stop_time = time.time()
hw_exec_time = stop_time-start_time
print('Hardware execution time: ',hw_exec_time)
for i in range(1024):
    print(out[i])

deca1.close()
gsw1.close()
out.close()

