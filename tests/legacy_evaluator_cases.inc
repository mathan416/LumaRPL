#include "../build/port302-baseline/research_engine.h"
static void legacy_evaluator_cases(){
 rt.reset();if(!execute("42 'X' STO 43 'Y' STO 7 'A' STO 123"))test_abort(__LINE__);
 auto angle=Settings.AngleMode();auto precision=Settings.Precision();auto dirs=rt.directories();
 unsigned compared=0;
 for(const char *formula:{"SIN(X)*COS(Y)+X^2","X^2+Y^2-25","(X-1)^2+(Y+2)^2","A*X+Y","LN(X)+Y","1/X+Y"}){
  double expected[100]{};bool valid[100]{};
  {pico_baseline_evaluator baseline(formula);if(!baseline.valid())test_abort(__LINE__);for(unsigned i=0;i<100;++i)valid[i]=baseline.field(&baseline,(int(i)-50)*.02,.5,expected[i]);}
  {pico_research_evaluator current(formula);if(!current.valid())test_abort(__LINE__);for(unsigned i=0;i<100;++i){double value=0;bool ok=current.field(&current,(int(i)-50)*.02,.5,value);if(ok!=valid[i]||(ok&&memcmp(&value,&expected[i],sizeof value)))test_abort(__LINE__);++compared;}}
 }
 {pico_research_evaluator current("X+Y");program::halted=true;double value;if(current.field(&current,1,2,value)||!current.cancelled)test_abort(__LINE__);}
 {pico_research_evaluator current("X+Y");if(!current.valid())test_abort(__LINE__);char block[1025];memset(block,'a',1024);block[1024]=0;unsigned pushed=0;while(object_p item=text::make(block)){if(!rt.push(item))break;++pushed;}rt.clear_error();while(object_p item=hwdouble::make(9.123)){if(!rt.push(item))break;++pushed;}rt.clear_error();double value;if(current.field(&current,3,4,value)||!current.failed)test_abort(__LINE__);rt.clear_error();while(pushed--)rt.drop();}
 double stored;if(compared!=600||rt.depth()!=1||rt.top()->as_int32(0,false)!=123||rt.directories()!=dirs||Settings.AngleMode()!=angle||Settings.Precision()!=precision||program::halted||!pico_formula_scalar("X",stored)||stored!=42||!pico_formula_scalar("Y",stored)||stored!=43||!pico_formula_scalar("A",stored)||stored!=7)test_abort(__LINE__);
 rt.reset();puts("PASS: restored old evaluator: 600 archived-baseline result/status comparisons, domain failures, cancellation, OOM, stack/settings/variables/directory preservation");
}
