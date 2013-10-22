import sys
from time import sleep
import serial

def show():
        checksum = sum(result[0:4]) & 0xFF
        if result[4] == checksum:
                temp = result[2]
                hum = result[0]
       

	print("T: " + str(temp))
	print("H: " + str(hum)) 


data = "";
QUERY = "\x7E\x00\x0F\x17\x05\x00\x13\xA2\x00\x40\xA8\xBB\x28\xFF\xFE\x02\x49\x53\xC8";
INPUT = "\x7E\x00\x10\x17\x05\x00\x13\xA2\x00\x40\xA8\xBB\x28\xFF\xFE\x02\x44\x34\x03\xE9";
LOW = "\x7E\x00\x10\x17\x05\x00\x13\xA2\x00\x40\xA8\xBB\x28\xFF\xFE\x02\x44\x34\x04\xE8";
HIGH = "\x7E\x00\x10\x17\x05\x00\x13\xA2\x00\x40\xA8\xBB\x28\xFF\xFE\x02\x44\x34\x05\xE7";
MAXTIMINGS = 100
laststate = '\x10'
counter = 0
j = 0
result = [0,0,0,0,0]
ser = serial.Serial("/dev/ttyAMA0", 38400)
ser.timeout = 0.0001

# start command
ser.write(HIGH)
ser.flushOutput()

sleep(0.5)

ser.write(LOW)
ser.flushOutput()

sleep(0.02)


ser.write(INPUT)
ser.flushOutput()

ser.write(QUERY)
ser.flushOutput()
data = ser.read()

if len(data) < 20:
	print("Packet Dropped, received packet: " + data)
	show()
	sys.exit(1)

while data[20] == '\x10':
        sleep(0.000001)
        ser.write(QUERY)
        ser.flushOutput()
        data = ser.read()

for i in range(0, MAXTIMINGS):
        counter = 0
                
        ser.write(QUERY)
        ser.flushOutput()
        data = ser.read()
        while ser.inWaiting():
        	data += ser.read()

	if len(data) < 20:
		print("Packet Dropped")
		break;

        while data[20] == laststate:
                ++counter
                if counter == 1000:
                        break
                ser.write(QUERY)
                ser.flushOutput()
                data = ser.read()
                while ser.inWaiting():
                	data += ser.read()
        ser.write(QUERY)
        ser.flushOutput()
        data = ser.read()
        while ser.inWaiting():
        	data += ser.read()

	if len(data) < 20:
		print("Packet Dropped")
		break

        laststate = data[20]
        if counter == 1000:
                break

        if (i>3) and (i%2 == 0):
                result[j/8] <<= 1
                if counter > 200:
                        result[j/8] |=1
                ++j
if j >=39:
	show()
ser.close()


