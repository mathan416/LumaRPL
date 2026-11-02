#!/usr/bin/env python3
"""Confirm reviewed PNG pixels match the final production-controller captures."""
from pathlib import Path
import argparse,subprocess,shutil,json
root=Path(__file__).resolve().parents[1];p=argparse.ArgumentParser();p.add_argument('--release',default='release');args=p.parse_args();out=root/'build/ui-pixel-check';out.mkdir(exist_ok=True)
(out/'check.cc').write_text(r'''#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <cstdio>
int main(int argc,char **argv){if(argc!=3)return 2;QDir input(argv[1]),output(argv[2]);auto files=input.entryList({"*.ppm"},QDir::Files,QDir::Name);if(files.empty())return 3;for(const auto &f:files){QImage raw(input.filePath(f)),png(output.filePath(QFileInfo(f).completeBaseName()+".png"));if(raw.isNull()||png.isNull()||raw.convertToFormat(QImage::Format_RGB888)!=png.convertToFormat(QImage::Format_RGB888)){fprintf(stderr,"Capture mismatch: %s\n",qPrintable(f));return 4;}}printf("PASS: %lld PNG captures match final controller pixels exactly\n",static_cast<long long>(files.size()));}
''')
(out/'check.pro').write_text('QT += gui\nCONFIG += console c++17\nCONFIG -= app_bundle\nSOURCES += check.cc\nTARGET = ui-pixels-run\n')
qmake=shutil.which('qmake') or '/opt/homebrew/opt/qtbase/bin/qmake'
subprocess.run([qmake,'check.pro'],cwd=out,check=True,stdout=subprocess.DEVNULL)
subprocess.run(['make','-j2'],cwd=out,check=True,stdout=subprocess.DEVNULL)
r=subprocess.check_output([str(out/'ui-pixels-run'),str(root/'build/review-render/selected'),str(root/'reports'/(args.release+'-screenshots'))],text=True);print(r,end='')
(root/'reports'/(args.release+'-capture-pixels.json')).write_text(json.dumps(dict(release=args.release,all_generated_png_pixels_match_final_controller=True,scope=r.strip()),indent=2)+'\n')
