path_ini=$(pwd)
src_path=$1
src_file=${src_path##*/}
file_exe=${src_file%.*}".EXE"
echo "*********************************************************************"
echo "*********************************************************************"
echo "*********   BUILDING $src_path TO $file_exe **********"
echo "*********************************************************************"
echo "*********************************************************************"
local=true
if $local
then 
  compile_path=~/T2K/ILC_TPC/ilc_macros/
  out=~/T2K/ILC_TPC/script/
else
  compile_path=~/INSTALLFCC/macros/
  out=~/suvorov/script/
fi

cp $src_path $compile_path$src_file
cd $compile_path
make SRC="mydict.C $src_file"  EXE=$file_exe
rm $src_file
mv $file_exe $out
cd $path_ini