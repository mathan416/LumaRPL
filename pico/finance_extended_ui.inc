void finance_more()
{for(;;){
 const char *items[]={"Compound future value","Savings with regular deposits","Solve nominal annual rate","Solve payment count"};int action=screen_menu("math.finance.more","Growth / rate / term",items,4);if(action<0)break;
 auto &d=*finance_doc;double value=0;bool ok=false;pico_finance::outcome state=pico_finance::outcome::invalid;
 screen_page("math.finance.busy","Calculating finance",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();
 if(action==0)ok=pico_finance::growth(d.principal,d.annual,d.periods,d.frequency,value);
 if(action==1)ok=pico_finance::savings(d.principal,d.deposit,d.annual,d.periods,d.frequency,d.beginning,value);
 if(action==2){state=pico_finance::rate(d.principal,d.repayment,d.periods,d.frequency,d.beginning,value,finance_cancelled);ok=state==pico_finance::outcome::ok;}
 if(action==3){state=pico_finance::term(d.principal,d.repayment,d.annual,d.frequency,d.beginning,value);ok=state==pico_finance::outcome::ok;}
 if(!ok){notice("Finance result unavailable",state==pico_finance::outcome::cancelled?"Cancelled; inputs retained":state==pico_finance::outcome::indeterminate?"Inputs do not determine a unique rate":"Check inputs: payment may not repay the loan, or result lies outside supported range");continue;}
 char context[260];if(action==2)snprintf(context,sizeof context,"Nominal annual percentage, compounded %.8g times/year. Based on payment %.12g. Verified in [-99.999999,10000]%%; no fees.",d.frequency,d.repayment);else if(action==3)snprintf(context,sizeof context,"Payment periods: %.12g. Years: %.12g. Fractional count; next whole period for payoff is %.0f. No fees or intermediate cent rounding.",value,value/d.frequency,ceil(value));else snprintf(context,sizeof context,"%.8g%% nominal/year, %.8g periods/year, %.0f periods. Deposit %.12g at %s of period. No fees/taxes or intermediate cent rounding.",d.annual,d.frequency,d.periods,action==1?d.deposit:0,d.beginning?"beginning":"end");object_g result=decimal::from(value);screen_result("math.finance.result",result,items[action],context);
}
}
void finance_irr()
{
 screen_page("math.finance.busy","Searching IRR interval",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();key_pop_all();auto &d=*finance_doc;auto result=pico_finance::irr(d.flows,d.count,d.irr_low,d.irr_high,finance_cancelled);
 if(result.state!=pico_finance::outcome::ok){notice("IRR search",result.state==pico_finance::outcome::cancelled?"Cancelled; inputs retained":result.state==pico_finance::outcome::indeterminate?"All cash flows are zero; every valid rate fits":"No resolved roots in this interval. Repeated/touching roots can be missed; try other bounds.");return;}
 stack_depth_restore stack;for(unsigned i=0;i<result.count;++i){object_g value=decimal::from(result.roots[i]);if(!value||!rt.push(value)){engine_error();return;}}object_g roots=list::list_from_stack(result.count,object::ID_list);if(!roots){engine_error();return;}char context[260];snprintf(context,sizeof context,"Rates in %% PER CASH-FLOW PERIOD, searched from %.8g%% to %.8g%%. CF0 now. Scaled residual <=1e-8. Sign-change/sample search can miss repeated or close roots; not proof of all solutions.",d.irr_low,d.irr_high);screen_result("math.finance.result",roots,"IRR roots in interval",context);
}
