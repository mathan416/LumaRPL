import subprocess,shutil,tempfile,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class RenderingAudit(unittest.TestCase):
    def test_rejects_new_paths(self):
        with tempfile.TemporaryDirectory() as temporary:
            root=Path(temporary)
            shutil.copytree(ROOT/'pico',root/'pico')
            for name in ('user_interface.cc','stack.cc','graphics.cc','dmcp/sysmenu.cc'):
                destination=root/'db48x/src'/name;destination.parent.mkdir(parents=True,exist_ok=True)
                shutil.copy2(ROOT/'db48x/src'/name,destination)
            (root/'tools').mkdir();(root/'reports').mkdir()
            for name in ('audit_ui.py','test_surface_engine.py'):shutil.copy2(ROOT/'tools'/name,root/'tools'/name)
            def run():return subprocess.run(['python3',str(root/'tools/audit_ui.py')],capture_output=True,text=True)
            self.assertEqual(run().returncode,0)
            probe=root/'pico/unreviewed.cc'
            probe.write_text('void bad(){Screen.text(0,0,text,Settings.header_font(),colour);}\n')
            self.assertNotEqual(run().returncode,0)
            probe.write_text('void bad(){screen_page("unregistered.screen","Bad");}\n')
            self.assertNotEqual(run().returncode,0)
            probe.write_text('void bad(){page("Bad");}\n')
            # Native screen helpers must use IDs, even when they reuse a reviewed renderer.
            probe.rename(root/'pico/unreviewed_ui.inc')
            self.assertNotEqual(run().returncode,0)
        print('PASS: audit rejects unreviewed direct drawing, unknown screen IDs and unregistered controller pages')
