export file=R2019_05_17-14_27_42-000
export compile=1
export startevent=1
export maxevent=1000

export make_convert="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/pandax/mclient/linux"
export bin_convert="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/bin/pandax/linux"

export input_aqs="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/aqs"
export input_txt="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/txt"
export output_root="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/root"

export here="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/Monitoring/"


###############################
# Convert .aqs file into .txt #
###############################
# if not done already create the executable
if [ $compile = 1 ]
then
	cd $make_convert
	make clean
	make
fi
cd $bin_convert
./fdecoder -i $file.aqs 

###############################
#   Pedestals visualisation   #
###############################
# if not done already create the executable
#if [ $compile = 1 ]
#then
#	cd $here/pedestals
#  make clean
#	make
#fi
#cd $here/bin
#./pedestals -i $input_aqs/ $file.aqs

cd $here
