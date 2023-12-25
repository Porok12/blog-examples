import argparse
import os
import socket
import sqlite3
import struct
import pathlib
from datetime import datetime
from psutil import process_iter
from signal import SIGTERM

import requests
# You will need to install influxdb3-python to use influxdb
# from influxdb_client_3 import InfluxDBClient3, Point


def forward_data(config: argparse.Namespace, temperature: float, humidity: float) -> None:
    # Save measurements to local database
    if config.db:
        connection = sqlite3.connect(config.db)
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
    data={'value1': f'Humidity {humidity:.2f}, Temperature {temperature:.2f}'}
    event = 'sensordata'
    webhooks_key = os.getenv('WEBHOOK_KEY')
    requests.post(f'https://maker.ifttt.com/trigger/{event}/with/key/{webhooks_key}', json=data)


def main(config: argparse.Namespace) -> None:
    buffer_size = 8
    payload_size = 8  # Specific to what PICO sends

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind((config.ip, config.port))
        server.listen(1)
        print('Listening for client...')

        try:
            while True:
                try:
                    conn, addr = server.accept()
                    print(f"Connected by {addr}")
                    while True:
                        data = conn.recv(buffer_size)
                        if len(data) == payload_size:
                            humidity, temperature = struct.unpack('ff', data)
                            print(f"Temp: {temperature}, Humidity: {humidity}")
                            forward_data(config, temperature, humidity)
                            if not data:
                                break
                        else:
                            conn.close()
                            break
                except KeyboardInterrupt:
                    print('Exiting...')
                    break
        finally:
            server.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='TCP server for PICO')
    parser.add_argument('-p', '--port', type=int, default=8008, required=False, help='port on which server is open')
    parser.add_argument('--ip', type=str, default='', required=False,
                        help='IP on which server is working (default: '')')
    parser.add_argument('--db', type=pathlib.Path, required=False, help='SQLite database location')
    args = parser.parse_args()

    # # Make sure port is free
    # for proc in process_iter():
    #     for conns in proc.connections(kind='inet'):
    #         if conns.laddr.port == args.port:
    #             proc.send_signal(SIGTERM)

    main(args)
