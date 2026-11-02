// Five bounded mathematical workbenches, using the common native UI and plots.
namespace {
pico_math_lab *lab=nullptr;pico_document_cache<pico_math_lab> lab_cache;
void lab_number_result(double value,const char *title,const char *context="",const char *units=""){
 if(!std::isfinite(value)){notice(title,"No finite result for these inputs. Check the parameters and domain.");return;}object_g result=pico_quantity_number(value,units);if(!result){engine_error();return;}screen_result("math.lab.result",result,title,context);
}
void lab_result(object_p value,const char *title,const char *context=""){object_g result=value;if(!result){engine_error();return;}screen_result("math.lab.result",result,title,context);}
void lab_window(double *values,const char *title){double next[4];memcpy(next,values,32);const char *names[]={"First X","Last X","First Y","Last Y"};for(int i=0;i<4;++i)if(!analysis_number(names[i],next[i]))return;if(!pico_graph_scene::bounds(next)){notice(title,"Use finite, increasing bounds on both axes.");return;}memcpy(values,next,32);lab->dirty=true;}
void lab_help(unsigned mode){
 if(mode==0)screen_text("math.lab.vector_help","Vector help",pico_ui_help::tool_text("math.lab.vector_help"),strlen(pico_ui_help::tool_text("math.lab.vector_help")));
 if(mode==1)screen_text("math.lab.probability_help","Probability help",pico_ui_help::tool_text("math.lab.probability_help"),strlen(pico_ui_help::tool_text("math.lab.probability_help")));
 if(mode==2)screen_text("math.lab.region_help","Inequality help",pico_ui_help::tool_text("math.lab.region_help"),strlen(pico_ui_help::tool_text("math.lab.region_help")));
 if(mode==3)screen_text("math.lab.multivariable_help","Multivariable help",pico_ui_help::tool_text("math.lab.multivariable_help"),strlen(pico_ui_help::tool_text("math.lab.multivariable_help")));
 if(mode==4)screen_text("math.lab.algebra_help","Algebra help",pico_ui_help::tool_text("math.lab.algebra_help"),strlen(pico_ui_help::tool_text("math.lab.algebra_help")));
}
void lab_files(unsigned mode){for(;;){
 const char *modes[]={"Degrees","Radians","Grads","Pi radians"};char angle_label[48];snprintf(angle_label,sizeof angle_label,"Angle mode: %s",modes[lab->angle]);const char *items[]={"Save math workbenches","Load math workbenches",angle_label,"Help"};int a=screen_menu("math.lab.files","Math workbench files / help",items,4);if(a<0)return;
 if(a==0)math_file_save(*lab,"mathlabs",".mathlab");if(a==1)math_file_load(*lab,"mathlabs",".mathlab");
 if(a==2){const char *modes[]={"Degrees","Radians","Grads","Pi radians"};int n=screen_menu("math.lab.angle","Workbench angle mode",modes,4);if(n>=0&&unsigned(n)!=lab->angle){lab->angle=n;lab->dirty=true;}}if(a==3)lab_help(mode);Settings.AngleMode(pico_lab_angle(lab->angle));
}
}
struct lab_vector_path {double v[2][2];};
int lab_vector_sample(void *p,double t,double *x,double *y){auto &v=*static_cast<lab_vector_path *>(p);unsigned which=t<=2?0:1;double u=which?t-2:t;u=u<=1?u:2-u;*x=v.v[which][0]*u;*y=v.v[which][1]*u;return 1;}
int lab_dummy_sample(void *,int,double,double,double *){return 0;}
void lab_vector_plot(){
 int plane=0;if(lab->dimensions==3){const char *planes[]={"XY projection","XZ projection","YZ projection"};plane=screen_menu("math.lab.vector_plane","Vector projection",planes,3);if(plane<0)return;}
 lab_vector_path values{};double scale=1;
 {pico_workbench_scope scope;if(!scope.entered){engine_error();return;}unit_g plot_units;for(int v=0;v<2;++v)for(unsigned i=0;i<lab->dimensions;++i){algebraic_g q=pico_quantity_parse(lab->vectors[v][i]);if(!q){engine_error();return;}unit_g u=unit::get(q);if(u&&!plot_units)plot_units=pico_form_unit_one(u);}for(int v=0;v<2;++v){unit_g expected=plot_units;for(unsigned i=0;i<lab->dimensions;++i){algebraic_g q=pico_quantity_parse(lab->vectors[v][i]);if(!q){engine_error();return;}unit_g u=unit::get(q);if(u&&!expected)expected=pico_form_unit_one(u);}for(int c=0;c<2;++c){int axis=plane==0?c:plane==1?(c?2:0):(c?2:1);algebraic_g number=pico_quantity_parse(lab->vectors[v][axis]);if(expected){if(!unit::get(number)){algebraic_g dimension=expected->uexpr();number=unit::make(number,dimension);}if(!number||!expected->convert(number)){engine_error();return;}unit_g q=unit::get(number);if(q)number=q->value();}if(!number||!pico_lab_real(number,values.v[v][c])||fabs(values.v[v][c])>1e99){if(rt.error())engine_error();else notice("Vector plot","Coordinates must fit within +/-1e99 for plotting.");return;}scale=fmax(scale,fabs(values.v[v][c]));}}}
 pico_path_scene path;path.first=0;path.last=4;path.window[0]=path.window[2]=-scale*1.2;path.window[1]=path.window[3]=scale*1.2;
 auto request=plot_request();request.active[0]=1;request.active[1]=request.active[2]=0;memcpy(request.window,path.window,32);snprintf(request.label[0],80,"A then B from origin | %s",plane==0?"XY":plane==1?"XZ":"YZ");
 plot_source source{lab_dummy_sample,&values};source.path=&path;source.path_sample=lab_vector_sample;plot_curves(request,source);
}
void lab_vectors(){
 for(;;){const char *items[]={"Edit vector A","Edit vector B",lab->dimensions==3?"Dimensions: 3":"Dimensions: 2","Products / geometry","Arithmetic / scale","Plot vectors / projections","Files / help"};int a=screen_menu("math.lab.vectors","Vector workbench",items,7);if(a<0)return;
 if(a<2){pico_math_lab next=*lab;bool ok=true;for(unsigned i=0;i<lab->dimensions;++i){char title[48];snprintf(title,sizeof title,"%c component %c (real expression)",'A'+a,'X'+i);if(!prompt(title,next.vectors[a][i],32)){ok=false;break;}}if(ok){object_g check=pico_lab_vector(next,a);if(!check)engine_error();else{next.dirty=true;*lab=next;}}}
 if(a==2){const char *n[]={"2D vectors","3D vectors"};int d=screen_menu("math.lab.vector_dimension","Vector dimensions",n,2);if(d>=0){lab->dimensions=d+2;lab->dirty=true;}}
 if(a==3){for(;;){const char *ops[]={"Vector A","Vector B","Dot product A.B",lab->dimensions==2?"Signed cross (Z)":"Cross product A x B","Magnitude of A","Distance between endpoints","Project A onto B","Angle A to B (degrees)"};int n=screen_menu("math.lab.vector_operations","Vector calculations",ops,8);if(n<0)break;if(n>=0)lab_result(pico_lab_vector(*lab,n),ops[n],n==7?"Numerical angle in degrees.":n==6?"Projection of A onto B.":n==5?"Length of A - B.":"");}}
 if(a==4){for(;;){char scale[64];snprintf(scale,sizeof scale,"Scale k: %s",lab->scalar);const char *ops[]={"A + B","A - B","k * A","A / k","Unit vector along A",scale};int n=screen_menu("math.lab.vector_arithmetic","Vector arithmetic",ops,6);if(n<0)break;if(n==5){char next[32];strcpy(next,lab->scalar);if(prompt("Real scale k",next,sizeof next)){pico_workbench_scope scope;if(!scope.entered){engine_error();continue;}algebraic_g checked=pico_quantity_parse(next);if(!checked)engine_error();else{strcpy(lab->scalar,next);lab->dirty=true;}}}else if(n>=0)lab_result(pico_lab_vector(*lab,n+8),ops[n]);}}
 if(a==5)lab_vector_plot();if(a==6)lab_files(0);
 }
}
int lab_probability_sample(void *p,int,double x,double,double *z){auto &d=*static_cast<pico_probability::distribution *>(p);*z=d.mass(d.kind?floor(x+.5):x);return std::isfinite(*z)?1:0;}
void lab_probability_plot(){
 auto d=lab->probability;auto request=plot_request();request.active[0]=1;request.active[1]=request.active[2]=0;
 double mode=d.kind==0?d.first:d.kind==1?fmin(d.first,floor((d.first+1)*d.second)):floor(d.first);double peak=d.mass(mode);
 if(d.kind==0){request.window[0]=d.first-4*d.second;request.window[1]=d.first+4*d.second;}else{double mean=d.kind==1?d.first*d.second:d.first,sd=sqrt(d.kind==1?d.first*d.second*(1-d.second):d.first);request.window[0]=fmax(-.5,floor(mean-5*sd)-.5);request.window[1]=fmin(d.kind==1?d.first+.5:8192.,ceil(mean+5*sd)+.5);}
 request.window[2]=0;request.window[3]=peak*1.25;if(!pico_graph_scene::bounds(request.window)){notice("Probability plot","These parameters cannot be represented in a finite plot window. Try a less extreme scale.");return;}
 snprintf(request.label[0],80,"%s | shaded interval %.5g to %.5g",d.kind?"PMF (integer outcomes)":"Normal density",lab->lower,lab->upper);
 pico_region_fill fill;fill.relation[0]=-1;fill.density=true;fill.first=d.kind?ceil(lab->lower)-.5:lab->lower;fill.last=d.kind?floor(lab->upper)+.5:lab->upper;
 plot_source source{lab_probability_sample,&d};source.fill=&fill;plot_curves(request,source);
}
void lab_probability(){
 for(;;){char at[48],range[80],q[48];snprintf(at,sizeof at,"Point X: %.9g",lab->at);snprintf(range,sizeof range,"Interval: %.8g to %.8g",lab->lower,lab->upper);snprintf(q,sizeof q,"Percentile p: %.9g",lab->percentile);
 const char *items[]={lab->probability.kind==0?"Distribution: normal":lab->probability.kind==1?"Distribution: binomial":"Distribution: Poisson","Parameters",at,range,q,"Calculate probability / quantile","Plot with shaded interval","Files / help"};int a=screen_menu("math.lab.probability","Probability workbench",items,8);if(a<0)return;
 if(a==0){const char *kinds[]={"Normal (mean, standard deviation)","Binomial (n, probability p)","Poisson (mean lambda)","Normal measurement units"};int k=screen_menu("math.lab.distribution","Distribution",kinds,4);if(k==3){char units[32];strcpy(units,lab->normal_units);if(prompt("Normal units (empty = dimensionless)",units,sizeof units)){pico_workbench_scope scope;if(*units&&!pico_form_unit(units)){engine_error();continue;}if(pico_quantity_affine(units)){notice("Units unavailable","Use absolute kelvin for normal distributions, rather than affine temperatures.");continue;}strcpy(lab->normal_units,units);lab->dirty=true;}continue;}if(k>=0){lab->probability.kind=k;lab->probability.first=k==0?0:k==1?10:3;lab->probability.second=k==1?.5:1;lab->dirty=true;}}
 if(a==1){auto next=lab->probability;bool ok=next.kind==0?analysis_quantity("Normal mean (units accepted)",next.first,lab->normal_units):analysis_number(next.kind==1?"Trials n (0-2000, whole)":"Poisson mean (0 < lambda <= 1000)",next.first);if(ok&&next.kind<2)ok=next.kind==0?analysis_quantity("Standard deviation (>0)",next.second,lab->normal_units):analysis_number("Success probability (0 to 1)",next.second);if(ok){if(!next.valid())notice("Parameters","Use positive scale/mean, integer n up to 2000, and probabilities from 0 to 1.");else{lab->probability=next;lab->dirty=true;}}}
 if(a==2){double v=lab->at;if(analysis_quantity("Point X (units accepted for normal)",v,lab->probability.kind?"":lab->normal_units)){lab->at=v;lab->dirty=true;}}
 if(a==3){double lo=lab->lower,hi=lab->upper;if(analysis_quantity("Interval first X (inclusive)",lo,lab->probability.kind?"":lab->normal_units)&&analysis_quantity("Interval last X (inclusive)",hi,lab->probability.kind?"":lab->normal_units)){if(lo>hi)notice("Interval","First X must not exceed last X.");else{lab->lower=lo;lab->upper=hi;lab->dirty=true;}}}
 if(a==4){double p=lab->percentile;if(analysis_number("Percentile p (0 < p < 1)",p)){if(p<=0||p>=1)notice("Percentile","Use a probability strictly between 0 and 1.");else{lab->percentile=p;lab->dirty=true;}}}
 if(a==5){for(;;){const char *ops[]={lab->probability.kind?"P(X = point)":"Density at point","P(X <= point)","P(X > point)","P(first <= X <= last)","Inverse CDF at p","Data inference / fit distribution"};int n=screen_menu("math.lab.probability_operations","Probability calculations",ops,6);if(n<0)break;if(n==5){advanced_launch(3);continue;}if(n>=0){auto &d=lab->probability;screen_page("math.lab.busy","Calculating probability",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();bool cancelled=false;auto poll=[](void *p){if(sampling_poll(nullptr,0)){*static_cast<bool *>(p)=true;return true;}return false;};double v=n==0?d.mass(lab->at):n==1?d.cdf(lab->at,false,poll,&cancelled):n==2?d.cdf(lab->at,true,poll,&cancelled):n==3?d.interval(lab->lower,lab->upper,poll,&cancelled):d.quantile(lab->percentile,poll,&cancelled);if(cancelled){notice("Probability cancelled","Inputs are retained.");continue;}char result_units[48]="";if(!d.kind&&*lab->normal_units){if(n==4)snprintf(result_units,sizeof result_units,"%s",lab->normal_units);if(n==0)snprintf(result_units,sizeof result_units,"1/(%s)",lab->normal_units);}lab_number_result(v,ops[n],n==4?(d.kind?"Smallest integer k with P(X <= k) >= p.":"Normal quantile; numerical."):n==0&&!d.kind?"Density is not a point probability.":d.kind?"Numerical probability; only integer outcomes contribute.":"Numerical normal probability.",result_units);}}}
 if(a==6)lab_probability_plot();if(a==7)lab_files(1);
 }
}
void lab_regions(){
 for(;;){const char *items[]={"First boundary","Second boundary","Third boundary","Plot window","Plot intersection","Files / help"};int a=screen_menu("math.lab.regions","Inequalities: Y versus f(X)",items,6);if(a<0)return;
 if(a<3){pico_math_lab next=*lab;const char *relations[]={"Disabled","Y >= f(X)","Y > f(X)","Y <= f(X)","Y < f(X)"};int n=screen_menu("math.lab.relation","Boundary relation",relations,5);if(n<0)continue;next.relation[a]=n==0?0:n<3?n:-(n-2);if(n&&!formula_entry("Boundary f(X)",next.region[a]))continue;next.dirty=true;*lab=next;}
 if(a==3)lab_window(lab->window,"Plot window");
 if(a==4){if(!lab->relation[0]&&!lab->relation[1]&&!lab->relation[2]){notice("No boundaries","Enable at least one boundary before plotting.");continue;}
 auto request=plot_request();memcpy(request.window,lab->window,32);pico_region_fill fill;for(int c=0;c<3;++c){fill.relation[c]=lab->relation[c];request.active[c]=lab->relation[c]!=0;snprintf(request.label[c],80,"Y %s %.65s",lab->relation[c]>0?(lab->relation[c]==2?">":">="):(lab->relation[c]==-2?"<":"<="),lab->region[c]);}
 pico_surface_evaluator e0(lab->region[0],false),e1(lab->region[1],false),e2(lab->region[2],false);if(!e0.valid()||!e1.valid()||!e2.valid()){engine_error();continue;}pico_surface_evaluator *evaluators[]={&e0,&e1,&e2};plot_source source{db48x_plot_sample,evaluators};source.fill=&fill;plot_curves(request,source);if(memcmp(lab->window,request.window,32)){memcpy(lab->window,request.window,32);lab->dirty=true;}
 }
 if(a==5)lab_files(2);
 }
}
bool lab_integral_sample(void *p,double x,double y,double &f){return static_cast<pico_surface_evaluator *>(p)->sample(x,y,f)==pico_surface_evaluator::finite;}
bool lab_integral_poll(void *){return sampling_poll(nullptr,0);}
void lab_multivariable(){
 for(;;){const char *items[]={"Edit F(X,Y)","Edit G(X,Y) for Jacobian","Point (X,Y)","Calculate / differentiate","Integral / surface bounds","Plot F surface","Plot tangent plane at point","Files / help"};int a=screen_menu("math.lab.multivariable","Multivariable calculus",items,8);if(a<0)return;
 if(a<2){char next[160];strcpy(next,a?lab->other:lab->function);if(formula_entry(a?"G(X,Y)":"F(X,Y)",next,false,true)){strcpy(a?lab->other:lab->function,next);lab->dirty=true;}}
 if(a==2){double x=lab->x,y=lab->y;if(analysis_number("Point X",x)&&analysis_number("Point Y",y)){lab->x=x;lab->y=y;lab->dirty=true;}}
 if(a==4){const char *names[]={"Integration rectangle","Surface X/Y window"};int n=screen_menu("math.lab.bounds","Calculus bounds",names,2);if(n==0)lab_window(lab->bounds,"Integration rectangle");if(n==1)lab_window(lab->surface_window,"Surface window");}
 if(a==3){for(;;){const char *ops[]={"Partial derivative dF/dX","Partial derivative dF/dY","Gradient [dF/dX, dF/dY]","Jacobian of [F,G]","Hessian of F","Evaluate F at point","Double integral (numerical)"};int n=screen_menu("math.lab.multivariable_operations","Calculus operations",ops,7);if(n<0)break;if(n<0)continue;if(n<5){lab_result(pico_lab_calculus(*lab,n),ops[n],n==3?"Rows: F,G. Columns: X,Y. Symbolic results retain X and Y.":n==4?"Rows and columns: X,Y.":"Symbolic result. Approx uses any stored X and Y.");continue;}
 pico_surface_evaluator evaluator(lab->function);if(!evaluator.valid()){engine_error();continue;}
 if(n==5){double value;if(evaluator.sample(lab->x,lab->y,value)!=pico_surface_evaluator::finite){if(rt.error())engine_error();else notice("Point unavailable","The formula is undefined here or calculation was interrupted.");continue;}lab_number_result(value,"F at point");}
 if(n==6){screen_page("math.lab.busy","Integrating rectangle",{{nullptr,nullptr,nullptr,nullptr,"Cancel"}});flush_display();auto r=pico_double_integral::integrate(lab->bounds,lab_integral_sample,&evaluator,lab_integral_poll);if(r.status==0&&program::halted)r.status=-1;if(r.status!=1){if(rt.error()){engine_error();continue;}notice("Integral unavailable",r.status==-1?"Cancelled. Your formula and bounds are retained.":r.status==-2?"Refinement did not converge. Try a smaller rectangle or a smoother integrand.":"The formula is undefined on the sample grid or exceeds the numerical range.");continue;}char context[240];snprintf(context,sizeof context,"Numerical estimate for a smooth integrand. Refinement difference / 15: %.3g. This is not a rigorous error bound.",r.error);lab_number_result(r.value,"Double integral",context);}
 }}
 if(a==5||a==6){char formula[160];strcpy(formula,lab->function);if(a==6){double plane[3];if(!pico_lab_plane(*lab,plane)){if(rt.error())engine_error();else notice("Tangent unavailable","A finite value and consistent nearby slopes are required. Try another point.");continue;}int n=snprintf(formula,sizeof formula,"(%.12g)+(%.12g)*(X-(%.12g))+(%.12g)*(Y-(%.12g))",plane[0],plane[1],lab->x,plane[2],lab->y);if(n<0||n>=160){notice("Tangent unavailable","Plane exceeds the formula size limit.");continue;}}

 pico_surface_evaluator evaluator(formula);if(!evaluator.valid()){engine_error();continue;}pico_surface_evaluator *evaluators[]={&evaluator};auto request=plot_request();request.surface=1;memcpy(request.bounds3d,lab->surface_window,32);snprintf(request.label[0],80,"%s",a==6?"Tangent plane (numerical centred slopes)":lab->function);plot_surface(request,{db48x_plot_sample,evaluators});}
 if(a==7)lab_files(3);
 }
}
void lab_algebra(){
 for(;;){const char *items[]={"Expression","Substitution variable","Replacement expression","Transform expression","Files / help"};int a=screen_menu("math.lab.algebra","Symbolic algebra",items,5);if(a<0)return;
 if(a==0||a==2){char next[160];strcpy(next,a==0?lab->algebra:lab->replacement);if(formula_entry(a==0?"Symbolic expression":"Replacement",next,false,false,false,"Expression; named symbols are retained")){strcpy(a==0?lab->algebra:lab->replacement,next);lab->dirty=true;}}
 if(a==1){const char *names[]={"X","Y","Z","A","B","C"};int n=screen_menu("math.lab.variable","Substitution variable",names,6);if(n>=0){lab->variable[0]=names[n][0];lab->dirty=true;}}
 if(a==3){for(;;){const char *ops[]={"Simplify","Expand","Collect / factor supported forms","Partial fractions","Substitute variable"};int n=screen_menu("math.lab.algebra_operations","Algebra transformation",ops,5);if(n<0)break;if(n<0)continue;object_g result=pico_lab_algebra(*lab,n);if(!result){engine_error();continue;}
 if(screen_result("math.lab.result",result,ops[n],n==2?"DB48X Collect rules; this is not a general polynomial-factorization guarantee.":"",true,"Use result")){char next[160];if(!pico_formula_source(result,next,sizeof next)){notice("Result too large","Copy the result to the stack; it cannot fit the 159-byte formula editor.");continue;}if(confirm("Replace the current algebra expression with this result?")){strcpy(lab->algebra,next);lab->dirty=true;}}
 }}
 if(a==4)lab_files(4);
 }
}
void lab_launch(int mode){
 if(rt.editing()){notice("Math workbench","Finish or cancel calculator entry first.");return;}bool first=!lab_cache.resident&&!lab_cache.handle;pico_document_access<pico_math_lab> access(lab_cache,lab);if(!lab){notice("Math workbench","Not enough memory to open the workbench. Existing documents are retained.");return;}if(first)lab->angle=angle_number();
 save<bool> display(user_display_enable,false);settings::SaveAngleMode angle(pico_lab_angle(lab->angle));
 if(mode==0)lab_vectors();if(mode==1)lab_probability();if(mode==2)lab_regions();if(mode==3)lab_multivariable();if(mode==4)lab_algebra();
}
}
