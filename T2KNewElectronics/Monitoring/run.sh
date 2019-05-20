export file=R2019_05_17-14_27_42-000
export compile=0
export startevent=1
export maxevent=1000

export input_aqs="/local/home/t2kt/Documents/DisplayT2KMarion/T2K-T2K/T2KNewElectronics/test_data/aqs"
export input_txt="/local/home/t2kt/Documents/DisplayT2KMarion/T2K-T2K/T2KNewElectronics/test_data/txt"
export output_root="/local/home/t2kt/Documents/DisplayT2KMarion/T2K-T2K/T2KNewElectronics/test_data/root"

export here="/local/home/t2kt/Documents/DisplayT2KMarion/T2K-T2K/T2KNewElectronics/Monitoring/"

###############################
#   Pedestals visualisation   #
###############################
# if not done already create the executable
if [ $compile = 1 ]
then
	cd $here/pedestals
	make clean
	make
fi
cd $here/bin
./pedestals -i $input_aqs/ $file.aqs

cd $here
