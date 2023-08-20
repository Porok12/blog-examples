import argparse
import os
import socket
import sqlite3
import struct
from datetime import datetime

import requests
# You will need to install influxdb3-python to use influxdb
# from influxdb_client_3 import InfluxDBClient3, Point


def forward_data(temperature: float, humidity: float) -> None:
    # Save measurements to local database
    connection = sqlite3.connect('/home/pi/sensordata.db')
    cursor = connection.cursor()
    cursor.execute('INSERT INTO sensordata VALUES (?, ?, ?)', (temperature, humidity, datetime.now()))
    connection.commit()
    cursor.close()
    connection.cursor()

    # Save data to external database
    # token = os.getenv('INFLUXDB_TOKEN')
    # org = os.getenv('INFLUXDB_ORG', 'N/A')
    # host = os.getenv('INFLUXDB_HOST', 'https://eu-central-1-1.aws.cloud2.influxdata.com')
    # database = 'sensordata'
    # client = InfluxDBClient3(host=host, token=token, org=org)
    # point = (
    #     Point('pico')
    #     .tag('location', 'greenhouse')
    #     .field(temperature, humidity)
    # )
    # client.write(database=database, record=point)
    # client.close()

    # Send notifications
    data = {temperature, humidity}
    event = 'sensordata'
    webhooks_key = os.getenv('WEBHOOK_KEY')
    requests.post(f'https://maker.ifttt.com/trigger/{event}/json/with/key/{webhooks_key}', json=data)


def main(config: argparse.Namespace) -> None:
    buffer_size = 8
    payload_size = 8  # Specific to what PICO sends

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind((config.ip, config.port))
        server.listen(1)
        print('Listening for client...')

        while True:
            try:
                conn, addr = server.accept()
                print(f"Connected by {addr}")
                while True:
                    data = conn.recv(buffer_size)
                    if len(data) == payload_size:
                        humidity, temperature = struct.unpack('ff', data)
                        print(f"Temp: {temperature}, Humidity: {humidity}")
                        forward_data(temperature, humidity)
                        if not data:
                            break
                    else:
                        conn.close()
                        break
            except KeyboardInterrupt:
                print('Exiting...')
                server.close()
                break


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='TCP server for PICO')
    parser.add_argument('-p', '--port', type=int, default=8008, required=False, help='port on which server is open')
    parser.add_argument('--ip', type=str, default='', required=False,
                        help='ip on which server is working (default: '')')
    args = parser.parse_args()

    main(args)
