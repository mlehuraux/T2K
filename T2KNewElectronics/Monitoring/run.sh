export file=R2019_05_27-14_26_27-000
export filedata=R2019_05_23-07_30_10-000
export compile=1
export startevent=1
export maxevent=1000

export data="/local/home/t2kt/projects/bin/pandax/data"

export make_convert="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/pandax/mclient/linux"
export bin_convert="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/bin/pandax/linux"

export input_aqs="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/aqs"
export input_txt="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/txt"
export output_root="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/root"

export here="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/Monitoring/"

cd $input_aqs
#ln -s $data/$filedata.aqs

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
#./pedestals -i $input_aqs/ $file.aqs 0 1134 380 250 5

if [ $compile = 1 ]
then
	cd $here/monitor
  make clean
	make
fi
cd $here/bin
./T2KMonitor -i $input_aqs/ $filedata.aqs
cd $here
