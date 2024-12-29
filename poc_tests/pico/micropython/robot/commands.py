from engines import break_engines, go
import configuration


class Command:
    def __init__(self, sock):
        self.current_power = configuration.MAX_POWER
        self.min_power = configuration.MIN_POWER
        self.max_power = configuration.MAX_POWER
        self.sock = sock

    def move_data(self, request):
        print("MoveData(%s)" % request)
        if len(request) == 1:
            self.command_without_data(request)
        else:
            self.command_with_data(request.decode("ascii"))

    def command_without_data(self, request):
        print("CommandWithoutData(%s)" % request)
        if request == b"v":
            self.sock.send("%g\r\n" % self.min_power)
        elif request == b'V':
            self.sock.send("%g\r\n" % self.max_power)
        elif request == b'c':
            self.sock.send("%g\r\n" % self.current_power)
        elif request == b'd':
            self.sock.send("unsupported\r\n")
        elif request == b's':
            self.sock.send("unsupported\r\n")

    def command_with_data(self, request):
        print("CommandWithData(%s)" % request)
        if request[0] == 'V':
            self.max_power = int(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 'v':
            self.min_power = int(request[1:])
            self.sock.send("OK\r\n")
        elif request[0] == 'c':
            if self.max_power >= int(request[1:]) >= self.min_power:
                self.current_power = int(request[1:])
                self.sock.send("OK\r\n")
            else:
                self.sock.send("OK\r\n")
        else: # unssuported command
            self.sock.send("OK\r\n")
