import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest import mock
spec=importlib.util.spec_from_file_location('currency',Path(__file__).resolve().parents[1]/'tools/update_currency_rates.py')
fx=importlib.util.module_from_spec(spec);spec.loader.exec_module(fx)

class CurrencyTests(unittest.TestCase):
    def table(self,rate='1.5'):
        return fx.encode('Test fixture',[('EUR','2026-10-30','1'),('CAD','2026-10-29',rate)])
    def test_roundtrip(self):
        provider,rows=fx.decode(self.table());self.assertEqual(rows[1],('CAD','2026-10-29','1.5'))
    def test_corruption(self):
        data=self.table()
        for i in range(len(data)):
            bad=bytearray(data);bad[i]^=1
            with self.assertRaises(ValueError):fx.decode(bad)
    def test_semantics(self):
        for rate in ['0','-1','NaN','Infinity','1e4','1;2','1'*24,'0.0000000000001']:
            with self.assertRaises(ValueError):self.table(rate)
        for date in ['2026-02-30','2100-02-29','2026-1-01']:
            with self.assertRaises(ValueError):fx.encode('Test',[('CAD',date,'1')])
        with self.assertRaises(ValueError):fx.encode('Test',[('CAD','2026-01-01','1')]*2)
    def test_backup_and_failed_install(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'CURRENT.rates';old=self.table();new=self.table('1.6');fx.replace(path,old);fx.replace(path,new)
            self.assertEqual(path.with_suffix('.rates.bak').read_bytes(),old)
            path.write_bytes(b'corrupt')
            real_replace=fx.os.replace
            def fail(source,target):
                if target==path:raise OSError('Injected promotion failure')
                return real_replace(source,target)
            with mock.patch.object(fx.os,'replace',side_effect=fail):
                with self.assertRaises(OSError):fx.replace(path,new)
            self.assertEqual(path.with_suffix('.rates.bak').read_bytes(),old)
            fx.replace(path,new);self.assertEqual(path.read_bytes(),new);self.assertEqual(path.with_suffix('.rates.bak').read_bytes(),old)
            self.assertFalse(path.with_suffix('.rates.tmp').exists())
    def test_historical_validation(self):
        class Response:
            def __enter__(self):return self
            def __exit__(self,*args):pass
            def geturl(self):return 'https://api.frankfurter.dev/v2/rates'
            def read(self,limit):return b'[{"base":"EUR","quote":"CAD","date":"2024-01-02","rate":1.5}]'
        with mock.patch.object(fx.urllib.request,'urlopen',return_value=Response()) as request:
            _,rows=fx.decode(fx.fetch(['CAD'],'2024-01-02'));self.assertEqual(rows[1][1],'2024-01-02');self.assertIn('date=2024-01-02',request.call_args.args[0].full_url)
            with self.assertRaises(ValueError):fx.fetch(['CAD'],'2023-01-01')
            with self.assertRaises(ValueError):fx.fetch(['CAD'],'2024-02-30')
    def test_network_validation(self):
        class Response:
            def __init__(self,body,url='https://api.frankfurter.dev/v2/rates'):self.body=body;self.url=url
            def __enter__(self):return self
            def __exit__(self,*args):pass
            def geturl(self):return self.url
            def read(self,limit):return self.body[:limit]
        body=b'[{"base":"EUR","quote":"CAD","date":"2026-10-30","rate":1.5}]'
        with mock.patch.object(fx.urllib.request,'urlopen',return_value=Response(body)):
            _,rows=fx.decode(fx.fetch(['CAD']));self.assertEqual(rows[1][2],'1.5')
        for bad in [b'[]',body.replace(b'CAD',b'USD'),body.replace(b'EUR',b'CAD'),body.replace(b'1.5',b'NaN'),body.replace(b'1.5',b'true'),body+body,b'x'*131073]:
            with mock.patch.object(fx.urllib.request,'urlopen',return_value=Response(bad)):
                with self.assertRaises(ValueError):fx.fetch(['CAD'])
        with mock.patch.object(fx.urllib.request,'urlopen',return_value=Response(body,'http://example.com')):
            with self.assertRaises(ValueError):fx.fetch(['CAD'])

if __name__=='__main__':unittest.main()
