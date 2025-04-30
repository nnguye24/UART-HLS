#!/bin/csh -f

make hls_BASIC

bdw_export -dutmodels RTL_V -libmodels RTL_V -combined_file top.v -lsconfig RC_BASIC rtl
