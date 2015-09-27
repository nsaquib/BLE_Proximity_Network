import serial
import time
import datetime

connected = False;
ser = serial.Serial("/dev/cu.usbserial-DN0073UM", 9600)
while not connected:
    serIn = ser.read()
    connected = True

now = datetime.datetime.now()
hour = now.hour
minute = now.minute
sec= now.second
ms = int(now.microsecond/1000)

print(hour)
print(minute)
print(sec)
print(ms)

ser.write(bytes(str(hour), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(minute), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(sec), 'ascii'))
ser.write(bytes(',', 'ascii'))
ser.write(bytes(str(ms), 'ascii'))
print("Wrote bits")
