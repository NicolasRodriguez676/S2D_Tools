import glob
import sys
import dearpygui.dearpygui as dpg
import serial

import queue

file_port_config = queue.Queue(maxsize=2)
file_port_exit = queue.Queue()


def serial_ports():
    """ [STACKOVERFLOW:EDITED]
        Lists serial port names

        raises EnvironmentError
            On unsupported or unknown platforms
        returns
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/ttyUSB[0-9]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port=port, baudrate=115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE, xonxoff=True, rtscts=False)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def setup_fonts():
    with dpg.font_registry():
        default_font = dpg.add_font("/usr/share/fonts/truetype/cascadia-code/CascadiaCode.ttf", 22)

    dpg.bind_font(default_font)


def save_init():
    dpg.save_init_file("dpg.ini")
