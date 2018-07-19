start=135
end=139
project="/local/home/ssuvorov/T2K/ILC_TPC/"
mkdir $project/tmp
for ((z=start; z<=end; ++z));
do
  run=$(((z)))
  if [ $run -ne 133 ] && [ $run -ne 134 ] && [ $run -ne 149 ] && [ $run -ne 150 ] && [ $run -ne 148 ] && [ $run -ne 140 ]
  then
    echo "Working on run $run"
    #ls /media/ILC-7M_DATA_04/data/RUN_0$run*.acq > FileList/acq/run$run.list
    ls $project/../data/T2K_TPC/data_acq/RUN_00$run*.acq > $project/tmp/run$run.list
    source $project/script/acq_to_root_separate_fem.sh $run
  fi
done
rm -r $project/tmp
