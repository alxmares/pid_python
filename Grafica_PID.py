"""
MAESTRÍA EN INGENIERÍA ELECTRÓNICA Y COMPUTACIÓN

Autor: Francisco Alex Mares Solano

Descripción: Gráfica en tiempo real de un control 
PID de temperatura.

* NOTA: Modificar la línea 21 para obtener datos del
* puerto serial correspondiente.

"""
import serial
from serial.serialutil import SerialException
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import re
import time

try:
    arduino = serial.Serial('COM6', 115200) # <--- Colocar el puerto deseado
except SerialException:
    print('No se pudo encontrar la conexión.')

plt.style.use('dark_background')
fig = plt.figure()
ax = fig.add_subplot(2,1,1)
ax2 = fig.add_subplot(2,1,2)

xdatos,ydatos,y2datos,y3datos = [],[],[],[]
xtick1 = 1
xtick2 = 500

def animate(i,xdatos,ydatos,y2datos,y3datos):
    global xtick1, xtick2
    
    target, celsius, control = arduinoread()
    
    xdatos.append(i)
    ydatos.append(int(target))
    y2datos.append(int(celsius))
    y3datos.append(int(control))

    
    # GRÁFICA 1
    ax.clear()
    ax.plot(xdatos, ydatos, label='Target (°C)')  # Gráfica para 'Target'
    ax.plot(xdatos, y2datos, label='Temperatura (°C)')  # Gráfica para 'Temperatura'
    
    # GRÁFICA 2
    ax2.clear()
    ax2.plot(xdatos, y3datos, label='Control')  # Gráfica para 'Target'
    
    # Ajustes para 'ax'
    ax.set_title('SetPoint vs Actual')  # Título para ax
    ax.set_ylabel('Temperatura (°C)')  # Etiqueta del eje Y para ax
    ax.grid()  # Grid para ax
    ax.legend()  # Leyenda para ax
    ax.axis([xtick1, xtick2, 15, 50])  # Limites para ax

    # Ajustes para 'ax1'
    ax2.set_title('Control PWM (Inverso)')  # Si quieres un título diferente para ax1
    ax2.set_ylabel('Valor de control PWM')  # Si necesitas una etiqueta diferente para ax1
    ax2.grid()  # Grid para ax1
    ax2.legend()  # Leyenda para ax1
    ax2.axis([xtick1, xtick2, 0, 255])  # Limites para ax1
    
    if i == xtick2: 
        xtick1 += 500
        xtick2 += 500

# *************************
# * * * L E C T U R A * * *
# *************************
def arduinoread():
    read = str(arduino.readline())
    read = read.split('C')
    x1 = read[1]
    x2 = read[2]
    x3 = re.findall(r'\d+',read[3])[0]
    
    return x1,x2,x3

# ***************************************
# * * * S I N C R O N I Z A C I Ó N * * *
# ***************************************
is_sinc = False
def sinc():
    global is_sinc
    time.sleep(1)
    while True:
        arduino.write(b'B')
        read = arduino.read()
        print(read)
        if 'A' in str(read):
            print('Hay sincronización con Arduino: COM3')
            is_sinc = False
            return True

if __name__ == '__main__':
    if is_sinc == False:
        sinc()
    while True:
        ani = animation.FuncAnimation(fig,animate,fargs=(xdatos,ydatos,y2datos,y3datos))
        plt.show()
        
