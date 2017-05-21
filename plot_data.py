import matplotlib.pyplot as plt
import numpy as np


f2 = open('bit_freq.dat', 'r')
lines = f2.readlines()
f2.close()


x1 = []
y1 = []


for line in lines:
    p = line.split()
    x1.append((p[0]))
    y1.append((p[1]))
   
xv = np.array(x1)
yv = np.array(y1)

plt.plot(xv, yv)

plt.show()
