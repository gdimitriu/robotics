import network
import socket
import configuration
import time
from commands import Command


def init_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(configuration.SSID, configuration.PASSWORD)

    max_wait = 10
    while max_wait > 0:
        if wlan.status() < 0 or wlan.status() >= 3:
            break
        max_wait -= 1
        print('waiting for connection...')
        time.sleep(1)

    if wlan.status() != 3:
        raise RuntimeError('net connection failed')
    else:
        print('connected')
        status = wlan.ifconfig()
        print('ip = ' + status[0])

    addr = socket.getaddrinfo('0.0.0.0', configuration.PORT)[0][-1]

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(addr)
    sock.listen(1)

    print('listening on', addr)
    return sock


def receive_data(sock):
    # Listen for connections
    try:
        cl, addr = sock.accept()
        print('client connected from', addr)
    except OSError as e:
        cl.close()
        print('connection closed')
        return
    command = Command(cl)
    try:
        while True:
            request = cl.readline(1024)
            print(request)
            if request != b'':
                command.move_data(request[:-2])
            else:
                cl.close()
                print('connection closed')
                return
    except OSError as e:
        cl.close()
        print('connection closed')
        return
