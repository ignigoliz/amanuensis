import argparse
import os
from DataConverter import DataConverter

class Parser:

    MAX_ADDR = 32767  # 15 bit address register


    def __init__(self):
        self.parser = self.create_parser()
        self.converter = DataConverter()


    def create_parser(self):
        parser = argparse.ArgumentParser(usage='nuensis [-h] {read/write} ...')
        parser.add_argument('--wd', nargs='?', default=os.getcwd(), help='NO NEED TO USE. working directory from which this command is being executed')
        subparser = parser.add_subparsers(dest='command')
        read = subparser.add_parser('read', help='read from EEPROM', usage = 'nuensis read [-h] [-w | -a ADDRESS | -r ADDRESS1 ADDRESS2]')
        write = subparser.add_parser('write', help='write to EEPROM', usage = 'nuensis write [-h] [-f FILE | -v ADDRESS VALUE | -w VALUE]')

        read_group = read.add_mutually_exclusive_group()

        read_group.add_argument('-w', '--whole', action='store_true', help='read whole EEPROM content')
        read_group.add_argument('-a', '--address', type=str, nargs=1,
                                help='read a specific 2-byte. Ex: ab12', metavar='ADDRESS')
        read_group.add_argument('-r', '--range', type=str, nargs=2,
                                help='read a range between two 2-byte addresses (endpoints included)', metavar=('ADDRESS1', 'ADDRESS2'))

        write_group = write.add_mutually_exclusive_group()
        write_group.add_argument('-f', '--file', type=str, nargs=1,
                                help='write a file.bin, passed as a PATH or located in the current folder', metavar='FILE')
        write_group.add_argument('-a', '--address', type=str, nargs=2,
                                help='write the specified 2-byte address with a 1-byte value. Ex: 01ab ea', metavar=('ADDRESS', 'VALUE'))
        write_group.add_argument('-w', '--whole', type=str, nargs=1,
                                help='write the specified 1-byte value to the whole EEPROM addresses. Ex: ea', metavar=('VALUE'))
        return parser


    def parse(self):
        return self.parser.parse_args()


    def msg(name=None):                                                            
        return '''nuensis [-h] '''


    def valid_address(self, addr_hex_str):
        try:
            addr_int = self.converter._hex_str_to_int(addr_hex_str)
            if addr_int <= self.MAX_ADDR and addr_int >= 0:
                return True
            else:
                print("Wrong Address:")
                print("Address must be positive and 15-bit long at maximum (smaller than 0x7fff):")
        except:
            print("Wrong Address:")
            
        return False


    def valid_value(self, value_hex_str):
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


    def valid_arguments(self, args):
        if args.command == "read":
            if args.address:
                return self.valid_address(args.address[0])
            elif args.range:
                return self.valid_address(args.range[0]) and self.valid_address(args.range[1])
        elif args.command == "write":
            if args.address:
                return self.valid_value(args.address[1]) and  self.valid_address(args.address[0])
            if args.whole:
                return self.valid_value(args.whole[0])
    