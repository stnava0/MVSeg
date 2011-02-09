# input image 
IMG=T1_ICBM_1mm_3perc_40_bias/t1_icbm_normal_1mm_pn3_rf40.mhd
PDG=PD_ICBM_1mm_3perc_0_bias/pd_icbm_normal_1mm_pn3_rf0.mhd
IT2=T2_ICBM_1mm_3perc_0_bias/t2_icbm_normal_1mm_pn3_rf0.mhd
if [ ! -s $IMG ] ; then 
 echo no image named $IMG exists --- exiting.   you should be able to download this image from the brainweb website and use the header defined below to make it readable by Atropos. 
 exit
fi
# number of classes
NC=3
# convergence criterion 
CONV=[3,0]
# MRF-beta parameter 
MRF=0.20
# socrates parameter --- estimates mixture model
SOC=1
# kmeans with N4
NAMING=bwebRF40example
# binary image used as a weight and mask image for N4 and segmentation mask for Atropos 
# you can get this by thresholding the ground_truth labeling 
WTIM=mask.nii.gz
BS="N4BiasFieldCorrection -d 3  -h 0 " ; ITS=20 ; ITS2=3
$BS  -i $IMG   -o  n4.nii.gz -s 2 -b [200] -w mask.nii.gz -c [${ITS}x${ITS}x${ITS}x${ITS}x${ITS2},0.0001] -x mask.nii.gz 
Atropos  -d 3 -a n4.nii.gz  -i KMeans[$NC] -o [test.nii.gz,test_prob%02d.nii.gz] -m [${MRF},1x1x1] -x mask.nii.gz -p Socrates[${SOC}]  -c $CONV # -k HistogramParzenWindows[1,32]
# evaluation numbers
if [ -s ground_truth.nii.gz ] ; then 
ImageMath 3 dice_out.txt DiceAndMinDistSum test.nii.gz ground_truth.nii.gz 
cat dice_out.txt 
fi
exit
# T2 results 3% RF0
 Label 1 DICE 0.931616  RO 0.871986 TP1 0.984176 TP2 0.884385
 Label 2 DICE 0.882997  RO 0.790505 TP1 0.79822 TP2 0.987921
 Label 3 DICE 0.901592  RO 0.820817 TP1 0.99405 TP2 0.82487
 AvgDice  : 0.905402
# T1_RF40_3%_N4 reference results 
 Label 1 DICE 0.941891  RO 0.890165 TP1 0.904786 TP2 0.98217
 Label 3 DICE 0.966776  RO 0.935688 TP1 0.962814 TP2 0.97077
 Label 2 DICE 0.953862  RO 0.911794 TP1 0.971043 TP2 0.937279
 AvgDice  : 0.954176
# T1_RF40_3%_N4 + T2_RF0_3% noise reference results 
Label 1 DICE 0.937508  RO 0.882367 TP1 0.889406 TP2 0.99111
 Label 3 DICE 0.96701  RO 0.936128 TP1 0.958398 TP2 0.975778
 Label 2 DICE 0.951639  RO 0.90774 TP1 0.977645 TP2 0.926981
 AvgDice  : 0.952052
# T1_RF40_3%_N4 + PD_RF0_3% noise reference results 
 Label 1 DICE 0.944732  RO 0.895254 TP1 0.912413 TP2 0.979425
 Label 3 DICE 0.970348  RO 0.942405 TP1 0.969169 TP2 0.971531
 Label 2 DICE 0.957976  RO 0.919341 TP1 0.97094 TP2 0.945353
 AvgDice  : 0.957685
# PD_RF0_3% noise reference results 
 Label 2 DICE 0.905959  RO 0.828085 TP1 0.892396 TP2 0.919941
 Label 3 DICE 0.942477  RO 0.891212 TP1 0.943874 TP2 0.941084
 Label 1 DICE 0.837847  RO 0.720944 TP1 0.87262 TP2 0.805739
 AvgDice  : 0.895428

# below this line is a header file that can be used for the brain web raw data:
ObjectType = Image
NDims = 3
BinaryData = True
BinaryDataByteOrderMSB = False
CompressedData = False
TransformMatrix = 1 0 0 0 1 0 0 0 1
Offset = 0 0 0
CenterOfRotation = 0 0 0
AnatomicalOrientation = RAI
ElementSpacing = 1 1 1
DimSize = 181 217 181
ElementType = MET_UCHAR
ElementDataFile = t1_icbm_normal_1mm_pn3_rf40.rawb
