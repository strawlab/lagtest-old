import matplotlib.pyplot as plt
import json
import numpy as np
from time_model import get_time_model
from matplotlib.transforms import blended_transform_factory
import sys

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

def make_recarray( orig, n0, n1 ):
    arr0 = np.array([row[0] for row in orig])
    arr1 = np.array([row[1] for row in orig])
    result = np.empty( (len(arr0),), dtype=
                       [(n0,arr0.dtype),(n1,arr1.dtype)])
    result[n0] = arr0
    result[n1] = arr1
    return result

if 1:
    fname = sys.argv[1]
    buf = open(fname).read()
    data1 = json.loads(buf)
    data = {}
    for (table,cols) in [
        ('switches',['time_host','value']),
        ('times',['time_host','time_ino']),
        ('adcs',['time_ino','adc']),
        ]:
        data[table] = make_recarray(data1.pop(table), cols[0], cols[1])
    for k in data1:
        print '%s: %r'%(k, data1[k])

    time_model = get_time_model(data['times']['time_ino'],
                                data['times']['time_host'],
                                max_residual=1e-4)
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
        ax1.plot( t, m0, 'b-', lw=2 )
        ax1.fill_between( t, m0-s0, m0+s0,
                          facecolor='b',
                          alpha=0.4,
                          linewidth=0 )

        ax2 = fig.add_subplot(2,1,2,sharex=ax1,sharey=ax1)
        ax2.plot( t, m1, 'b-', lw=2 )
        ax2.fill_between( t, m1-s1, m1+s1,
                          facecolor='b',
                          alpha=0.4,
                          linewidth=0 )

        # find crossover point where on->off is dimmer than off->on
        cond = m0 < m1
        idx = np.nonzero(cond)[0][0]
        t_crossover = t[idx]

        ax1.axvline( t_crossover )
        ax2.axvline( t_crossover )

        trans = blended_transform_factory(ax1.transData, ax1.transAxes)
        ax1.text(t_crossover, 0.9, 'crossover: %.1f msec'%t_crossover,
                 transform=trans,
                 horizontalalignment='center')
        ax1.set_title('ON->OFF')
        ax2.set_title('OFF->ON')

        ax2.set_xlabel( 'time (msec)')
        ax2.set_ylabel( 'luminance (DAC units)')
        ax1.set_ylabel( 'luminance (DAC units)')
        ax2.set_xlim( (-20,150))

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

        fig = plt.figure()
        ax1 = fig.add_subplot(2,1,1)
        for data in accum[0]:
            ax1.plot( t, data, alpha=0.4 )
        ax2 = fig.add_subplot(2,1,2)
        for data in accum[1]:
            ax2.plot( t, data, alpha=0.4 )

    plt.show()
