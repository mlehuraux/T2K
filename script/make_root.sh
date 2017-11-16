src_path=$1
src_file=${src_path##*/}
file_exe=${src_file%.*}".EXE"
echo "*********************************************************************"
echo "*********************************************************************"
echo "*********   BUILDING $src_path TO $file_exe **********"
echo "*********************************************************************"
echo "*********************************************************************"

cp $src_path ~/INSTALLFCC/macros
cd ~/INSTALLFCC/macros
make SRC="mydict.C $src_file"  EXE=$file_exe
rm $src_file
mv $file_exe ~/suvorov/script
cd ~/suvorov/