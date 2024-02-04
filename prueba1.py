import serial
from serial.serialutil import SerialException
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import re
import time 

try:
    arduinoSerialData = serial.Serial('COM3',115200)
except SerialException:
    print('No se pudo encontrar la conexión.')
    
plt.style.use('dark_background')
fig = plt.figure()
ax = fig.add_subplot(1,1,1)

xdatos,ydatos,y2datos,y3datos = [],[],[],[]
xtick1 = 1
xtick2 = 500
sinc_1 = False

def animate(i,xdatos,ydatos,y2datos):
    global xtick1, xtick2
    
    datos = str(arduinoSerialData.readline())
    print(datos)
    xdatos.append(i)
    
    # Econtrar el valor de control y temperatura
    datos = datos.split('C')
    control = float(datos[1])
    #control = normalizar(control)
    temp = re.findall(r'\d+\.\d+',datos[2])
    
    
    ydatos.append(control)
    y2datos.append(float(temp[0]))
    
    ax.clear()
    ax.plot(xdatos,ydatos, label='Target (°C)')
    ax.plot(xdatos,y2datos,label='Temperatura (°C)')
    
    # Format plot
    plt.xticks(rotation=45,ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.title('Control de temperatura PID')
    plt.ylabel('Temperatura (°C)')
    plt.grid()
    plt.legend()
    plt.axis([xtick1,xtick2,15,50])# Usar para números arbitrarios de intentos
    
    if i == xtick2: 
        xtick1 += 500
        xtick2 += 500
    

# Acomplamiento entre Arduino y Python
def sinc():
    global sinc_1
    time.sleep(1)
    while True:
        arduinoSerialData.write(b'B')
        dato = (arduinoSerialData.readline())
        print(dato)
        if "A" in str(dato):
            print("Hay sincronización con Arduino: COM3")
            dato = (arduinoSerialData.readline())
            print(dato)
            print("Comenzó el programa")
            sinc_1 = True
            return True


if __name__ == '__main__':
    if sinc_1 == False: 
        sinc() 
    while True:
        ani = animation.FuncAnimation(fig,animate,fargs=(xdatos,ydatos,y2datos))
        plt.show()
        
