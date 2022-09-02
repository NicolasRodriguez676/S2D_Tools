from time import sleep

import queue
import serial


class S2DLog:
    def __init__(self):
        self.port_config_queue = queue.Queue(maxsize=2)
        self.text_queue = queue.Queue()
        self.disconnect_queue = queue.Queue(maxsize=1)

        self.ansi_colors = {
            #             R    G    B
            "\033[30m": (000, 000, 000),   # black
            "\033[31m": (187, 000, 000),   # red
            "\033[32m": (000, 187, 000),   # green
            "\033[33m": (187, 187, 000),   # yellow
            "\033[34m": (000, 000, 187),   # blue
            "\033[35m": (187, 000, 187),   # magenta
            "\033[36m": (000, 187, 187),   # cyan
            "\033[37m": (187, 187, 187),   # white
        }

        self.tty = serial.Serial()

    def listener(self):
        self.wait_for_user()
        log = ""

        while True:
            if self.disconnect_queue.full():
                self.disconnect_queue.get()

                if self.tty.isOpen():
                    self.tty.close()
                    print("closed logger port")
                else:
                    print("port already disconnected")

                self.wait_for_user()

            while self.tty.in_waiting > 0:
                char = self.tty.read().decode("utf-8")

                if char == '\n':
                    res = self.get_log_color(log)
                    self.text_queue.put(res[0])
                    self.text_queue.put(res[1])

                    log = ""
                else:
                    log += char

            sleep(0.1)

    def get_log_color(self, log):
        log_color = 0

        for color in self.ansi_colors:
            log_color = log.find(color, 0, len(color))

            if not log_color == -1:
                log_color = log[:len(color)]
                break

        return [log[len(log_color):], self.ansi_colors[log_color]]

    def wait_for_user(self):
        while not self.port_config_queue.full():
            sleep(0.1)

        port = self.port_config_queue.get()
        baud = int(self.port_config_queue.get())

        print(f"logger port={port}")
        print(f"logger baud={baud}")

        self.tty = serial.Serial(port=port, baudrate=baud, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                                 stopbits=serial.STOPBITS_ONE, xonxoff=True, rtscts=False)

        print("opened logger port")

    def __del__(self):
        if self.tty.isOpen():
            self.tty.close()
