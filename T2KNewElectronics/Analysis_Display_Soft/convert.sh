
export file=$1
export compile=1

export here="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft"
export t2k="/home/mlehuraux/GitLab/T2K/T2KNewElectronics"
export input_aqs=$t2k/test_data/aqslocal/

if [ $compile = 1 ]
then
	cd $here/converter
	make clean
	make
fi
cd $here/bin
./converter -i $input_aqs $file.aqs

#################
# Pre-treatment #
#################
if [ $compile = 1 ]
then
	cd $here/preTreatment
	make clean
	make
fi
cd $here/bin
./preTreatment $file.root

cd $here
