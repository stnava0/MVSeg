IMG=" T1_ICBM_1mm_3perc_40_bias/t1_icbm_normal_1mm_pn3_rf40.mhd "
EX=Atropos
EX=~/code/bin/ants_git/bin/Release/antsSegmentation 
NC=3
CONV=[3,0]
MRF=0.20
SOC=1
# kmeans with N4
NAMING=bwebRF40example
WTIM=test_init_prob03.nii.gz
# $EX  -d 3 -a  $IMG -i KMeans[$NC] -o [test_init.nii.gz,test_init_prob%02d.nii.gz] -m [${MRF},1x1x1] -x mask.nii.gz -p Socrates[${SOC}]  -c $CONV 
BS=" N4BiasFieldCorrection -d 3  -h 0 " ; ITS=20 ; ITS2=3
$BS  -i $IMG   -o  n4.nii.gz -s 2 -b [200] -w mask.nii.gz -c [${ITS}x${ITS}x${ITS}x${ITS}x${ITS2},0.0001] -x mask.nii.gz 
exe="$EX  -d 3 -a  n4.nii.gz -i KMeans[$NC] -o [test.nii.gz,test_prob%02d.nii.gz] -m [${MRF},1x1x1] -x mask.nii.gz -p Socrates[${SOC}]  -c $CONV "
echo $exe > ${NAMING}.txt 
$exe #  >> ${NAMING}.txt 
ImageMath 3 dice_out.txt DiceAndMinDistSum test.nii.gz ground_truth.nii.gz 
cat dice_out.txt >> ${NAMING}.txt 
rm dice* 

