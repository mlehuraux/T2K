path_ini=$(pwd)
run=$1
SOFT="/local/home/ssuvorov/T2K/ILC_TPC/DataWriter_devel"
array=($(cat $SOFT/../tmp/run$run.list))

merge=true

cd $SOFT
mkdir temp
# loop over subruns
for ((i=0; i<${#array[@]}; i++));
do
  path=${array[$i]}
  f_acq=${path##*/}
  f_root=${f_acq%.*}".root"
  echo "Processing file: $path"
  cp $path $SOFT/temp/$f_acq
  source setup.sh

  ./bin/acqToRootConverterExe -t 0 -m 1 -f  "temp/$f_acq" --noxml
  ls -l temp/
  mv temp/$f_root $SOFT/../../data/T2K_TPC/data_root/$f_root

  rm $SOFT/temp/$f_acq
done

rm -r temp

if  $merge
then
  cd $SOFT/../../data/T2K_TPC/data_root
  echo "Merging the subruns"
  hadd -f "${f_acq%.*.*}.root" "${f_acq%.*.*}."*".root"
  rm "${f_acq%.*.*}."*".root"
fi

cd $path_ini


