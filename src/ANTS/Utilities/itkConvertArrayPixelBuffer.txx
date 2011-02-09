/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkConvertArrayPixelBuffer.txx,v $
  Language:  C++
  Date:      $Date: 2010-08-14 19:35:57 $
  Version:   $Revision: 1.27 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkConvertArrayPixelBuffer_txx
#define __itkConvertArrayPixelBuffer_txx

#include "itkConvertArrayPixelBuffer.h"

namespace itk
{
template< typename InputPixelType, typename T, typename OutputConvertTraits >
void
ConvertPixelBuffer< InputPixelType, Array< T >, OutputConvertTraits >
::Convert(InputPixelType *inputData, int inputNumberOfComponents, Array< T > *outputData, size_t size)
{
  InputPixelType *endInput = inputData + size * static_cast< size_t >( inputNumberOfComponents );

  while ( inputData != endInput )
    {
    ( *outputData ).SetSize(inputNumberOfComponents);
    for ( int ii = 0; ii < inputNumberOfComponents; ++ii )
      {
      OutputConvertTraits::SetNthComponent( ii, *outputData, static_cast< T >( *inputData++ ) );
      }

    outputData++;
    }
}

template< typename InputPixelType, typename T, typename OutputConvertTraits >
void
ConvertPixelBuffer< InputPixelType, Array< T >, OutputConvertTraits >
::ConvertVectorImage(InputPixelType *inputData, int inputNumberOfComponents, Array< T > *outputData, size_t size)
{
	size_t length = size* (size_t)inputNumberOfComponents;
	for( size_t i=0; i< length; i++ )
	{
		( *outputData ).SetSize(1);
		OutputConvertTraits::SetNthComponent( 0, *outputData, static_cast <  OutputComponentType >( *inputData ));
		++outputData;
		++inputData;
	}
}
} // end namespace itk

#endif
