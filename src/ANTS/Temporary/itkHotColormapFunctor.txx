/*=========================================================================

  Program:   Advanced Normalization Tools
  Module:    $RCSfile: itkHotColormapFunctor.txx,v $
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
#ifndef __itkHotColormapFunctor_txx
#define __itkHotColormapFunctor_txx

#include "itkHotColormapFunctor.h"

namespace itk {

namespace Functor {

template <class TScalar, class TRGBPixel>
typename HotColormapFunctor<TScalar, TRGBPixel>::RGBPixelType
HotColormapFunctor<TScalar, TRGBPixel>
::operator()( const TScalar & v ) const
{
  // Map the input scalar between [0, 1].
  RealType value = this->RescaleInputValue( v );

  // Apply the color mapping.
  RealType red   = 63.0/26.0 * value - 1.0/13.0;
  red = vnl_math_max( 0.0, red );
  red = vnl_math_min( 1.0, red );

  RealType green = 63.0/26.0 * value - 11.0/13.0;
  green = vnl_math_max( 0.0, green );
  green = vnl_math_min( 1.0, green );

  RealType blue  = 4.5 * value - 3.5;
  blue = vnl_math_max( 0.0, blue );
  blue = vnl_math_min( 1.0, blue );

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
