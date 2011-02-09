/*=========================================================================
   
  Program:   Advanced Normalization Tools 
  Module:    $RCSfile: ComputeSimilarityMetric.cxx,v $ 
  Language:  C++       
  Date:      $Date: 2008/09/10 19:53:13 $ 
  Version:   $Revision: 1.1 $ 
 
  Copyright (c) ConsortiumOfANTS. All rights reserved. 
  See accompanying COPYING.txt or 
 http://sourceforge.net/projects/advants/files/ANTS/ANTSCopyright.txt for 
 
     This software is distributed WITHOUT ANY WARRANTY; without even  
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  
     PURPOSE.  See the above copyright notices for more information. 
   
=========================================================================*/ 
#include "ReadWriteImage.h" 
#include "itkDiscreteGaussianImageFilter.h"  
#include "itkAvantsMutualInformationRegistrationFunction.h" 
#include "itkProbabilisticRegistrationFunction.h" 
#include "itkCrossCorrelationRegistrationFunction.h" 
// #include "itkLandmarkCrossCorrelationRegistrationFunction.h"
 

template <unsigned int ImageDimension>
int ComputeSimilarityMetric(int argc, char *argv[])         
{ 
  typedef float  PixelType; 
  typedef itk::Vector<float,ImageDimension>         VectorType; 
  typedef itk::Image<VectorType,ImageDimension>     FieldType; 
  typedef itk::Image<PixelType,ImageDimension> ImageType; 
  typedef itk::ImageFileWriter<ImageType> writertype; 
  typedef typename ImageType::IndexType IndexType; 
  typedef typename ImageType::SizeType SizeType; 
  typedef typename ImageType::SpacingType SpacingType; 
  typedef itk::AffineTransform<double,ImageDimension>   AffineTransformType; 
  typedef itk::LinearInterpolateImageFunction<ImageType,double>  InterpolatorType1; 
  typedef itk::ImageRegionIteratorWithIndex<ImageType> Iterator; 
 
 
  typedef itk::Image<float,2> JointHistType; 
  typedef itk::ImageFileWriter<JointHistType> jhwritertype; 
 
// get command line params 
  unsigned int argct=2;
  unsigned int whichmetric = atoi(argv[argct]); argct++;
  std::string fn1 = std::string(argv[argct]); argct++;
  std::string fn2 = std::string(argv[argct]); argct++;
  std::string logfilename=""; 
  if (argc > argct) logfilename = std::string(argv[argct]); argct++; 
  std::string imgfilename=""; 
  if (argc > argct) imgfilename = std::string(argv[argct]); argct++; 
 
  typename ImageType::Pointer image1 = NULL;
  ReadImage<ImageType>(image1, fn1.c_str());  
  typename ImageType::Pointer image2 = NULL;  
  ReadImage<ImageType>(image2, fn2.c_str());  
 
/*
  typedef itk::ImageRegionIteratorWithIndex<FieldType> VIterator; 
  typename FieldType::Pointer field=FieldType::New(); 
  field->SetLargestPossibleRegion( image1->GetLargestPossibleRegion() ); 
  field->SetBufferedRegion( image1->GetLargestPossibleRegion() ); 
  field->SetLargestPossibleRegion( image1->GetLargestPossibleRegion() ); 
  field->Allocate();  
  field->SetSpacing(image1->GetSpacing()); 
  field->SetOrigin(image1->GetOrigin()); 
  VectorType zero; 
  zero.Fill(0); 
  VIterator vfIter2( field,  field->GetLargestPossibleRegion() );   
  for(  vfIter2.GoToBegin(); !vfIter2.IsAtEnd(); ++vfIter2 )  
    { 
      IndexType index=vfIter2.GetIndex(); 
      vfIter2.Set(zero); 
    } 
*/ 
  typename ImageType::Pointer metricimg=ImageType::New(); 
  metricimg->SetLargestPossibleRegion( image1->GetLargestPossibleRegion() ); 
  metricimg->SetBufferedRegion( image1->GetLargestPossibleRegion() ); 
  metricimg->SetLargestPossibleRegion( image1->GetLargestPossibleRegion() ); 
  metricimg->Allocate();  
  metricimg->SetSpacing(image1->GetSpacing()); 
  metricimg->SetOrigin(image1->GetOrigin()); 
  Iterator iter( metricimg,  metricimg->GetLargestPossibleRegion() );   
  for(  iter.GoToBegin(); !iter.IsAtEnd(); ++iter ) iter.Set(0); 
 
  typedef ImageType FixedImageType; 
  typedef ImageType MovingImageType; 
  typedef FieldType DeformationFieldType; 

  // Choose the similarity metric 
  typedef itk::AvantsMutualInformationRegistrationFunction<FixedImageType,MovingImageType,DeformationFieldType> MIMetricType; 
  typedef itk::CrossCorrelationRegistrationFunction<FixedImageType,MovingImageType,DeformationFieldType> CCMetricType; 
  //typedef itk::LandmarkCrossCorrelationRegistrationFunction<FixedImageType,MovingImageType,DeformationFieldType> MetricType; 
  //typename  
  typename MIMetricType::Pointer mimet=MIMetricType::New(); 
  typename CCMetricType::Pointer ccmet=CCMetricType::New(); 
   
  int nbins=32; 
 
  typename CCMetricType::RadiusType hradius; 
  typename CCMetricType::RadiusType ccradius; 
  ccradius.Fill(4); 
  typename MIMetricType::RadiusType miradius; 
  miradius.Fill(0); 
 
//  mimet->SetDeformationField(field); 
  mimet->SetFixedImage(image1); 
  mimet->SetMovingImage(image2); 
  mimet->SetRadius(miradius); 
  mimet->SetGradientStep(1.e2); 
  mimet->SetNormalizeGradient(false); 

//  ccmet->SetDeformationField(field); 
  ccmet->SetFixedImage(image1); 
  ccmet->SetMovingImage(image2); 
  ccmet->SetRadius(ccradius); 
  ccmet->SetGradientStep(1.e2); 
  ccmet->SetNormalizeGradient(false); 

  double metricvalue=0;
  std::string metricname="";
  if (whichmetric  == 0)
    {
    hradius=miradius;
    unsigned long ct = 0; 
    for(  iter.GoToBegin(); !iter.IsAtEnd(); ++iter )  
      { 
      IndexType index=iter.GetIndex(); 
      double fval=image1->GetPixel(index); 
      double mval=image2->GetPixel(index); 
      metricvalue+=fabs(fval-mval);
      ct++; 
      } 
    metricvalue/=(float)ct;
    metricname="MSQ ";
    }
  else if (whichmetric == 1 ) // imagedifference
    {
    hradius=ccradius;
    ccmet->InitializeIteration();
    metricvalue=ccmet->ComputeCrossCorrelation();
    metricname="CC ";
    }
  else 
    {
    hradius=miradius;
    mimet->InitializeIteration();
    metricvalue=mimet->ComputeMutualInformation();
   metricname="MI ";
     }
  std::cout << fn1 << " : " << fn2 << " => " <<  metricname << metricvalue << std::endl;
  if (logfilename.length() > 3 ){ 
  std::ofstream logfile;
  logfile.open(logfilename.c_str(), std::ofstream::app);
  if (logfile.good())
    {
    logfile <<  fn1 << " : " << fn2 << " => " << metricname << metricvalue << std::endl;
    }
  else std::cout << " cant open file ";
  logfile.close();

  }


  if (imgfilename.length() > 3 ){ 
/*
  typename MetricType::NeighborhoodType   asamIt( hradius, field,field->GetLargestPossibleRegion()); 
  unsigned long ct = 0; 
  double totval = 0; 
  for(  vfIter2.GoToBegin(); !vfIter2.IsAtEnd(); ++vfIter2 )  
    { 
      IndexType index=vfIter2.GetIndex(); 
      double val=0; 
      asamIt.SetLocation(index);
      //      met->ComputeUpdate( asamIt,  gd);
      met->ComputeMetricAtPairB(index,  zero);
      metricimg->SetPixel(index, val); 
      //if (ct % 10000 == 0)  
      //		std::cout << val << " index " << index << std::endl; 
      //      asamIt.SetLocation(index); 
      //      totval+=met->localProbabilistic; 
      ct++; 
    } 
    
  std::cout << " AvantsMI : " << totval/(double)ct << " E " <<  met->GetEnergy() <<  std::endl; 
  std::cout << " write begin " << std::endl; 
  typedef itk::ImageFileWriter<ImageType> writertype; 
  writertype::Pointer w= writertype::New(); 
  w->SetInput(metricimg); 
  w->SetFileName(outname.c_str()); 
  w->Write();  //  met->WriteImages(); 
   
  std::cout << " write end " << std::endl; 
*/  
 }
  

  return 0; 
  
 }      
 


int main(int argc, char *argv[])        
{

   
  if ( argc < 3 )     
    { std::cout << "Basic useage ex: " << std::endl;
    std::cout << argv[0] << " ImageDimension whichmetric image1.ext image2.ext {logfile} {outimage.ext}  " << std::endl;
    std::cout << "  outimage and logfile are optional  " << std::endl;
    return 1;
  }           

   // Get the image dimension
  switch( atoi(argv[1]))
   {
   case 2:
     ComputeSimilarityMetric<2>(argc,argv);
      break;
   case 3:
     ComputeSimilarityMetric<3>(argc,argv);
      break;
   default:
      std::cerr << "Unsupported dimension" << std::endl;
      exit( EXIT_FAILURE );
   }
	
  return 0;
} 

