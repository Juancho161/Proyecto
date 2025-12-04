#!/usr/bin/env python3
"""
@file visual.py
@brief Visualización 3D de la simulación
"""

import zmq
import numpy as np
from vispy import scene, app
import time

CTX = zmq.Context()
SOCK = CTX.socket(zmq.SUB)
SOCK.connect("tcp://localhost:5555")
SOCK.setsockopt(zmq.SUBSCRIBE, b"")
SOCK.setsockopt(zmq.RCVTIMEO, 50)

print("Iniciando")

canvas = scene.SceneCanvas(keys='interactive', show=True, 
                          bgcolor='white', size=(1400, 900),
                          title="Simulación de Partículas")

view = canvas.central_widget.add_view()
view.camera = scene.cameras.TurntableCamera(
    fov=50, azimuth=30, elevation=25, 
    distance=3500, up='+z'
)

is_running = True
frame_count = 0
last_time = time.time()
particle_scale_factor = 8.0
L = None
cube = None
markers = None

def crear_cubo(L):
    cube_vertices = np.array([
        [0,0,0], [L,0,0], [L,0,0], [L,L,0], [L,L,0], [0,L,0], [0,L,0], [0,0,0],
        [0,0,L], [L,0,L], [L,0,L], [L,L,L], [L,L,L], [0,L,L], [0,L,L], [0,0,L],
        [0,0,0], [0,0,L], [L,0,0], [L,0,L], [L,L,0], [L,L,L], [0,L,0], [0,L,L]
    ], dtype=np.float32)
    
    cube = scene.visuals.Line(
        pos=cube_vertices, 
        color=(0.3, 0.3, 0.3, 0.8), 
        parent=view.scene, 
        width=2, 
        connect='segments'
    )
    return cube

def crear_markers():
    markers = scene.visuals.Markers(parent=view.scene, scaling=True)
    markers.set_data(
        np.array([[0, 0, 0]], dtype=np.float32), 
        face_color=(0.2, 0.2, 0.8, 1), 
        size=10
    )
    return markers

def update(event):
    global frame_count, last_time, is_running, particle_scale_factor
    global L, cube, markers
    
    if not is_running:
        return
    
    frame_count += 1
    
    try:
        msg = SOCK.recv(zmq.NOBLOCK)
        data = np.frombuffer(msg, dtype=np.float64)
        
        if len(data) > 4:
            nuevo_L = data[0]
            
            if L is None or L != nuevo_L:
                L = nuevo_L
                
                if cube is not None:
                    cube.parent = None
                if markers is not None:
                    markers.parent = None
                
                cube = crear_cubo(L)
                markers = crear_markers()
                view.camera.distance = L * 1.75
                print(f"L = {L:.1f}")
            
            parts_data = data[1:].reshape(-1, 4)
            positions = parts_data[:, :3].astype(np.float32)
            radii = parts_data[:, 3]
            
            if len(positions) > 0 and markers is not None:
                sizes = radii * particle_scale_factor
                sizes = np.clip(sizes, 2.0, 40.0)
                
                markers.set_data(
                    pos=positions, 
                    face_color=(0.2, 0.2, 0.8, 1), 
                    size=sizes
                )
                
    except zmq.Again:
        pass
    
    current_time = time.time()
    if current_time - last_time >= 1.0:
        fps = frame_count / (current_time - last_time)
        frame_count = 0
        last_time = current_time
        canvas.title = f"Simulación | FPS: {fps:.1f} | Partículas: {len(positions) if 'positions' in locals() else 0}"

@canvas.events.key_press.connect
def on_key_press(event):
    global is_running, particle_scale_factor
    
    if event.key == 'Escape':
        is_running = False
        app.quit()
    elif event.key == 'p':
        particle_scale_factor *= 1.3
        print(f"Tamaño partículas: {particle_scale_factor:.1f}")
    elif event.key == 'o':
        particle_scale_factor /= 1.3
        print(f"Tamaño partículas: {particle_scale_factor:.1f}")

timer = app.Timer(interval=1.0/60.0, connect=update)
timer.start()

print("Visualizador listo (ESC para salir)")


try:
    app.run()
except KeyboardInterrupt:
    print("\n Programa terminado")
finally:
    is_running = False
    try:
        timer.stop()
        SOCK.close()
        CTX.term()
    except:
        pass
