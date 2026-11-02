import importlib.util
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('checks', ROOT/'tools/simulator_checks.py')
checks = importlib.util.module_from_spec(spec)
spec.loader.exec_module(checks)


class SimulatorChecks(unittest.TestCase):
    def test_assertion_cannot_pass_with_zero_exit(self):
        with tempfile.TemporaryDirectory() as folder:
            report = Path(folder)/'report.json'
            output = io.BytesIO()
            code = checks.run_checked([sys.executable, '-c',
                'import sys; print("assert_error: Assertion failed: sentinel", file=sys.stderr)'],
                folder, report, output)
            self.assertNotEqual(code, 0)
            self.assertEqual(json.loads(report.read_text())['assertion_count'], 1)
            self.assertIn(b'Assertion failed', output.getvalue())

    def test_exit_failure_and_clean_success(self):
        with tempfile.TemporaryDirectory() as folder:
            for expected in (0, 7):
                self.assertEqual(checks.run_checked([sys.executable, '-c',
                    f'raise SystemExit({expected})'], folder,
                    Path(folder)/'report.json', io.BytesIO()), expected)

    def test_variant_and_byte_offsets(self):
        with tempfile.TemporaryDirectory() as folder:
            work = Path(folder)
            (work/'help').mkdir()
            for colour, name in ((False, 'db48x.md'), (True, 'db50x.md')):
                checks.prepare_help(ROOT, work, colour)
                raw = (ROOT/'db48x/help'/name).read_bytes()
                self.assertEqual((work/'help/db48x.md').read_bytes(), raw)
                for line in (work/'help/db48x.idx').read_bytes().splitlines():
                    offset, label = line.split(b':', 1)
                    self.assertTrue(raw[int(offset):].startswith(label))


if __name__ == '__main__':
    unittest.main()
