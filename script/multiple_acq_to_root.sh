start=5148
for ((z=0; z<13; ++z));
do
  run=$(($start+$z))
  echo "$run"
  source script/acq_to_root_separate_fem.sh $run
done
