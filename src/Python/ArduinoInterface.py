from lib2to3.pytree import convert
import os
import serial  # pip3 install pyserial
import sys
from DataConverter import DataConverter
import urllib.parse


class ArduinoInterface:
    """ Communication starts with a handshake that sends the mode to be executed. Afterwards, information dependent on the chosen mode is exchanged.
        Values occupy 1 byte , while addresses 2.
    """

    port: serial.Serial
    MAX_ADDR = 32767  # 15 bit address register

    def __init__(self):
        self.converter = DataConverter()
        self._attempt_connection()
        

    def _attempt_connection(self):
        self._print_and_clear("Connecting to Arduino...")

        dev = '/dev/' + os.popen("ls /dev/ | grep cu.usbmodem").read().rstrip()
        try:
            self.port = serial.Serial(port=dev, baudrate=9600, timeout=0.1)
        except:
            print("Connection to Arduino not available.")
            exit()

        self._wait_for_ACK()
        # print("Connection succesful")
        

    def _handshake(self, mode):
        # self._print_and_clear("Waiting for handshake response...")

        packet = bytearray()  # Sends communication details [R/W, n_bytes]
        packet += bytes(mode, encoding='ascii')

        self.port.write(packet)

        self._wait_for_ACK()
        
        
    def _print_and_clear(self, str):
        print(str, end='\r')
        sys.stdout.write("\033[K")  # Clear to the end of line
        
        
    def _print_block(self, current_block, block_index, value_str):
        current_block_str = self.converter._int_to_hex_str(current_block)
        
        if block_index == 0 :
            print("+----+--------------------------------------------------------------------------------+")
            print("|ADDR|                                      DATA                                      |")
            print("+----+--------------------------------------------------------------------------------+")
            
        to_print = "|"
        to_print += current_block_str[0:3]
        to_print += "_| "

        for ind, value in enumerate(value_str):
            to_print += value
            if ind == 9:
                to_print += " || "
            else:
                to_print += " | "

        print(to_print)
        to_print = ""
        print("+----+--------------------------------------------------------------------------------+")


    def _print_progress(self, progress):
        number_of_steps = 50
        increment = 100/number_of_steps
        steps_done = int(progress//increment)

        str = "["
        str += "#"*steps_done
        str += "."*(number_of_steps - steps_done)
        str += "]  "
        str += f"{progress}%"

        if progress<100:
            self._print_and_clear(str)
        else:
            print(str)


    def _read(self, addr_hex):
        packet = bytearray()
        packet += addr_hex
        self.port.write(packet)

        self._wait_for_ACK()        
        self._wait_until_data_in_buffer()

        value = self.port.read(1)

        self._send_ACK()

        return value


    def _read_bin_file(self, file):
        file = open(file, 'rb')

        values_to_write = []
        addresses_to_write = []

        address = 0
        byte = file.read(1)
        while byte:
            # if byte != b'\xea':  # NO-OP (0xea) excluded
            values_to_write.append(byte)
            addresses_to_write.append(self.converter._int_to_hex(address))
            
            byte = file.read(1)
            address += 1
        file.close()

        return [addresses_to_write, values_to_write]
    
    
    def _send_ACK(self):
        self.port.write(b'\x06')


    def _serial_buffer_empty(self):
        return self.port.in_waiting == 0
        

    def _wait_for_ACK(self):
        notReady = True
        while notReady:
            if self.port.inWaiting() >= 1:
                ack = self.port.read(1)
                if ack == b'\x06':
                    notReady = False
                else:
                    print("Expecting ACK. Got{}".format(ack))
                    exit()
               
               
    def _wait_until_data_in_buffer(self):
            while self._serial_buffer_empty(): pass
            
    
    def _write(self, addr_hex="", value_hex=""):

        packet = bytearray()
        if addr_hex != "":
            packet += addr_hex
        if value_hex != "":
            packet += value_hex
        self.port.write(packet)

        self._wait_for_ACK()

    # def _send(self, value_hex):
    #     packet = bytearray()
    #     packet += value_hex

    #     self.port.write(packet)

    #     self._wait_for_ACK()

    # -------------------------------------------------------------------------

    def read_routine(self, routine, *args):

        if routine == 'single':
            self._handshake(mode='R')
            addr_str = args[0]
            addr_hex = self.converter._hex_str_to_hex(addr_str)
            value = self._read(addr_hex)
            print(self.converter._hex_to_hex_str(value, type="value"))

        elif routine == 'range':
            beg_str = args[0]
            end_str = args[1]
            
            beg_int = self.converter._hex_str_to_int(beg_str)
            end_int = self.converter._hex_str_to_int(end_str)
            
            beg_block_memory = beg_int - beg_int%16
            end_block_memory = end_int - end_int%16 + 15
            
            amount_of_blocks = (end_block_memory+1 - beg_block_memory) // 16
            
            beg_block_memory_hex = self.converter._int_to_hex(beg_block_memory)
            end_block_memory_hex = self.converter._int_to_hex(end_block_memory)            
                    
            self._handshake(mode ='N')

            packet = bytearray()
            packet += beg_block_memory_hex
            packet += end_block_memory_hex
            self.port.write(packet)
    
            self._wait_for_ACK()

            for block_idx in range(amount_of_blocks):
                self._wait_until_data_in_buffer()
                data = self.port.read(16)
                self._send_ACK()

                data_list_str = [f"{hex_value:02x}" for hex_value in data]

                if block_idx == 0:
                    data_list_str[:beg_int%16] = ["--" for _ in range(beg_int%16)]
                if block_idx == amount_of_blocks-1:
                    data_list_str[end_int%16+1:] = ["--" for _ in range(15 - end_int%16)]


                current_block = beg_block_memory + 16*block_idx
            
                self._print_block(current_block, block_idx, data_list_str)


    def _combine_paths(self, file_path, working_directory):
        working_directory = working_directory.replace('"', '')
        if working_directory[-1] != '/':
            working_directory += "/"
        return urllib.parse.urljoin(working_directory, file_path)
        

    def write_routine(self, routine, *args):
        if routine == 'file':
            file_path = args[0]
            working_directory = args[1]

            full_file_path = self._combine_paths(file_path, working_directory)

            if not os.path.isfile(full_file_path):
                print(f"No such file: {full_file_path}")
                exit()

            addresses_to_write, values_to_write = self._read_bin_file(file_path)

            total_iter = len(addresses_to_write)
            current_iter = 1

            print(" ")
            
            for addr, val in zip(addresses_to_write, values_to_write):
                progress = int(current_iter/total_iter*100)
                self._print_progress(progress)
                self._handshake(mode='W')
                self._write(addr, val)
                current_iter+= 1

        elif routine == 'value':
            self._print_and_clear
            addr_str = args[0]
            value_str = args[1]

            self._handshake(mode='W')

            addr_hex = self.converter._hex_str_to_hex(addr_str)
            value_hex = self.converter._hex_str_to_hex(value_str)

            self._write(addr_hex, value_hex)
        
        elif routine == 'all':
            value_str = args[0]

            self._handshake(mode='A')

            value_hex = self.converter._hex_str_to_hex(value_str)
            self._write(value_hex=value_hex)

            while True:
                if not self._serial_buffer_empty():
                    msg = self.port.read(1)
                    if msg == b'\xFF':  # 0xFF sent by Arduino to mark EOF
                        break
                    else:
                        progress = self.converter._hex_to_int(msg)
                        self._print_progress(progress)

    
    def valid_arguments(self, args):
        def valid_address(addr_hex_str):
            try:
                addr_int = self.converter._hex_str_to_int(addr_hex_str)
                if addr_int <= self.MAX_ADDR and addr_int >= 0:
                    return True
                else:
                    print("Wrong Address:")
                    print("Address must be a positive 15-bit long HEX number (smaller than 0x7fff):")
            except:
                print("Wrong Address:")
                return False

        def valid_value(value_hex_str):
            try:
                value_int = self.converter._hex_str_to_int(value_hex_str)
                if value_int <= 255 and value_int >= 0:
                    return True
                else:
                    print("Wrong Value:")
                    print("Value must be a positive 8-bit HEX number (smaller than 0xff):")
            except:
                print("Wrong Value:")

            return False
            
        
        if args.command == "read":
            if args.address:
                return valid_address(args.address[0])
            elif args.range:
                return valid_address(args.range[0]) and valid_address(args.range[1])
        elif args.command == "write":
            if args.address:
                return valid_value(args.address[1]) and  valid_address(args.address[0])
            if args.whole:
                return valid_value(args.whole[0])
