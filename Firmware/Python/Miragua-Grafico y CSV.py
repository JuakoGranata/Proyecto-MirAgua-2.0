# ----- Incluímos las librerías a emplear -----
import serial  # Permite realizar la comunicación serial (libreria pyserial)
import time  # Proporciona funciones relacionadas con el tiempo
import collections  # Implementa tipos de datos de contendores especializados
import matplotlib.pyplot as plt  # Permite realizar gráficos
import matplotlib.animation as animation  # Permite animar las gráficas
from matplotlib.lines import Line2D
import pandas as pd  # Permite trabajar con archivos CSV

# Configuración inicial
SerialPort = "COM3"  # Puerto serial de Arduino
baudRate = 115200  # Velocidad de baudios

# Inicializamos nuestro objeto Serial con los parámetros declarados
try:
    serialConnection = serial.Serial(SerialPort, baudRate)
except Exception as e:
    print(f"No se logró la conexión con el puerto: {e}")
    exit()

# Parámetros de la gráfica y datos
Samples = 200  # Número de muestras
sampleTime = 100  # Tiempo de muestreo en ms
numData = 5  # Número de sensores

# Límites de los ejes para nuestra gráfica
xmin = 0
xmax = Samples
ymin = [-50, 0]
ymax = [50, 100, 110]

# Estructuras para almacenar datos
lines = []
data = []
for i in range(numData):
    data.append(collections.deque([0] * Samples, maxlen=Samples))  # Buffer circular
    lines.append(Line2D([], [], color="green"))

# Crear DataFrame para almacenar datos
df = pd.DataFrame(columns=["Timestamp", "TempAmbiente", "HumAmbiente", "HumTierra", "IntLuz", "Gases"])

# Función para leer datos del puerto serie y actualizar gráficas -----
def getSerialData(self, Samples, numData, serialConnection, lines):
    global df  # Usar el dataframe global
    timestamp = time.strftime('%Y-%m-%d %H:%M:%S')  # Generar timestamp
    new_row = {"Timestamp": timestamp}  # Nueva fila para el DataFrame

    # Leer y procesar datos del puerto serie
    for i in range(numData):
        try:
            value = float(serialConnection.readline().strip())  # Leer dato
            data[i].append(value)  # Actualizar buffer de datos
            lines[i].set_data(range(Samples), data[i])  # Actualizar gráfica
            new_row[df.columns[i + 1]] = value  # Añadir dato al nuevo registro
        except ValueError:
            print("Error al convertir dato del puerto serie.")
            return

    # Agregar la nueva fila al DataFrame
    df = pd.concat([df, pd.DataFrame([new_row])], ignore_index=True)

# ----- Configuración de la gráfica -----
fig = plt.figure(figsize=(10, 6))
fig.canvas.manager.set_window_title('Miragua v2.0')
fig.subplots_adjust(hspace=0.6, wspace=0.3)

# Crear subgráficas
ax1 = fig.add_subplot(3, 2, 1, xlim=(xmin, xmax), ylim=(ymin[0], ymax[0]))
ax1.title.set_text("Temp Ambiente")
ax1.set_xlabel("Cant de muestras")
ax1.set_ylabel("Valor (°C)")
ax1.add_line(lines[0])

ax2 = fig.add_subplot(3, 2, 2, xlim=(xmin, xmax), ylim=(ymin[1], ymax[2]))
ax2.title.set_text("Humedad Ambiente")
ax2.set_xlabel("Cant de muestras")
ax2.set_ylabel("Valor (%)")
ax2.add_line(lines[1])

ax3 = fig.add_subplot(3, 2, 3, xlim=(xmin, xmax), ylim=(ymin[1], ymax[2]))
ax3.title.set_text("Humedad Tierra")
ax3.set_xlabel("Cant de muestras")
ax3.set_ylabel("Valor (%)")
ax3.add_line(lines[2])

ax4 = fig.add_subplot(3, 2, 4, xlim=(xmin, xmax), ylim=(ymin[1], ymax[2]))
ax4.title.set_text("Intensidad de Luz")
ax4.set_xlabel("Cant de muestras")
ax4.set_ylabel("Valor (Lumen)")
ax4.add_line(lines[3])

ax4 = fig.add_subplot(3, 2, 5, xlim=(xmin, xmax), ylim=(ymin[1], ymax[1]))
ax4.title.set_text("Gases")
ax4.set_xlabel("Cant de muestras")
ax4.set_ylabel("Valor (ppm)")
ax4.add_line(lines[4])

# ----- Animación y manejo de excepciones -----
try:
    anim = animation.FuncAnimation(fig, getSerialData, fargs=(Samples, numData, serialConnection, lines), interval=sampleTime)
    plt.show()
except KeyboardInterrupt:
    print("Interrumpido por el usuario.")
finally:
    # Guardar el DataFrame en un archivo CSV
    df.to_csv('Miragua.csv', index=False, encoding='utf-8-sig')
    serialConnection.close()  # Cerrar el puerto serie
    print("Datos guardados en 'Miragua.csv'. Puerto serie cerrado.")
