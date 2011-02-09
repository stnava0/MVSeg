/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkConvertArrayPixelBuffer.h,v $
  Language:  C++
  Date:      $Date: 2010-08-14 23:24:28 $
  Version:   $Revision: 0.00 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkConvertArrayPixelBuffer_h
#define __itkConvertArrayPixelBuffer_h

#include "itkConvertPixelBuffer.h"
#include "itkArray.h"

namespace itk
{
/**
 * \class ConvertPixelBuffer
 *  \brief Class to convert blocks of data from one type to another.
 *
 * ConvertPixelBuffer has a static method Convert().  It is used by
 * itkImageFileReader to convert from an unknown type at run-time to the
 * compile-time template pixel type in itkImageFileReader.  To work with
 * complex pixel types like RGB and RGBA a traits class is used.
 * OutputConvertTraits() is the traits class.  The default one used is
 * DefaultConvertPixelTraits.  
 *
 */
                                        
template <typename InputPixelType, typename T, typename OutputConvertTraits>
class ConvertPixelBuffer<InputPixelType, Array<T>, OutputConvertTraits> 
{
public:
  /** Determine the output data type. */
  typedef typename OutputConvertTraits::ComponentType OutputComponentType;

  /** General method converts from one type to another. */
  static void Convert(InputPixelType* inputData, 
                      int inputNumberOfComponents, 
                      Array<T>* outputData , 
                      size_t size);
  static void ConvertVectorImage(InputPixelType* inputData, 
                      int inputNumberOfComponents, 
                      Array<T>* outputData , 
                      size_t size);
private:
  ConvertPixelBuffer();
  ~ConvertPixelBuffer();
};
} //namespace ITK

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConvertArrayPixelBuffer.txx"
#endif

#endif // __itkConvertArrayPixelBuffer_h
