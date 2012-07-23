import matplotlib.pyplot as plt
from matplotlib.mlab import csv2rec
import numpy as np
import sys

data = csv2rec('results.csv', names=['adc','epoch','stamp'] )
ts = data['epoch']*2**16 + data['stamp']

plt.plot( ts, data['adc'], 'b.' )
plt.show()
