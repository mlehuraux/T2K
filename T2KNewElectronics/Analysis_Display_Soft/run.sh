export input_aqs="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/aqs" 
export input_txt="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/txt"
export output_root="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/root"

export make_convert="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/pandax/mclient/linux"
export bin_convert="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/bin/pandax/linux"
export here="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft"

###############################
# Convert .aqs file into .txt #
###############################
# if not done already create the executable 
#cd $make_convert 
#make 
#cd $bin_convert
#./fconverter -i $input_aqs/ R2019_05_02-11_48_23-000.aqs
#./fdecoder -i $input_aqs/R2019_05_02-11_48_23-000.aqs -vflags 0x00003100

###################
# Build ROOT tree #
###################
#cd $here/treeBuilder
#make
#cd $here/bin
#./treeBuilder R2019_05_02-11_48_23-000
#cd $here/outputs
#mkdir -p R2019_05_02-11_48_23-000

#################
# Pre-treatment #
#################
#cd $here/preTreatment
#make
#cd $here/bin
#./preTreatment R2019_05_02-11_48_23-000.root

###########
# Display #
###########
#cd $here/display
#make
#cd $here/bin
#for value in `seq 0 1 1357`
#do
#echo $value
#./display R2019_05_02-11_48_23-000.root $value
#done

##################
# Post-Treatment #
##################
#cd $here/postTreatment
#make
cd $here/bin
./postTreatment R2019_05_02-11_48_23-000.root
cd $here

