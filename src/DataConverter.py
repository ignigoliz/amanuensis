
class DataConverter:
  """ Addresses occupy 2 bytes (e.g. 0xABCD) and values 1 byte (e.g. 0xEF)
      hex_str: "ABCD"
      hex: b'\xde\xad'
      int: 17 (==0x11)
  """


  def __init__(self):
      pass


  def _hex_to_hex_str(self, hex_value, type="address"):
      int_value =  int.from_bytes(hex_value, "big")
      return self._int_to_hex_str(int_value, type)


  def _hex_str_to_hex(self, str):
      return bytearray.fromhex(str)


  def _hex_str_to_int(self, str):
        return int(str, 16)

      
  def _int_to_hex_str(self, int, type="address"):
        if type=="address":
            return f'{int:04x}'
        elif type=="value":
            return f'{int:02x}'
  

  def _int_to_hex(self, int):
      return self._hex_str_to_hex(self._int_to_hex_str(int))


  def _hex_to_int(self, hex):
      return self._hex_str_to_int(self._hex_to_hex_str(hex))