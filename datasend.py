#!/usr/bin/python3
import serial
import paho.mqtt.client as mqtt
import json

# Configuración del puerto serie
serial_port = "/dev/ttyACM1"  # Reemplaza con el puerto serie correcto
baudrate = 115200  # Ajusta la velocidad de baudios según corresponda

# Configuración de conexión MQTT
broker = "iot.eie.ucr.ac.cr"
port = 1883
topic = "v1/devices/me/telemetry"
username = "STM32_Kevin_Andres"
password = "mwcikgtyf2934lfc9tju"

# Configuración del cliente MQTT
client = mqtt.Client()
client.username_pw_set(password)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conexión exitosa al broker MQTT")
    else:
        print("Error al conectarse al broker MQTT")

def on_log(client, userdata, level, buf):
    print("log:", buf)

def on_message(client, userdata, message):
    print("received message =", str(message.payload.decode("utf-8")))
client.on_message = on_message
client.on_log = on_log
client.on_connect = on_connect

# Conexión al broker MQTT
client.connect(broker, port)

# Configuración del puerto serie

ser = serial.Serial(serial_port, baudrate)
client.subscribe(topic)
client.loop_start()
while True:
    if ser.inWaiting():

        # Ejemplo de línea recibida: "123 456 789"
        x = ser.readline(16).decode('utf-8')
        y = ser.readline(16).decode('utf-8')
        z = ser.readline(16).decode('utf-8')
        print(x)
        print(y)
        print(z)   
        if len(x) >= 1:
            try:
                # Convertir los valores a números enteros
                valor_eje_x = int(x)
                valor_eje_y = int(y)
                valor_eje_z = int(z)

                # Crear el diccionario con los datos a enviar
                data = {
                    "gyro_x": valor_eje_x,
                    "gyro_y": valor_eje_y,
                    "gyro_z": valor_eje_z
                }
                # Convertir el diccionario a formato JSON
                payload = json.dumps(data)

                # Publicar los datos en el topic de ThingsBoard
                client.publish(topic, payload)
            except ValueError:
                print("Error al convertir los valores a números enteros")
    # Dividir la línea en partes separadas por espacios
    # Verificar si se recibieron los tres valores esperados
        

# Cerrar la conexión serial y desconectarse del broker MQTT al terminar
ser.close()
client.disconnect()
