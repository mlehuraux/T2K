path_ini=$(pwd)
cd ~/suvorov/data_temp/
run=$1
array=($(cat ~/suvorov/FileList/acq/run$run.list))
SOFT="/local/home/ssuvorov/T2K/ILC_TPC/DataWriter_devel"
#SOFT="~/INSTALLFCC/DataWriter_devel/"

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

  # loop over fems
  for ((j=0; j<7; j++));
  do
    f_root_new=${f_acq%.*}"_fem"$j".root"
    echo "**********************************************"
    echo "Creating $f_root_new"
    echo "**********************************************"
    k=1
    if (($j==1))
      then
      k=2
    fi
    ./bin/acqToRootConverterExe -t $j -m $k -f  "temp/$f_acq" --noxml
    ls -l temp/
    mv temp/$f_root ~/$f_root_new
  done

  rm $SOFT/temp/$f_acq
done

rm -r temp

cd ~/suvorov/
if  $merge
then
  echo "Merging the subruns"

  # loop over fems
  for ((j=0; j<7; j++));
  do
    rm data_root/${f_acq%.*.*}_fem$j.root
    hadd "data_root/${f_acq%.*.*}_fem$j.root" "data_root/${f_acq%.*.*}."*"_fem$j.root"
  done
  rm "data_root/RUN"*"."*"_fem"*".root"
fi

cd $path_ini


