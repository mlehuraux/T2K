export output_root="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/test_data/root"
export here="/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/Analysis_Display_Soft"

source run.sh R2019_05_20-12_08_01-000
source run.sh R2019_05_21-09_40_00-000
source run.sh R2019_05_22-14_42_16-000
source run.sh R2019_05_22-21_08_42-000
source run.sh R2019_05_23-07_30_10-000
source run.sh R2019_05_23-14_40_29-000
source run.sh R2019_05_24-08_53_45-000

cd $output_root
hadd -f R2019_05_20-12_08_01-000_processed.root "R2019_05_20-12_06_04-000.root" "R2019_05_20-12_08_01-000.root"
hadd -f R2019_05_21-09_40_00-000_processed.root "R2019_05_21-09_40_00-000.root" "R2019_05_21-09_37_36-000.root"
hadd -f R2019_05_22-14_42_16-000_processed.root "R2019_05_22-14_41_34-000.root" "R2019_05_22-14_42_16-000.root"
hadd -f R2019_05_22-21_08_42-000_processed.root "R2019_05_23-07_15_13-000.root" "R2019_05_22-21_08_42-000.root"
hadd -f R2019_05_23-07_30_10-000_processed.root "R2019_05_23-07_19_48-000.root" "R2019_05_23-07_30_10-000.root"
hadd -f R2019_05_23-14_40_29-000_processed.root "R2019_05_23-14_38_28-000.root" "R2019_05_23-14_40_29-000.root"
hadd -f R2019_05_24-08_53_45-000_processed.root "R2019_05_24-08_51_39-000.root" "R2019_05_24-08_53_45-000.root"

cd $here
