#**************************************************************
# Copyright 2017 Cadence Design Systems, Inc
# All Rights Reserved.
#**************************************************************

# Libraries
set _LIB_SEARCH_PATH $::env(LIB_SEARCH_PATH)
set _LIB_NAME "tcb018gbwp7ttc.lib"
use_tech_lib "$_LIB_SEARCH_PATH/$_LIB_NAME"


# C++ compiler options
set_systemc_options -gcc 6.3

set CLOCK_PERIOD                    "5.0"
set_attr cc_options                 " -DCLOCK_PERIOD=$CLOCK_PERIOD "
set_attr hls_cc_options             " -DCLOCK_PERIOD=$CLOCK_PERIOD "

# stratus_hls Options
set_attr clock_period               $CLOCK_PERIOD
set_attr default_input_delay        0.0
set_attr dpopt_auto                 op
set_attr flatten_arrays             all
set_attr method_processing          synthesize
set_attr power                      on
set_attr prints                     off
set_attr sched_asap                 off
set_attr unroll_loops               on
set_attr wireload                   none
set_attr parts_effort               low
set_attr relax_timing               on
set_attr output_style_reset_all     on
set_attr output_style_structure_only off

# Simulation Options
use_systemc_simulator xcelium
use_verilog_simulator xcelium
enable_waveform_logging -shm 
#set_attr end_of_sim_command "make cmp_result"


# Synthesis Module Configurations
define_hls_module datapath ../src/datapath.cpp
define_hls_config datapath BASIC

define_hls_module controller ../src/controller.cpp
define_hls_config controller BASIC

define_hls_module memory_map ../src/memory_map.cpp
define_hls_config memory_map BASIC

define_hls_module top ../src/top.cpp -submodules {datapath controller memory_map}
define_hls_config top BASIC

# System Module Configurations
define_system_module sc_main "../sc_main/sc_top.cpp"


# Simulation Configurations
define_sim_config B    {top BEH}
define_sim_config V    {top RTL_V BASIC}


# Logic synthesis Configurations
# set_logic_synthesis_options         "BDW_LS_TECHLIB $TECH_LIB"
define_logic_synthesis_config       RC {top -all} -command bdw_runrc
