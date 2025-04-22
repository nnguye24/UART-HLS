#!/bin/csh -f

# setenv STRATUS_HOME /xxx/STRATUS16.20-p100

xrun -sysc \
	./src/datapath.cpp \
	./sc_main/sc_datapath.cpp \
	-I./src -I./tb -I./sc_main \
	-I`cds_root stratus_ide`/share/stratus/include/ \
	-input wave.tcl -access rwc \
	-cdslib $IP_HOME/STD_CELL/tcb018gbwp7t_290a/TSMCHOME/digital/Back_End/cdk/tcb018gbwp7t_290a/cds.lib
