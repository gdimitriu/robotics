"""
 esp32-wroom and wifi

 wifi (the main entry point for wifi communication)

 Copyright 2025 Gabriel Dimitriu
  This file is part of raspberryPicoDroids project.

 robotics is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 robotics is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with robotics; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
"""
import network
import socket
import time


def init_wifi():
    """
    initialize the wifi connection
    this should be called only once at startup
    :return: socket for communication
    """
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect("YOUR-SSID", "YOUR-PASSWORD")

    max_wait = 10
    while True:
        if wlan.isconnected():
            break
        max_wait -= 1
        print('waiting for connection...')
        time.sleep(1)

    if not wlan.isconnected():
        raise RuntimeError('net connection failed')
    else:
        print('connected')
        status = wlan.ifconfig()
        print('ip = ' + status[0])

    addr = socket.getaddrinfo('0.0.0.0', 4242)[0][-1]

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(addr)
    sock.listen(1)
    print('listening on', addr)
    return sock


def receive_data(sock):
    """
    this is the main entry point for receiving command from the robot control center
    this should be called in infinite loop on core 0
    :param sock:  the socket for communication received from init_wifi
    """
    # Listen for connections
    try:
        cl, addr = sock.accept()
        print('client connected from', addr)
    except OSError as e:
        cl.close()
        print('connection closed')
        return
    try:
        while True:
            request = cl.readline(1024)
            print(request)
            if request == b"exit#\n":
                cl.close()
                print('connection closed')
                return
            cl.send(request)
    except OSError as e:
        cl.close()
        print('connection closed')
        return
