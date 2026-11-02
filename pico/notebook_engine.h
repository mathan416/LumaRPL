#pragma once
#include "notebook.h"
#include "workbench_engine.h"
inline void pico_notebook_preview(char *out,size_t capacity,const char *source,size_t length)
{
    if(!capacity)return;size_t n=std::min(length,capacity-1);bool clipped=n<length;
    if(clipped&&capacity>4)n=std::min(n,capacity-4);
    while(n&&!pico_text_editor::valid_utf8(source,n))--n;
    memcpy(out,source,n);out[n]=0;if(clipped&&capacity>4)strcat(out,"...");
}
// Run through a selected cell starting at cell one. Cached previews are never
// inputs: each Rn is rebound from this explicit run and dies with the scope.
inline object_p pico_notebook_run(pico_notebook &book,unsigned through,int &failed,bool &cancelled,
                                  bool (*poll)(void *)=nullptr,void *context=nullptr)
{
    failed=-1;cancelled=false;book.clear_results();
    if(!book.valid()||through>=book.count)return nullptr;
    auto precision=Settings.Precision();bool hardware=Settings.HardwareFloatingPoint();
    bool numerical=Settings.NumericalResults(),constants=Settings.NumericalConstants(),angle_units=Settings.SetAngleUnits(),symbolic_solver=Settings.SymbolicSolver();
    auto iterations=Settings.SolverIterations(),imprecision=Settings.SolverImprecision(),shuffles=Settings.SolverShuffles();
    stack_depth_restore stack;object_g last;
    {
        pico_workbench_scope scope;if(!scope.entered)return nullptr;
        settings::SavePrecision restore_precision(precision);settings::SaveHardwareFloatingPoint restore_hardware(hardware);
        settings::SaveNumericalResults restore_numerical(numerical);settings::SaveNumericalConstants restore_constants(constants);
        settings::SaveSetAngleUnits restore_angle_units(angle_units);settings::SaveSymbolicSolver restore_symbolic_solver(symbolic_solver);
        settings::SaveSolverIterations restore_iterations(iterations);settings::SaveSolverImprecision restore_imprecision(imprecision);settings::SaveSolverShuffles restore_shuffles(shuffles);
        for(unsigned i=0;i<=through;++i)
        {
            auto &cell=book.cells[i];failed=int(i);
            bool stop=(poll&&poll(context))||program::interrupted();
            if(stop){cancelled=true;cell.status=pico_notebook::state::cancelled;strcpy(cell.preview,"Interrupted");last=nullptr;break;}
            if(!pico_notebook::prior_results_only(cell.expression,i,Settings.IgnoreSymbolCase()))
            {cell.status=pico_notebook::state::error;strcpy(cell.preview,"Rn must refer to an earlier cell");last=nullptr;break;}
            expression_g equation=pico_formula_parse(cell.expression);
            size_t depth=rt.depth();
            if(!equation||equation->run()!=object::OK||rt.depth()!=depth+1)
            {
                cancelled=program::halted;cell.status=cancelled?pico_notebook::state::cancelled:pico_notebook::state::error;
                const char *error=rt.error()?cstring(rt.error()):cancelled?"Interrupted":"Expression must return one result";
                pico_notebook_preview(cell.preview,sizeof cell.preview,error,strlen(error));last=nullptr;break;
            }
            object_g value=rt.pop();last=rt.clone(value);
            if(!last){cell.status=pico_notebook::state::error;strcpy(cell.preview,"Out of memory");break;}
            char name[8];snprintf(name,sizeof name,"R%u",i+1);symbol_g symbol=symbol::make(name);
            if(!symbol||!directory::store_here(symbol,value))
            {cell.status=pico_notebook::state::error;strcpy(cell.preview,"Could not bind cell result");last=nullptr;break;}
            text_g text=last->as_text(true);
            if(!text){cell.status=pico_notebook::state::error;strcpy(cell.preview,"Could not render result");last=nullptr;break;}
            size_t length;utf8 bytes=text->value(&length);pico_notebook_preview(cell.preview,sizeof cell.preview,reinterpret_cast<const char *>(bytes),length);
            cell.status=pico_notebook::state::complete;failed=-1;
        }
        // Drop failed-expression operands before the child directory is purged;
        // otherwise cleanup may need to clone global operands during an OOM.
        stack.clear();
    }
    return last;
}
