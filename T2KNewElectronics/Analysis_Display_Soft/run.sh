#export file=R2019_05_13-16_05_17-000
#export file=R2019_05_13-16_10_24-000
#export file=R2019_05_15-08_06_29-000
export file=$1
export compile=1
export startevent=1
export nevent=20

export t2k="/home/mlehuraux/GitLab/T2K/T2KNewElectronics"
#export data="/local/home/t2kt/projects/bin/pandax/data"
export input_aqs=$t2k/test_data/aqs
export input_txt=$t2k/test_data/txt
export output_root=$t2k/test_data/root

export make_convert=$t2k/pandax_client_16apr19/projects/pandax/mclient/linux
export bin_convert=$t2k/pandax_client_16apr19/projects/bin/pandax/linux
export here=$t2k/Analysis_Display_Soft

#cd $input_aqs
#ln -s $data/$file.aqs

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
#./fconverter -i $input_aqs/ $file.aqs
#./fdecoder -i $input_aqs/$file.aqs -vflags 0x00003100

###################
# Build ROOT tree #
###################
if [ $compile = 1 ]
then
	cd $here/treeBuilder
	make clean
	make
fi
cd $here/bin
./treeBuilder $file
cd $here/outputs
mkdir -p $file


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


###########
# Display #
###########
if [ $compile = 1 ]
then
	cd $here/display
	make clean
	make
fi
cd $here/bin
./display $file.root 1 1000

cd $here

##################
# Post-Treatment #
##################
#cd $here/postTreatment
#make
#cd $here/bin
#./postTreatment $file.root
#cd $here
