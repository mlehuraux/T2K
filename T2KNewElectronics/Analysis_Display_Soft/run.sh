#export file=R2019_05_13-16_05_17-000
#export file=R2019_05_13-16_10_24-000
#export file=R2019_05_15-08_06_29-000
export file=R2019_05_17-14_30_16-000
export compile=0
export startevent=1
export maxevent=1000

export input_aqs="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/aqs"
export input_txt="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/txt"
export output_root="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/root"

export make_convert="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/pandax/mclient/linux"
export bin_convert="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/bin/pandax/linux"
export here="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft"

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
./fconverter -i $input_aqs/ $file.aqs
#./fdecoder -i $input_aqs/$file.aqs -vflags 0x00003100

###################
# Build ROOT tree #
###################
if [ $compile = 1 ]
then
	cd $here/treeBuilder
	make clean
	make
else
	cd $here/bin
	./treeBuilder $file
	cd $here/outputs
	mkdir -p $file
fi

#################
# Pre-treatment #
#################
if [ $compile = 1 ]
then
	cd $here/preTreatment
	make clean
	make
else
	cd $here/bin
	./preTreatment $file.root
fi

###########
# Display #
###########
if [ $compile = 1 ]
then
	cd $here/display
	make clean
	make
else
	cd $here/bin
	for value in `seq $startevent 1 $maxevent`
	do
		echo $value
		./display $file.root $value
	done
fi

##################
# Post-Treatment #
##################
#cd $here/postTreatment
#make
#cd $here/bin
#./postTreatment $file.root
#cd $here

