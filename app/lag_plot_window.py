import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg

class LagPlotCanvas(FigureCanvasGTK3Agg):
    def __init__(self):
        self.fig = plt.Figure()

        super(LagPlotCanvas,self).__init__(self.fig)

        self.ax = self.fig.add_subplot(1,1,1)

    def new_data(self,t,accum):
        m0 = np.mean( accum, axis=0 )
        s0 = np.std( accum, axis=0 )

        if t.shape != m0.shape:
            # not enough data yet?
            return {}

        self.ax.cla()
        self.ax.plot( t, m0, 'b-', lw=2 )
        self.ax.fill_between( t, m0-s0, m0+s0,
                              facecolor='b',
                              alpha=0.4,
                              linewidth=0 )
        return {'mean':m0}

    def show_lag(self, t_crossover):
        self.ax.axvline( t_crossover )
