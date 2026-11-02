#!/usr/bin/env python3
"""Run a complete suite in isolated fixtures; quantize colour goldens for RGB332."""
import argparse,shutil,subprocess,json
from pathlib import Path
from simulator_checks import prepare_help, run_checked
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--indexed',action='store_true');p.add_argument('--baseline',action='store_true');p.add_argument('--tests',default='all');p.add_argument('--binary',type=Path);p.add_argument('--work',type=Path);args=p.parse_args()
kind='baseline' if args.baseline else 'rgb332' if args.indexed else 'colour';work=args.work or root/'build'/('port18-suite-'+kind)
if work.exists():shutil.rmtree(work)
work.mkdir()
for name in ('config','help','library','images','color-images'):
    shutil.copytree(root/'db48x'/name,work/name,ignore=shutil.ignore_patterns('bad','.DS_Store'))
for name in ('state','data','images/bad','color-images/bad'):(work/name).mkdir(parents=True,exist_ok=True)
prepare_help(root, work, colour=not args.baseline)
if args.indexed:
    converter=root/'build/golden-quantizer/quantize-goldens'
    if not converter.exists() or converter.stat().st_mtime<(root/'tools/quantize_goldens.cc').stat().st_mtime:
        converter.parent.mkdir(exist_ok=True)
        qmake=shutil.which('qmake') or '/opt/homebrew/opt/qtbase/bin/qmake'
        subprocess.run([qmake,str(root/'tools/quantize_goldens.pro'),'-o',str(converter.parent/'Makefile')],check=True)
        subprocess.run(['make','-C',str(converter.parent),'-j2'],check=True)
    count=int(subprocess.check_output([str(root/'build/golden-quantizer/quantize-goldens'),str(work/'color-images')],text=True).strip())
    (work/'GOLDENS.json').write_text(json.dumps({'source':'Unmodified upstream RGB565 PNG fixtures','conversion':'R/G 3 bits, B 2 bits; channel truncation and the production channel-expansion formulas','images':count},indent=2)+'\n')
code=run_checked([str(args.binary.resolve() if args.binary else root/'build/simulator-colour/baseline-release' if args.baseline else root/'build'/('simulator-'+kind)/'db48x.app/Contents/MacOS/db48x'),'-H','-m500','-T'+args.tests,'-w180000','-r1'],cwd=work,report=work/'ASSERTIONS.json')
raise SystemExit(code)
