#BDW_VLOGSIM_ARGS = 

#BDW_EXTRA_CCFLAGS = -GCC_VERS=4.4
BDW_CCFLAGS_TAIL = -GCC_VERS=4.4

BDW_VLOGCOMP_ARGS = " -GCC_VERS 4.4"

MAKEFILE_PRJ = Makefile.prj
 
$(MAKEFILE_PRJ) : project.tcl
	@bdw_makegen project.tcl
 
-include $(MAKEFILE_PRJ)
 
.PHONY: CLEAN
CLEAN:
	@rm -f results.diff
	@rm -rf bdw_work cachelib arith_commands.log Makefile.prj
	@rm -f *.pro *.pro.user
	@rm -rf .stack*

COMPARE_FILE = output.log

cmp_result:
	@echo "*******************************"
	@echo " Simulation Results for the"
	@echo " $(BDW_SIM_CONFIG) simConfig"
	@if cmp -s ./golden/$(COMPARE_FILE) ./$(COMPARE_FILE) ; then \
        echo " SIMULATION PASSED" ; \
        bdw_sim_pass ; \
        else \
        echo " SIMULATION FAILED" ; \
        bdw_sim_fail ; \
        fi
	@echo "*******************************"

