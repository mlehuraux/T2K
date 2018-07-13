#!/bin/tcsh
setenv LD_LIBRARY_PATH `pwd`/lib/:${LD_LIBRARY_PATH}:.
if !( -f Makefile )  then
 bash MTools/setupmtools.sh
endif
