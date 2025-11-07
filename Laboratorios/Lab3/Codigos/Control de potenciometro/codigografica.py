import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import deque


SERIAL_PORT = 'COM3'  
BAUD_RATE = 9600
MAX_DATA_POINTS = 100 



times = deque(maxlen=MAX_DATA_POINTS)
data_ref = deque(maxlen=MAX_DATA_POINTS)
data_actual = deque(maxlen=MAX_DATA_POINTS)
data_pwm = deque(maxlen=MAX_DATA_POINTS)


time_counter = 0

try:
    
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    ser.flushInput()
    print(f"Conectado a {SERIAL_PORT}...")

except serial.SerialException as e:
    print(f"Error al abrir el puerto {SERIAL_PORT}: {e}")
    print("Por favor, verifica el nombre del puerto y que no esté siendo usado.")
    exit()



fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)
fig.suptitle('Monitoreo de Servomotor')


ax1.set_ylabel('Posición (0-1023)')
line_ref, = ax1.plot([], [], lw=2, label='Referencia (Pot 1)')
line_actual, = ax1.plot([], [], lw=2, label='Actual (Pot 2)')
ax1.legend()
ax1.grid(True)
ax1.set_ylim(-10, 1034) 


ax2.set_ylabel('PWM (0-255)')
line_pwm, = ax2.plot([], [], lw=2, label='PWM', color='r')
ax2.legend()
ax2.grid(True)
ax2.set_ylim(-10, 265) 

ax2.set_xlabel('Muestras')

def update_plot(frame):
    global time_counter
    try:
        if ser.inWaiting() > 0:
           
            line = ser.readline().decode('utf-8').rstrip()
            
            
            parts = line.split(',')
            
            if len(parts) == 4:
                
                ref_val = int(parts[0])
                actual_val = int(parts[1])
                pwm_val = int(parts[2])
                
                
                times.append(time_counter)
                data_ref.append(ref_val)
                data_actual.append(actual_val)
                data_pwm.append(pwm_val)
                
                time_counter += 1

               
                line_ref.set_data(times, data_ref)
                line_actual.set_data(times, data_actual)
                line_pwm.set_data(times, data_pwm)
                
               
                ax1.set_xlim(times[0], times[-1])

    except Exception as e:
        print(f"Error leyendo o parseando datos: {e}")

    return line_ref, line_actual, line_pwm

ani = FuncAnimation(fig, update_plot, blit=True, interval=10, cache_seq_False)

print("Iniciando gráfica... Mueve el potenciómetro de referencia.")
plt.show()

ser.close()
print("Conexión serial cerrada.")