from time import sleep
from statemachine import StateMachine, State

import struct
import threading
import queue
import serial
import numpy as np

S2D_FILE_START = b"\\"
S2D_FILE_ACK = b"A"


class S2DStateMachine(StateMachine):
    port_disconnected = State("PortDisconnected", initial=True)

    waiting = State("Waiting",)
    start_transaction = State("StartTransaction")
    receive_ack = State("ReceiveAck")
    transmit_data = State("TransmitData")
    receive_data = State("ReceiveData")
    transmit_ack = State("TransmitAck")

    error = State("ErrorInSequence")

    connect_port = port_disconnected.to(waiting) | waiting.to(error)
    disconnect_port = waiting.to(port_disconnected) | waiting.to(error)
    new_event = waiting.to(start_transaction) | waiting.to(error)
    get_ack = start_transaction.to(receive_ack) | start_transaction.to(error)
    send_data = receive_ack.to(transmit_data) | receive_ack.to(error)
    get_data = transmit_data.to(receive_data) | transmit_data.to(error)
    send_ack = receive_data.to(transmit_ack) | receive_data.to(error)
    end_event = transmit_ack.to(waiting) | transmit_ack.to(error)


class S2DFile(threading.Thread, StateMachine):
    def __init__(self):
        threading.Thread.__init__(self)
        self.event = threading.Event()

        self.port_config_queue = queue.Queue(maxsize=2)
        self.disconnect_queue = queue.Queue(maxsize=1)
        self.data_queue = queue.Queue(maxsize=1)

        self.tty = serial.Serial()

        self.sm = S2DStateMachine()

        self.series_tag = ""
        self.size_tag = ""
        self.freq_tag = ""

        # length refers to number of elements
        # size refers to number of bytes to represent all elements
        self.length = 0
        self.size = 0
        self.freq = 0.0

    def listener(self):

        cs = self.sm.current_state

        while True:
            match cs:
                case self.sm.port_disconnected:
                    cs = self.sm_waiting_for_port(cs)

                case self.sm.waiting:
                    cs = self.sm_waiting_for_event(cs)

                case self.sm.start_transaction:
                    cs = self.sm_start_transaction(cs)

                case self.sm.receive_ack:
                    cs = self.sm_receive_ack(cs)

                case self.sm.transmit_data:
                    cs = self.sm_transmit_data(cs)

                case self.sm.receive_data:
                    cs = self.sm_receive_data(cs)

                case self.sm.transmit_ack:
                    cs = self.sm_transmit_ack(cs)

    def sm_transmit_ack(self, cs):
        print(f"state={cs.name}")

        self.tty.write(S2D_FILE_ACK)

        self.sm.end_event()
        return self.sm.current_state

    def sm_receive_data(self, cs):
        print(f"state={cs.name}")

        print(f"self.size={self.size}")

        data = list()
        temp = bytearray()

        while True:
            if self.tty.in_waiting > 0:
                temp += self.tty.read_all()
                if len(temp) >= self.size:
                    break

        if len(temp) == self.size:
            for i in range(0, self.length):
                data.append(np.frombuffer(temp[i * 4:i * 4 + 4], dtype=np.float32))

            self.data_queue.put(data)
        else:
            # TODO: Provide path back to waiting state
            print(f"invalid data length: {len(temp)}")

        self.sm.send_ack()
        return self.sm.current_state

    def sm_transmit_data(self, cs):
        print(f"state={cs.name}")

        temp_int = struct.pack("<I", self.length)
        temp_float = struct.pack("<f", self.freq)

        self.tty.write(temp_int)
        self.tty.write(temp_float)

        self.sm.get_data()
        return self.sm.current_state

    def sm_receive_ack(self, cs):
        print(f"state={cs.name}")

        while True:
            if self.tty.in_waiting > 0:
                self.tty.read().decode("utf-8")
                break

            sleep(0.1)

        self.sm.send_data()
        return self.sm.current_state

    def sm_start_transaction(self, cs):
        print(f"state={cs.name}")

        self.tty.write(S2D_FILE_START)

        self.sm.get_ack()
        return self.sm.current_state

    def sm_waiting_for_event(self, cs):
        print(f"state={cs.name}")

        while True:
            if self.event.is_set():
                break

            if self.disconnect_queue.full():
                break

            sleep(0.1)

        if self.disconnect_queue.full():
            self.disconnect_queue.get()

            if self.tty.isOpen():
                self.tty.close()
                print("closed file port")
            else:
                print("port already disconnected")

            self.sm.disconnect_port()
            return self.sm.current_state

        else:
            self.event.clear()

            self.sm.new_event()
            return self.sm.current_state

    def sm_waiting_for_port(self, cs):
        print(f"state={cs.name}")

        while not self.port_config_queue.full():
            sleep(0.1)

        port = self.port_config_queue.get()
        baud = int(self.port_config_queue.get())

        print(f"file port={port}")
        print(f"file baud={baud}")

        self.tty = serial.Serial(port=port, baudrate=baud, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                                 stopbits=serial.STOPBITS_ONE, xonxoff=True, rtscts=False)

        print("opened file port")

        self.sm.connect_port()
        return self.sm.current_state

    def __del__(self):
        if self.tty.isOpen():
            self.tty.close()
