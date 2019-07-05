

export output_root="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/test_data/root"
export here="/home/mlehuraux/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft"

source run.sh R2019_06_15-19_34_23-000

cd $output_root
#hadd -f R2019_05_20-12_08_01-000_processed.root "R2019_05_20-12_06_04-000.root" "R2019_05_20-12_08_01-000.root"


cd $here
