export input_aqs="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/aqs"
export input_txt="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/txt"
export output_root="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/root"

export make_convert="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/pandax/mclient/linux"
export bin_convert="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/pandax_client_16apr19/projects/bin/pandax/linux"
export here="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft"

export file=R2019_05_15-15_55_34-000

###############################
# Convert .aqs file into .txt #
###############################
# if not done already create the executable
#cd $make_convert
#make
cd $bin_convert
./fconverter -i $input_aqs/ $file.aqs


###################
# Build ROOT tree #
###################
#cd $here/treeBuilder
#make
cd $here/bin
./treeBuilder $file
cd $here/outputs
mkdir -p $file

#################
# Pre-treatment #
#################
#cd $here/preTreatment
#make
cd $here/bin
./preTreatment $file.root

###########
# Display #
###########
#cd $here/display
#make
cd $here/bin
start=0
nevent=1000
./display $file.root $start $nevent

##################
# Post-Treatment #
##################
#cd $here/postTreatment
#make
#cd $here/bin
#./postTreatment R2019_05_02-11_48_23-000.root
#cd $here
