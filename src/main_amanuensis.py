from ArduinoInterface import ArduinoInterface
from CLIParser import Parser


if __name__ == "__main__":    
    parser = Parser()
    args = parser.parse()

    print(" \r")
    print(" \r")

    eeprom = ArduinoInterface()

    if args.command == 'read':
        if args.whole:
                print("READING WHOLE MEMORY:")
                eeprom.read_routine('range', "0000", "FFFF")
#            else:
#                print("Bad usage. Pass no parameters")
        elif args.address:
            if parser.valid_arguments(args):
                addr_str = args.address[0]
                print(f"READING ADDRESS 0x{addr_str}:\n")
                eeprom.read_routine('single', addr_str)
            else:
                print("Try [-a A], where A is a valid HEX Address (e.g. 10ef)")
        elif args.range:
            if parser.valid_arguments(args):
                begin_str = args.range[0]
                end_str = args.range[1]
                print(f"READING RANGE 0x{begin_str} TO 0x{end_str}:\n")
                eeprom.read_routine('range', begin_str, end_str)
            else:
                print("Try [-r A1 A2], where A_ is a valid HEX Address (e.g. 10ef)")
    elif args.command == 'write':
        if args.address:
            if parser.valid_arguments(args):
                addr_str = args.address[0]
                value_str = args.address[1]
                print(f"WRITING ADDRESS 0x{addr_str} WITH VALUE 0x{value_str}:")
                eeprom.write_routine('value', addr_str, value_str)
            else:
                print("Try [-v A V] where A is a valid HEX Address (e.g. 10ef)")
                print("and V(alue) a 1-byte HEX value (e.g. ab)")
        if args.file:
            print("FILE WRITTING:")
            filepath = args.file[0]
            working_directory = args.wd
            eeprom.write_routine('file', filepath, working_directory)
        if args.whole:
            if parser.valid_arguments(args):
                value = args.whole[0]
                print(f"OVERWRITTING WHOLE MEMORY WITH VALUE 0x{value}:")
                eeprom.write_routine('all', value)
            else:
                print("Try [-w V] , where V is a valid HEX Value (e.g. ab)")
    
    else:
        print("Introduce a valid mode! (read/write)")

    print("\r")