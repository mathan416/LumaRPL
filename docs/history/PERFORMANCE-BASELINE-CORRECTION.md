# Correction to the device performance baseline

The original IMG_8530 benchmark photograph reads **1,183,319 microseconds** for 100 expression evaluations. It was incorrectly transcribed as 118,319 in earlier conversation and release documentation. This transcription error created the apparent eightfold slowdown. Earlier archived artifacts remain unchanged; this note supersedes their baseline interpretation. No eightfold device regression is established by these photos.

The controlled port 30.4 comparison from IMG_8535–IMG_8539 reports:

| Measurement | Old / normal | New / normal | Old / quiet | New / quiet |
|---|---:|---:|---:|---:|
| Elapsed, us | 1169417 | 1155400 | 1036571 | 1016001 |
| Keyboard polling, us | 510595 | 510550 | 476777 | 476654 |
| Keyboard bus subset, us | 509131 | 509085 | 475309 | 475210 |
| Keyboard bus calls | 105 | 105 | 98 | 98 |
| LCD transfer, us | 91328 | 97415 | 0 | 0 |
| Background polling, us | 65 | 68 | 51 | 52 |
| Battery polling, us | 0 | 0 | 0 | 0 |
| Other time, us | 567429 | 547367 | 559743 | 539295 |

All 600 numerical comparisons match bit for bit. The SDK-reported CPU clock is 150 MHz. The new evaluator is about 1.2% faster than the old evaluator in normal mode and about 2.0% faster in quiet mode, within the same firmware. New / normal is about 2.4% faster than the original photographic reading, though those separate runs do not control all conditions.

Keyboard polling consumes about 47% of New / quiet elapsed time; almost all of that is the I2C register-read transaction. The average transaction is approximately 4.85 ms. Source inspection confirms the controller bus runs at 10 kHz, with a 10 ms minimum polling interval. The archived port 30 source already used the same policy. It is an existing cost, not evidence of a newly introduced polling regression.

Busy animation adds about 12% to New / normal versus quiet elapsed time, including its transfer time and additional polling during the longer run. Other time is not pure arithmetic: it includes engine operations, allocation/GC, CPU drawing work and instrumentation. Neither these values nor stable heap snapshots establish the absence of every leak.

The evidence supports further optimization of keyboard communication and sampling, with responsive cancellation preserved, and profiling the remaining CPU work. It does not justify reducing precision, disabling cancellation, changing the controller bus speed without compatibility checks, or claiming the entire remaining time is mathematical computation.
