import argparse
import os

class Parser:

    def __init__(self):
        self.parser = self.create_parser()


    def create_parser(self):
        parser = argparse.ArgumentParser(usage='nuensis [-h] [read | write] [-h]')
        subparser = parser.add_subparsers(dest='command')
        read = subparser.add_parser('read', help='read from EEPROM', usage = 'nuensis read [-h] [-w | -a ADDRESS | -r ADDRESS1 ADDRESS2]')
        write = subparser.add_parser('write', help='write to EEPROM', usage = 'nuensis write [-h] [-f FILE | -v ADDRESS VALUE | -w VALUE]')

        read_group = read.add_mutually_exclusive_group()

        read_group.add_argument('-w', '--whole', action='store_true', help='read all memory addresses.')
        read_group.add_argument('-a', '--address', type=str, nargs=1,
                                help='read a 2-byte memory address. Ex: nuensis read -a ab12', metavar='ADDRESS')
        read_group.add_argument('-r', '--range', type=str, nargs=2,
                                help='read contents between two 2-byte memory addresses. Ex: nuensis read -r 0001 000f', metavar=('ADDRESS1', 'ADDRESS2'))

        write_group = write.add_mutually_exclusive_group()
        write_group.add_argument('-f', '--file', type=str, nargs=1,
                                help='write a hexdump file.bin to the memory. Ex: nuensis write -f ../myfile.bin', metavar='FILE')
        write_group.add_argument('-a', '--address', type=str, nargs=2,
                                help='write a 1-byte value to a 2-byte address. Ex: nuensis write -a 01ab ea', metavar=('ADDRESS', 'VALUE'))
        write_group.add_argument('-w', '--whole', type=str, nargs=1,
                                help='write a 1-byte value to the all the memory addresses. Ex: nuensis write -w ea', metavar=('VALUE'))
        return parser


    def parse(self):
        return self.parser.parse_args()


    def msg(name=None):                                                            
        return '''nuensis [-h]'''
    