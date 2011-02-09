~/code/bin/ants_git/bin/Release/antsSegmentation -d 2 -a r16slice.nii -i PriorLabelImage[3,r16slice_prior2.nii.gz,0.5]  -p Plato[1] -x mask.nii.gz -c [5,-0.1] -m [0.2] -o test.nii.gz  # -l 1[0.1,1] -l 2[0.1,1] -l 3[0.1,1]
exit
# 0.  Here, we use r16 as a template and r64 as a subject T1 image. 
# 1. Segment the input images 
  ThresholdImage 2 r16slice.nii mask.nii.gz 0.001 Inf
  Atropos -d 2 -a r16slice.nii -o [r16slice_seg.nii.gz,r16slice_prob_%02d.nii.gz] \
      -c [10,0.001] -m [0.05,1] -i kmeans[3] -x mask.nii.gz
  ThresholdImage 2 r64slice.nii mask.nii.gz 0.001 Inf
  Atropos -d 2 -a r64slice.nii -o [r64slice_seg.nii.gz,r64slice_prob_%02d.nii.gz] \
       -c [10,0.001] -m [0.05,1] -i kmeans[3] -x mask.nii.gz
# 2. Compute the deformation
  sh ants.sh 2 r16slice_prob_02.nii.gz r64slice_prob_02.nii.gz SPM 100x100x100
# 3. Smooth the segmented image 
  SmoothImage 2 r64slice_prob_02.nii.gz  1. sr64slice_prob_02.nii.gz  
# 3. Warp the segmented image by the 
  WarpImageMultiTransform 2 sr64slice_prob_02.nii.gz  sr64slice_prob_02_norm.nii.gz \ 
     -R r16slice.nii SPMWarp.nii.gz SPMAffine.txt 
# 4. Compute the log Jacobian from the transformation.
  ANTSJacobian 2 SPMWarp.nii.gz SPM 1  # take the log
  gzip SPMlogjacobian.nii 
# 5. Multiply the log-jacobian by the gray matter probability. 
 ImageMath 2 SPMlogjacobianmask.nii.gz m SPMlogjacobian.nii.gz sr64slice_prob_02_norm.nii.gz
# 6.  Estimate cortical thickness --- use svn revision 684 or greater (not in official release yet). 
 KellySlater 2 r64slice_seg.nii.gz r64slice_prob_03.nii.gz  r64slice_prob_02.nii.gz \
    r64slice_thickness.nii.gz 0.02 50 8 0 0.5
# 7.  Warp to template space. 
  WarpImageMultiTransform 2 r64slice_thickness.nii.gz  r64slice_thickness_norm.nii.gz  
    -R r16slice.nii SPMWarp.nii.gz SPMAffine.txt 
# 8.  Repeat steps 1-7 for a population and run statistics.
