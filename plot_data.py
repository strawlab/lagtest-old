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
                                data['times']['time_host'],
                                max_residual=1e-4)
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
    if 0:
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

    if 1:

        WIDTH = 100
        accum = {0:[], 1:[]}
        for switch_row in data['switches']:
            time_host, value = switch_row
            time_arduino = time_model.timestamp2framestamp(time_host)

            idx = np.argmin(abs(data['adcs']['time_ino']-time_arduino))
            idx_start = idx-WIDTH
            idx_stop = idx+WIDTH
            if idx_start < 0:
                continue
            if idx_stop >= len(data['adcs']):
                continue
            accum[ value ].append( data['adcs']['adc'][idx_start:idx_stop] )

            t = time_model.framestamp2timestamp(
                data['adcs']['time_ino'][idx_start:idx_stop])
        t = (t-t[WIDTH])*1000.0
        m0 = np.mean( accum[0], axis=0 )
        m1 = np.mean( accum[1], axis=0 )

        s0 = np.std( accum[0], axis=0 )
        s1 = np.std( accum[1], axis=0 )

        fig = plt.figure()
        ax1 = fig.add_subplot(2,1,1)
        # for samples in accum[0]:
        #     ax1.plot(t,samples,'-', color=(0.7,0.7,0.7))
        ax1.plot( t, m0, 'b-', lw=2 )
        ax1.fill_between( t, m0-s0, m0+s0,
                          facecolor='b',
                          alpha=0.4,
                          linewidth=0 )

        ax2 = fig.add_subplot(2,1,2,sharex=ax1)
        # for samples in accum[1]:
        #     ax2.plot(t,samples,'-', color=(0.7,0.7,0.7))
        ax2.plot( t, m1, 'b-', lw=2 )
        ax2.fill_between( t, m1-s1, m1+s1,
                          facecolor='b',
                          alpha=0.4,
                          linewidth=0 )

        ax2.set_xlabel( 'time (msec)')

    plt.show()
