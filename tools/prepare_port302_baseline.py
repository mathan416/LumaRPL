#!/usr/bin/env python3
"""Recreate immutable port 30.1 performance fixtures (no algorithm edits)."""
from pathlib import Path
import tarfile,hashlib,json
root=Path(__file__).resolve().parents[1]
archive=root/'releases/port30.1/DB48X-PicoCalc-Pico2W-Colour-source.tar.gz'
expected={'release': 'port30.1', 'archive_sha256': '7c54e503e9e27cbb8e9c1a68d00264dbc9d63de39e91e1741e01a02e403d4124', 'headers': {'research_engine.h': '59e889c37661d7f924ebdda537dbc838f083733a2e31a08fc17518b26f9931a5', 'research_numeric.h': '7b7b253c61188c690a9610b7410d11520b312e842bd2977007c6e4b68ac202ab'}, 'adaptation': 'Evaluator class and numeric namespace renamed for side-by-side execution; algorithms unchanged.'}
if archive.exists():assert hashlib.sha256(archive.read_bytes()).hexdigest()==expected['archive_sha256']
out=root/'build/port302-baseline';out.mkdir(parents=True,exist_ok=True)
for name,digest in expected['headers'].items():
  raw=(root/'tools/baselines/port301'/name).read_bytes();assert hashlib.sha256(raw).hexdigest()==digest
  text=raw.decode()
  if name=='research_engine.h':text=text.split('struct pico_research_uncertainty_evaluator')[0].replace('pico_research_evaluator','pico_baseline_evaluator')
  else:text=text.replace('namespace pico_research {','namespace pico_baseline_numeric {')
  (out/name).write_text(text)
print('PASS: pinned baseline fixtures generated with names-only adaptation')
