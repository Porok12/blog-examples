import socket
import struct


if __name__ == '__main__':
    target_host = '192.168.1.62'
    target_post = 8008

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((target_host, target_post))
    client.send(struct.pack('ff', 30.0, 50.0))
    client.close()
