import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg

class LagPlotCanvas(FigureCanvasGTK3Agg):
    def __init__(self):
        self.fig = plt.Figure()

        super(LagPlotCanvas,self).__init__(self.fig)

        self.ax = self.fig.add_subplot(1,1,1)
        self.ax.set_xlim(-10,90)
        self.ax.set_xlabel('time (milliseconds)')
        self.ax.set_ylabel('luminance (ADC units)')
        self.did_once = False
        self.vline = None

    def new_data(self,t,accum):
        m0 = np.mean( accum, axis=0 )
        s0 = np.std( accum, axis=0 )

        if t.shape != m0.shape:
            # not enough data yet?
            return {}

        if self.did_once:
            self.line.set_xdata(t)
            self.line.set_ydata(m0)

            self.line_low.set_xdata(t)
            self.line_low.set_ydata(m0-s0)

            self.line_high.set_xdata(t)
            self.line_high.set_ydata(m0+s0)

            self.ax.relim()
            self.ax.autoscale_view(False, False, True)
        else:
            self.line, = self.ax.plot( t, m0, 'b-', lw=2 )
            # self.poly_collection = self.ax.fill_between( t, m0-s0, m0+s0,
            #                                              facecolor='b',
            #                                              alpha=0.4,
            #                                              linewidth=0 )
            self.line_low,  = self.ax.plot(t, m0-s0, 'b-', alpha=0.4 )
            self.line_high, = self.ax.plot(t, m0+s0, 'b-', alpha=0.4 )
            self.did_once = True
        return {'mean':m0}

    def show_lag(self, t_crossover):
        if self.vline is None:
            self.vline = self.ax.axvline( t_crossover, color='k')
        else:
            self.vline.set_xdata([t_crossover])

        xmin,xmax = self.ax.get_xlim()
        if t_crossover <= xmin:
            xmin = t_crossover-5
            self.ax.set_xlim(xmin,xmax)
        elif t_crossover >= xmax:
            xmax = t_crossover+5
            self.ax.set_xlim(xmin,xmax)
