/*=========================================================================

  Program:   Advanced Normalization Tools
  Module:    $RCSfile: itkCopperColormapFunctor.txx,v $
  Language:  C++
  Date:      $Date: 2009-05-15 02:47:59 $
  Version:   $Revision: 1.1 $

  Copyright (c) ConsortiumOfANTS. All rights reserved.
  See accompanying COPYING.txt or 
 http://sourceforge.net/projects/advants/files/ANTS/ANTSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkCopperColormapFunctor_txx
#define __itkCopperColormapFunctor_txx

#include "itkCopperColormapFunctor.h"

namespace itk {

namespace Functor {  

template <class TScalar, class TRGBPixel>
typename CopperColormapFunctor<TScalar, TRGBPixel>::RGBPixelType
CopperColormapFunctor<TScalar, TRGBPixel>
::operator()( const TScalar & v ) const
{
  // Map the input scalar between [0, 1].
  RealType value = this->RescaleInputValue( v );
  
  // Apply the color map.
  RealType red = 1.2 * value;
  red = vnl_math_min( 1.0, red );
  
  RealType green = 0.8 * value;
  
  RealType blue = 0.5 * value;  
  
  // Set the rgb components after rescaling the values.
  RGBPixelType pixel;
  
  pixel[0] = this->RescaleRGBComponentValue( red );
  pixel[1] = this->RescaleRGBComponentValue( green );
  pixel[2] = this->RescaleRGBComponentValue( blue );
                                          
  return pixel;
}

} // end namespace Functor

} // end namespace itk


#endif
