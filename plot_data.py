import matplotlib.pyplot as plt
from matplotlib.mlab import csv2rec
import numpy as np
from time_model import get_time_model

def persist( xo, yo ):
    x = []
    y = []

    for i in range(len(xo)-1):
        xi0 = xo[i]
        yi = yo[i]

        xi1 = xo[i+1]

        x.append( xi0 ); y.append(yi)
        x.append( xi1 ); y.append(yi)
    return np.array(x), np.array(y)


if 1:
    data = {}
    for (table,cols) in [
        ('switches',['time_host','value']),
        ('times',['time_host','time_ino']),
        ('adcs',['time_ino','adc']),
        ]:
        fname = table+'.csv'
        rec = csv2rec( fname, names=cols)
        data[table] = rec

    time_model = get_time_model(data['times']['time_ino'],
                                data['times']['time_host'])
    fig = plt.figure()

    ax1=fig.add_subplot(2,1,1)
    x,y = persist( data['switches']['time_host'], data['switches']['value'])
    t0 = x[0]
    ax1.plot( (x-t0)*1000.0, y, 'g-' )

    ax2=fig.add_subplot(2,1,2,sharex=ax1)
    t = time_model.framestamp2timestamp(data['adcs']['time_ino'])
    adc_dt = np.median(t[1:]-t[:-1]) # median dt
    smooth_len_sec = 0.02
    n_samps = int(round(smooth_len_sec/adc_dt))
    y = data['adcs']['adc']
    if 1:
        w=np.hanning(n_samps)
        y = np.convolve(w/w.sum(),y,mode='same')
    ax2.plot( (t-t0)*1000.0, 
              #data['adcs']['adc'],
              y,
              'g-' )
    ax2.plot( (t-t0)*1000.0, 
              data['adcs']['adc'],
              'b.' )
    ax2.set_xlabel('time (msec)')

    plt.show()
