import struct,subprocess,tempfile,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class FirmwareValidation(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.original=(ROOT/'build/pico/db48x-picocalc-pico2w.uf2').read_bytes()
    def validate(self,data):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/'test.uf2';p.write_bytes(data)
            r=subprocess.run(['python3',str(ROOT/'tools/validate_uf2.py'),str(p)],capture_output=True)
            return r.returncode
    def change(self,offset,value):
        data=bytearray(self.original);struct.pack_into('<I',data,offset,value);return data
    def test_original(self): self.assertEqual(self.validate(self.original),0)
    def test_wrong_application(self):
        self.assertIn(b"LumaRPL",self.original)
        self.assertNotEqual(self.validate(self.original.replace(b"LumaRPL",b"Unknown")),0)
    def test_incomplete(self): self.assertNotEqual(self.validate(self.original[:-1]),0)
    def test_wrong_chip(self): self.assertNotEqual(self.validate(self.change(512+28,0xe48bff56)),0)
    def test_bad_dummy_address(self): self.assertNotEqual(self.validate(self.change(12,0x10000000)),0)
    def test_payload_outside_partition(self): self.assertNotEqual(self.validate(self.change(512+12,0x10400000)),0)
    def test_missing_first_block(self): self.assertNotEqual(self.validate(self.original[:512]+self.original[1024:]),0)
    def test_wrong_sequence(self): self.assertNotEqual(self.validate(self.change(1024+20,0)),0)
if __name__=='__main__':unittest.main()
