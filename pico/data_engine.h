#pragma once
#include "data_table.h"
#include "workbench_engine.h"
#include "stats.h"
#include "array.h"
struct pico_data_summary {double values[2][8]{};bool sample[2]{};};
struct pico_data_fit {double intercept=0,slope=0,correlation=0;unsigned model=0;bool correlation_valid=false;};
struct pico_data_matrix_context {const pico_data_table *table;int column;};
inline object_p pico_data_matrix(size_t,size_t,size_t row,size_t column,void *opaque)
{
    auto &context=*static_cast<pico_data_matrix_context *>(opaque);
    return hwdouble::make(context.table->points[row][context.column<0?column:context.column]);
}
inline bool pico_data_prepare(const pico_data_table &table,int column)
{
    pico_data_matrix_context context{&table,column};array_g matrix=array::build(table.rows,column<0?2:1,pico_data_matrix,&context);
    if(!matrix){if(program::halted)rt.interrupted_error();return false;}
    if(!directory::store_here(StatsData::Access::name(),matrix))return false;
    integer_g one=integer::make(1),two=integer::make(2),zero=integer::make(0);
    object_g model=object::static_object(object::ID_LinearFit);list_g parameters=list::make(one,two,zero,zero,model);
    return parameters&&directory::store_here(StatsParameters::Access::name(),parameters);
}
inline bool pico_data_describe(const pico_data_table &table,pico_data_summary &summary)
{
    stack_depth_restore stack;pico_workbench_scope scope;if(!scope.entered||!table.valid()||!table.rows)return false;
    for(int c=0;c<2;++c)
    {
        if(program::interrupted()){rt.interrupted_error();return false;}
        if(!pico_data_prepare(table,c))return false;StatsAccess stats;
        typedef algebraic_p (StatsAccess::*method)()const;
        method methods[]={&StatsAccess::min,&StatsAccess::max,&StatsAccess::average,&StatsAccess::median,&StatsAccess::total,&StatsAccess::population_variance,&StatsAccess::variance,&StatsAccess::standard_deviation};
        for(int i=0;i<8;++i)
        {
            if(i>=6&&table.rows<2)continue;algebraic_g value=(stats.*methods[i])();
            if(!pico_workbench_double(value,summary.values[c][i]))return false;
        }
        summary.sample[c]=table.rows>=2;
    }
    return true;
}
inline bool pico_data_regress(const pico_data_table &table,pico_data_fit &fit)
{
    stack_depth_restore stack;pico_workbench_scope scope;if(!scope.entered||!table.valid()||table.rows<2)return false;
    if(!pico_data_prepare(table,-1))return false;StatsAccess stats;
    const object::id models[]={object::ID_LinearFit,object::ID_LogarithmicFit,object::ID_ExponentialFit,object::ID_PowerFit};
    stats.model=models[table.model];if(!stats.StatsParameters::Access::write()||!stats.linear_regression())return false;
    if(!pico_workbench_double(stats.intercept,fit.intercept)||!pico_workbench_double(stats.slope,fit.slope))return false;
    fit.model=table.model;algebraic_g correlation=stats.correlation();fit.correlation_valid=pico_workbench_double(correlation,fit.correlation);
    if(!fit.correlation_valid){if(program::halted||rt.error()&&!strcmp(cstring(rt.error()),"Out of memory"))return false;rt.clear_error();}
    return true;
}
inline bool pico_data_predict(const pico_data_fit &fit,double x,double &y)
{
    if((fit.model==1||fit.model==3)&&x<=0)return false;
    y=fit.model==0?fit.intercept+fit.slope*x:fit.model==1?fit.intercept+fit.slope*std::log(x):fit.model==2?fit.intercept*std::exp(fit.slope*x):fit.intercept*std::pow(x,fit.slope);
    return std::isfinite(y);
}
