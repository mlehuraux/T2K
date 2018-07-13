#!/bin/tcsh

 setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:lib/
if ( $ROOTSYS != /cern/root-6.04.04/ ) then
echo WARNING, wrong version of root is set
echo need root6
else
echo" root version (6) ok"
endif


