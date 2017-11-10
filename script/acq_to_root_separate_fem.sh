path_ini=$(pwd)
cd ~/suvorov/data_temp/

array=($(cat ~/suvorov/FileList/acq/run5206.list))

merge=true

cd ~/INSTALLFCC/DataWriter_devel/
mkdir temp
# loop over subruns
for ((i=0; i<${#array[@]}; i++));
do
  echo "$i"
  path=${array[$i]}
  f_acq=${path##*/}
  f_root=${f_acq%.*}".root"
  echo "Processing file: $path"
  cp $path ~/INSTALLFCC/DataWriter_devel/temp/$f_acq
  
  # loop over fems
  for ((j=0; j<1; j++));
  do
    f_root_new=${f_acq%.*}"_fem"$j".root"
    echo "**********************************************"
    echo "Creating $f_root_new"
    echo "**********************************************"
    
    ./bin/acqToRootConverterExe -t $j -m 1 -f  "temp/$f_acq" --noxml
    ls -l temp/
    mv temp/$f_root ~/suvorov/data_root/$f_root_new
  done

  rm ~/INSTALLFCC/DataWriter_devel/temp/$f_acq
done

rm -r temp

cd ~/suvorov/
if  $merge
then
  echo "Merging the subruns"
  # loop over fems
  for ((j=0; j<1; j++));
  do
    hadd "data_root/${f_acq%.*.*}_fem$j.root" "data_root/${f_acq%.*.*}."*"_fem$j.root"
  done
  rm "data_root/RUN"*"."*"_fem"*".root"
fi

cd $path_ini


