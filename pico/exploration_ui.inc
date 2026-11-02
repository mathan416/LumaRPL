namespace {
pico_complex_document *complex_doc=nullptr;pico_document_cache<pico_complex_document> complex_cache;
pico_geometry_document *geometry_doc=nullptr;pico_document_cache<pico_geometry_document> geometry_cache;
void complex_edit(unsigned which)
{
 const char *forms[]={"Rectangular: real, imaginary","Polar: magnitude, phase"};int form=screen_menu("math.complex.form","Complex input form",forms,2);if(form<0)return;
 pico_complex_document next=*complex_doc;
 for(unsigned i=0;i<2;++i){char text[32];snprintf(text,sizeof text,"%s",next.polar[which]==form?next.input[which*2+i]:i?"0":"1");const char *title=form?(i?(next.radians?"Phase (radians)":"Phase (degrees)"):"Magnitude (nonnegative)"):(i?"Imaginary part":"Real part");if(!prompt(title,text,sizeof text))return;strcpy(next.input[which*2+i],text);}next.polar[which]=form;
 pico_complex_document validate=next;if(which){strcpy(validate.input[0],next.input[2]);strcpy(validate.input[1],next.input[3]);validate.polar[0]=next.polar[1];}object_g checked=pico_complex_calculate(validate,0);if(!checked){engine_error();return;}next.dirty=true;*complex_doc=next;
}
void complex_menu()
{
 if(rt.editing()){notice("Complex workbench","Finish or cancel calculator entry");return;}pico_document_access<pico_complex_document> access(complex_cache,complex_doc);if(!complex_doc){notice("Complex workbench","Not enough bounded heap");return;}save<bool> display(user_display_enable,false);
 for(;;){const char *items[]={"Edit Z","Edit W","Z conversions / roots","Z,W arithmetic",complex_doc->radians?"Phase unit: radians":"Phase unit: degrees","Files","Help"};int a=screen_menu("math.complex.menu","Complex workbench",items,7);if(a<0)return;
 if(a<2)complex_edit(a);if(a==4&&!pico_complex_set_unit(*complex_doc,!complex_doc->radians))engine_error();
 if(a==2||a==3){for(;;){const char *ops[]={"Rectangular Z","Polar Z","Magnitude |Z|","Phase of Z","Conjugate Z","n-th roots of Z"};const char *binary[]={"Z + W","Z - W","Z * W","Z / W"};int op=screen_menu(a==2?"math.complex.operations":"math.complex.arithmetic",a==2?"Z conversions / roots":"Complex arithmetic",a==2?ops:binary,a==2?6:4);if(op<0)break;unsigned degree=2;if(a==2&&op==5){double n=2;if(!analysis_number("Root degree (2-16)",n))continue;if(n<2||n>16||n!=floor(n)){notice("Root degree","Use a whole number from 2 to 16");continue;}degree=unsigned(n);}object_g value=pico_complex_calculate(*complex_doc,a==2?op:op+6,degree);if(!value){engine_error();continue;}settings::SaveAngleMode angle(complex_doc->radians?object::ID_Rad:object::ID_Deg);screen_result("math.complex.result",value,a==2?ops[op]:binary[op],op==5&&a==2?"All n roots, evaluated numerically at 16 digits, including multiplicities. Copy explicitly.":complex_doc->radians?"Phase is in radians. Inputs and stack are preserved until Copy.":"Phase is in degrees. Inputs and stack are preserved until Copy.");}
 }if(a==5){const char *files[]={"Save complex workbench","Load complex workbench"};int n=screen_menu("math.complex.files","Complex files",files,2);if(n==0)math_file_save(*complex_doc,"complex",".complex");if(n==1)math_file_load(*complex_doc,"complex",".complex");}
 if(a==6){const char *help=pico_ui_help::tool_text("math.complex.help");screen_text("math.complex.help","Complex help",help,strlen(help));}}
}
void geometry_coordinates(pico_geometry_document &doc,unsigned which)
{
 double x=doc.points[which][0],y=doc.points[which][1];if(!analysis_quantity("Point X (units accepted)",x,doc.units)||!analysis_quantity("Point Y (units accepted)",y,doc.units))return;
 if(fabs(x)>1e50||fabs(y)>1e50){notice("Coordinate range","Use coordinates within +/-1e50");return;}doc.points[which][0]=x;doc.points[which][1]=y;doc.dirty=true;
}
void geometry_measurements(const pico_geometry_document &doc)
{
 double intersection[2],hits[2][2],radius=pico_geometry::distance(doc.points[4],doc.points[5]);auto relation=pico_geometry::intersection(doc.points[0],doc.points[1],doc.points[2],doc.points[3],intersection);int count=pico_geometry::line_circle(doc.points[0],doc.points[1],doc.points[4],radius,hits);
 char text[800];int n=snprintf(text,sizeof text,"Coordinate / distance units: %s\n\nDistance AB: %.12g\nDistance CD: %.12g\nCircle radius EF: %.12g\n",*doc.units?doc.units:"dimensionless",pico_geometry::distance(doc.points[0],doc.points[1]),pico_geometry::distance(doc.points[2],doc.points[3]),radius);
 if(relation==pico_geometry::relation::unique)n+=snprintf(text+n,sizeof text-n,"AB/CD intersection: (%.12g, %.12g)\n",intersection[0],intersection[1]);else n+=snprintf(text+n,sizeof text-n,"AB/CD: %s\n",relation==pico_geometry::relation::parallel?"numerically parallel":relation==pico_geometry::relation::coincident?"coincident lines (not a unique point)":"degenerate line");
 double ab=pico_geometry::distance(doc.points[0],doc.points[1]),cd=pico_geometry::distance(doc.points[2],doc.points[3]);if(ab&&cd){double dot=((doc.points[1][0]-doc.points[0][0])/ab)*((doc.points[3][0]-doc.points[2][0])/cd)+((doc.points[1][1]-doc.points[0][1])/ab)*((doc.points[3][1]-doc.points[2][1])/cd);n+=snprintf(text+n,sizeof text-n,"Line direction angle (deg): %.12g\n",acos(fmax(-1.,fmin(1.,dot)))*180/3.141592653589793);}
 n+=snprintf(text+n,sizeof text-n,"AB / circle intersections: %s\n",count<0?"degenerate line":count==0?"none":count==1?"one (tangent / point circle)":"two");for(int i=0;i<count;++i)n+=snprintf(text+n,sizeof text-n,"(%.12g, %.12g)\n",hits[i][0],hits[i][1]);snprintf(text+n,sizeof text-n,"\nNumerical geometry; relative tolerance 1e-12. Lines are infinite, not segments.");screen_text("math.geometry.measurements","Geometry measurements",text,strlen(text));
}
int geometry_sample(void *,int,double,double,double *){return 0;}
void geometry_menu()
{
 if(rt.editing()){notice("Geometry","Finish or cancel calculator entry");return;}pico_document_access<pico_geometry_document> access(geometry_cache,geometry_doc);if(!geometry_doc){notice("Geometry","Not enough bounded heap");return;}save<bool> display(user_display_enable,false);
 for(;;){const char *items[]={"Explore / move points","Point coordinates","Distances / intersections","Visible constructions","Reset demo","Files","Help","Coordinate units"};int a=screen_menu("math.geometry.menu","Interactive geometry",items,8);if(a<0)return;
 if(a==0){pico_data_table points;points.rows=6;for(unsigned i=0;i<6;++i)memcpy(points.points[i],geometry_doc->points[i],16);pico_plot_request request{};request.active[0]=1;request.colour[0]=0x35a8e6;snprintf(request.label[0],80,"Geometry AB, CD, E/F | units:%s",*geometry_doc->units?geometry_doc->units:"1");memcpy(request.window,geometry_doc->window,32);unsigned selected=geometry_doc->selected;plot_source source{geometry_sample,nullptr,nullptr,&points};source.geometry=geometry_doc;source.scatter_selection=&selected;plot_curves(request,source);if(memcmp(request.window,geometry_doc->window,32)){memcpy(geometry_doc->window,request.window,32);geometry_doc->dirty=true;}}
 if(a==1){const char *names[]={"Point A","Point B","Point C","Point D","Point E (circle center)","Point F (on circle)"};int p=screen_menu("math.geometry.points","Edit point",names,6);if(p>=0)geometry_coordinates(*geometry_doc,p);}
 if(a==2)geometry_measurements(*geometry_doc);
 if(a==3){const char *names[]={"Toggle infinite line AB","Toggle infinite line CD","Toggle circle centered E through F"};int layer=screen_menu("math.geometry.layers","Geometry layers",names,3);if(layer>=0){geometry_doc->flags^=1u<<layer;geometry_doc->dirty=true;}}
 if(a==4&&confirm("Replace unsaved geometry with demo?")){*geometry_doc=pico_geometry_document{};geometry_doc->dirty=true;}
 if(a==5){const char *names[]={"Save geometry","Load geometry"};int file=screen_menu("math.geometry.files","Geometry files",names,2);if(file==0)math_file_save(*geometry_doc,"geometry",".geometry");if(file==1)math_file_load(*geometry_doc,"geometry",".geometry");}
 if(a==7){char units[24];strcpy(units,geometry_doc->units);if(prompt("Coordinate units (empty = dimensionless)",units,sizeof units)&&strcmp(units,geometry_doc->units)){pico_workbench_scope scope;if(pico_quantity_affine(units)||(*units&&!pico_form_unit(units))){if(rt.error())engine_error();else notice("Geometry units","Use absolute physical units; affine temperatures are unsupported.");continue;}pico_geometry_document next=*geometry_doc;bool ok=true;if(*units&&*next.units){for(unsigned i=0;i<6&&ok;++i)for(unsigned j=0;j<2&&ok;++j){algebraic_g q=pico_quantity_number(next.points[i][j],next.units);ok=q&&pico_quantity_value(q,units,next.points[i][j],false);}for(unsigned i=0;i<4&&ok;++i){algebraic_g q=pico_quantity_number(next.window[i],next.units);ok=q&&pico_quantity_value(q,units,next.window[i],false);}}else ok=confirm(*units?"Assign these units to the existing coordinate magnitudes?":"Remove units and retain the numerical coordinate magnitudes?");if(!ok){if(rt.error())engine_error();continue;}strcpy(next.units,units);if(!next.valid()){notice("Coordinate range","The converted geometry exceeds the supported range; current geometry retained.");continue;}next.dirty=true;*geometry_doc=next;}}
 if(a==6){const char *help=pico_ui_help::tool_text("math.geometry.help");screen_text("math.geometry.help","Geometry help",help,strlen(help));}}
}
}
