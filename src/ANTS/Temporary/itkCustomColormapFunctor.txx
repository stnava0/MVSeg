/*=========================================================================

  Program:   Advanced Normalization Tools
  Module:    $RCSfile: itkCustomColormapFunctor.txx,v $
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
#ifndef __itkCustomColormapFunctor_txx
#define __itkCustomColormapFunctor_txx

#include "itkCustomColormapFunctor.h"

namespace itk {

namespace Functor {

template <class TScalar, class TRGBPixel>
typename CustomColormapFunctor<TScalar, TRGBPixel>::RGBPixelType
CustomColormapFunctor<TScalar, TRGBPixel>
::operator()( const TScalar & v ) const
{
  // Map the input scalar between [0, 1].
  RealType value = this->RescaleInputValue( v );

  // Apply the color mapping.
  RealType red = 0.0;
  if ( this->m_RedChannel.size() == 1 || value == 0.0 )
    {
    red = this->m_RedChannel[0];
    }
  else if ( this->m_RedChannel.size() > 1 )
    {
    RealType size = static_cast<RealType>( this->m_RedChannel.size() );
    RealType index = vcl_ceil( value * ( size - 1.0 ) );
    RealType p1 = this->m_RedChannel[static_cast<unsigned int>( index )];
    RealType m1 = this->m_RedChannel[static_cast<unsigned int>( index-1 )];
    RealType d = p1 - m1;
    red = d * ( size - 1.0 ) * ( value - ( index - 1.0 )/( size - 1.0 ) )
      + m1;
    }

  RealType green = 0.0;
  if ( this->m_GreenChannel.size() == 1 || value == 0.0 )
    {
    green = this->m_GreenChannel[0];
    }
  else if ( this->m_GreenChannel.size() > 1 )
    {
    RealType size = static_cast<RealType>( this->m_GreenChannel.size() );
    RealType index = vcl_ceil( value * ( size - 1.0 ) );
    RealType p1 = this->m_GreenChannel[static_cast<unsigned int>( index )];
    RealType m1 = this->m_GreenChannel[static_cast<unsigned int>( index-1 )];
    RealType d = p1 - m1;
    green = d * ( size - 1.0 ) * ( value - ( index - 1.0 )/( size - 1.0 ) )
      + m1;
    }

  RealType blue = 0.0;
  if ( this->m_BlueChannel.size() == 1 || value == 0.0 )
    {
    blue = this->m_BlueChannel[0];
    }
  else if ( this->m_BlueChannel.size() > 1 )
    {
    RealType size = static_cast<RealType>( this->m_BlueChannel.size() );
    RealType index = vcl_ceil( value * ( size - 1.0 ) );
    RealType p1 = this->m_BlueChannel[static_cast<unsigned int>( index )];
    RealType m1 = this->m_BlueChannel[static_cast<unsigned int>( index-1 )];
    RealType d = p1 - m1;
    blue = d * ( size - 1.0 ) * ( value - ( index - 1.0 )/( size - 1.0 ) )
      + m1;
    }

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
