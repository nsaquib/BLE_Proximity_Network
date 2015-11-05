import csv
import matplotlib.pyplot as plt
from matplotlib import animation
import numpy as np
import serial
import time

connected = False;
ser = serial.Serial("/dev/cu.usbserial-DN0073UM", 9600)
#ser = serial.Serial("/dev/cu.usbserial-DN00B462", 9600)
#ser = serial.Serial("/dev/cu.usbserial-DN00B1WO", 9600)

win = 300
PLOT_LINES = 8
yData = [[], [], [], [], [], [], [], []]
lines = []
fig, ax = plt.subplots()
line, = ax.plot([], [], 'k-')
ax.set_title('RSSI Signal Strength of PrNet Devices', fontsize = 16)
ax.set_xlabel('Time Steps', fontsize = 14)
ax.set_ylabel('RSSI Signal Strength', fontsize = 14)
ax.margins(0.05)
plotlays = [PLOT_LINES]
plotcols = ["red", "orange", "yellow", "green", "blue", "violet", "brown", "black"]
for index in range(PLOT_LINES):
    lobj = ax.plot([], [], lw = 2,color = plotcols[index], label = "DEVICE" + str(index))[0]
    lines.append(lobj)
plt.legend(loc='upper left', bbox_to_anchor=(1, 1.02))
fig.subplots_adjust(right=0.75)

# Create CSV file for data output
filename = "Sensor RSSI Data " + time.strftime("%m%d%y %H%M%S") + ".csv"
with open(filename, "w") as file:
    writer = csv.writer(file)
    row = ["Timestamp"]
    for i in range(0, PLOT_LINES):
        row.append("DEVICE"+str(i))
    writer.writerow(row)

while not connected:
    reponse = ser.read()
    connected = True
    
def init():
    for line in lines:
        line.set_data([],[])
    return lines

def process(xlist, ylist, index, i):
    # Read line from serial port
    data = ser.readline().rstrip()
    # Strip newline charcter and split along commas
    data = data.decode("ascii").split(',')
    # Add data point to list
    yData[index].append(int(data[1]))
    # Use numpy array for manipulation
    y = np.asarray(yData[index]);
    # X axis is length of y data
    x = np.arange(y.size) + 1
    # Function for gating the amount of data to plot
    imin = min(max(0, i - win), x.size - win)
    # Resize x data
    x = x[imin:i]
    # Resize y data
    y = y[imin:i]
    # Add x data
    xlist.append(x)
    # Add y data
    ylist.append(y)
    # Return updated x and y lists
    return {"xlist": xlist, "ylist": ylist}

def writeCSVRow():
    with open(filename, "a") as file:
        writer = csv.writer(file)
        row = [time.time()]
        for i in range(0,PLOT_LINES):
            row.append(yData[i][-1])
        writer.writerow(row)

def animate(i):
    xlist = []
    ylist = []
    data = ser.readline().rstrip()
    data = data.decode("ascii").split(',')
    # Check to process DEVICE0 first
    if (len(data) == 3 and data[0] == '0'):
        yData[0].append(int(data[1]))
        y = np.asarray(yData[0]);
        x = np.arange(y.size) + 1
        imin = min(max(0, i - win), x.size - win)
        x = x[imin:i]
        y = y[imin:i]
        xlist.append(x)
        ylist.append(y)
        # Process the following 7 devices
        for plotNumber in range(1, PLOT_LINES):
            # Get processed data
            newLines = process(xlist, ylist, plotNumber, i)
            # Update x list
            xlist = newLines["xlist"]
            # Update y list
            ylist = newLines["ylist"]
    # Exit animation call if not processing DEVICE0
    else:
        return
    # For index in range(0, 1)
    for lnum,line in enumerate(lines):
        # Set data for each line
        line.set_data(xlist[lnum], ylist[lnum])
    # Rescale axes limits
    ax.relim()
    # Rescale axes view
    ax.autoscale()
    writeCSVRow()
    # Return calculated lines
    return lines

# Animation and plot handler
anim = animation.FuncAnimation(fig, animate, init_func = init, interval = 50)
plt.show()
