#!/bin/bash
#
# D. Attié
#
# 16.04.2019
#

if [ $# -ne 0 ]
then
  echo ""
  echo -e "\tUsage:\n\t\t ./`basename $0`"
  echo ""
  echo -e "\tRun it with MapLayoutFile.txt."
  echo ""
  exit $E_BADARGS
fi

for i in 1728
do
  E=`cat $1 | grep $i $i` 
done
