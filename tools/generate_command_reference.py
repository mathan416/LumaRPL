#!/usr/bin/env python3
"""Compatibility entry point for the new native LumaRPL reference generator."""
from pathlib import Path
import runpy
runpy.run_path(str(Path(__file__).with_name('generate_native_reference.py')),run_name='__main__')
