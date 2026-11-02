#include "plot_engine.h"

#include "plot_analysis.h"
// Headless view adapter exercises the production sampling engine, not LCD/UI.
extern "C" {
int test_plots=0,test_plot_mode=0,test_plot_count=0;
double test_plot_min=0,test_plot_max=0;
int test_analysis_mode=0,test_analysis_status=0;double test_analysis_value=0;
int pico_native_plot(pico_plot_request *request,pico_plot_sample sample,void *context)
{

    ++test_plots;test_plot_mode=request->surface;
    if(test_analysis_mode)
    {
        auto result=pico_plot_analysis::run(test_analysis_mode==2?pico_plot_analysis::operation::between:pico_plot_analysis::operation::integral,sample,context,0,1,0,1,0);
        test_analysis_status=int(result.state);test_analysis_value=result.value;return result.state==pico_plot_analysis::status::ok;
    }
    double values[900];int result;
    if(request->surface)
        result=pico_plot_engine::sample_surface(*request,sample,context,values,test_plot_min,test_plot_max,test_plot_count);
    else
    {
        result=pico_plot_engine::sample_curves(*request,sample,context,values);
        test_plot_count=0;
        for(double v:values)if(std::isfinite(v))++test_plot_count;
    }
    return result==1;
}
}
