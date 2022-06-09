import argparse
import os

class Parser:

    def __init__(self):
        self.parser = self.create_parser()


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
    