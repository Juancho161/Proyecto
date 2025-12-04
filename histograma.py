#!/usr/bin/env python3
"""
@file histograma.py  
@brief Histogramas de distribución de partículas
"""

import zmq
import numpy as np
import matplotlib.pyplot as plt
import time

class HistogramaSimple:
    def __init__(self):
        self.ctx = zmq.Context()
        self.socket = self.ctx.socket(zmq.SUB)
        self.socket.connect("tcp://localhost:5555")
        self.socket.setsockopt(zmq.SUBSCRIBE, b"")
        self.socket.setsockopt(zmq.RCVTIMEO, 50)
        
        self.L = None
        self.num_bins = 40
        
        plt.ion()
        self.fig, self.axes = plt.subplots(1, 3, figsize=(15, 5))
        self.fig.canvas.manager.set_window_title('Histogramas de Distribución')
        
        self.hist_xy = None
        self.hist_xz = None
        self.hist_yz = None
        
        self.im_xy = None
        self.im_xz = None
        self.im_yz = None
    
    def setup_plots(self):
        zeros = np.zeros((self.num_bins, self.num_bins))
        
        self.im_xy = self.axes[0].imshow(
            zeros, cmap='plasma', origin='lower',
            extent=[0, self.L, 0, self.L], aspect='auto'
        )
        self.axes[0].set_title('Plano XY')
        self.axes[0].set_xlabel('X')
        self.axes[0].set_ylabel('Y')
        
        self.im_xz = self.axes[1].imshow(
            zeros, cmap='plasma', origin='lower',
            extent=[0, self.L, 0, self.L], aspect='auto'
        )
        self.axes[1].set_title('Plano XZ')
        self.axes[1].set_xlabel('X')
        self.axes[1].set_ylabel('Z')
        
        self.im_yz = self.axes[2].imshow(
            zeros, cmap='plasma', origin='lower',
            extent=[0, self.L, 0, self.L], aspect='auto'
        )
        self.axes[2].set_title('Plano YZ')
        self.axes[2].set_xlabel('Y')
        self.axes[2].set_ylabel('Z')
        
        self.hist_xy = np.zeros((self.num_bins, self.num_bins), dtype=np.float32)
        self.hist_xz = np.zeros((self.num_bins, self.num_bins), dtype=np.float32)
        self.hist_yz = np.zeros((self.num_bins, self.num_bins), dtype=np.float32)
        
        self.fig.tight_layout()
        self.fig.canvas.draw()
    
    def update(self):
        try:
            msg = self.socket.recv(zmq.NOBLOCK)
            data = np.frombuffer(msg, dtype=np.float64)
            
            if len(data) > 4:
                nuevo_L = data[0]
                
                if self.L is None:
                    self.L = nuevo_L
                    self.setup_plots()
                    print(f"L = {self.L:.1f}")
                
                particles = data[1:].reshape(-1, 4)
                
                if len(particles) > 0 and self.L is not None:
                    x = particles[:, 0]
                    y = particles[:, 1]
                    z = particles[:, 2]
                    
                    x = np.clip(x, 0.01, self.L - 0.01)
                    y = np.clip(y, 0.01, self.L - 0.01)
                    z = np.clip(z, 0.01, self.L - 0.01)
                    
                    hxy, _, _ = np.histogram2d(x, y, bins=self.num_bins, range=[[0, self.L], [0, self.L]])
                    hxz, _, _ = np.histogram2d(x, z, bins=self.num_bins, range=[[0, self.L], [0, self.L]])
                    hyz, _, _ = np.histogram2d(y, z, bins=self.num_bins, range=[[0, self.L], [0, self.L]])
                    
                    self.hist_xy += hxy
                    self.hist_xz += hxz
                    self.hist_yz += hyz
                    
                    if self.im_xy is not None:
                        self.im_xy.set_data(self.hist_xy.T)
                        self.im_xz.set_data(self.hist_xz.T)
                        self.im_yz.set_data(self.hist_yz.T)
                        
                        self.im_xy.autoscale()
                        self.im_xz.autoscale()
                        self.im_yz.autoscale()
                        
                        self.fig.canvas.draw_idle()
                        return True
                        
        except zmq.Again:
            pass
        
        return False
    
    def run(self):
        try:
            while plt.fignum_exists(self.fig.number):
                if self.update():
                    plt.pause(0.001)
                else:
                    plt.pause(0.05)
        except KeyboardInterrupt:
            print("\n Histograma terminado")
        finally:
            plt.close('all')
            self.socket.close()
            self.ctx.term()

if __name__ == "__main__":
    hist = HistogramaSimple()
    hist.run()
