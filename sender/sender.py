import serial
import sys
import time

ser = serial.Serial(sys.argv[1], 250000)

t = time.time()
cnt = 0
with open(sys.argv[2], 'rb') as f:
    buf = f.read(32)
    while buf:
        ser.write(buf)
        cnt += len(buf)
        if time.time() - t > 1:
            t = time.time()
            print(cnt)
            cnt = 0
        buf = f.read(32)
ser.close()
