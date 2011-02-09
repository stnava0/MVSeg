/*=========================================================================

  Program:   Advanced Normalization Tools
  Module:    $RCSfile: itkJetColormapFunctor.h,v $
  Language:  C++
  Date:      $Date: 2009-05-15 19:22:31 $
  Version:   $Revision: 1.2 $

  Copyright (c) ConsortiumOfANTS. All rights reserved.
  See accompanying COPYING.txt or 
 http://sourceforge.net/projects/advants/files/ANTS/ANTSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkJetColormapFunctor_h
#define __itkJetColormapFunctor_h

#include "itkColormapFunctor.h"

namespace itk {

namespace Functor {  

/**
 * \class JetColormapFunctor
 * \brief Function object which maps a scalar value into an RGB colormap value.
 *
 * \author Nicholas Tustison, Hui Zhang, Gaetan Lehmann, Paul Yushkevich and ̈James C. Gee
 * 
 * This code was contributed in the Insight Journal paper:
 *
 * "Meeting Andy Warhol Somewhere Over the Rainbow: RGB Colormapping and ITK"
 * http://www.insight-journal.org/browse/publication/285
 * http://hdl.handle.net/1926/1452
 *
 */
template< class TScalar, class TRGBPixel >
class ITK_EXPORT JetColormapFunctor
: public ColormapFunctor<TScalar, TRGBPixel>
{
public:

  typedef JetColormapFunctor                     Self;
  typedef ColormapFunctor<TScalar, TRGBPixel>    Superclass;
  typedef SmartPointer<Self>                     Pointer;
  typedef SmartPointer<const Self>               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  typedef typename Superclass::RGBPixelType      RGBPixelType;
  typedef typename Superclass::ScalarType        ScalarType;  
  typedef typename Superclass::RealType          RealType;
  
  virtual RGBPixelType operator()( const TScalar & ) const;

protected:
  JetColormapFunctor() {};
  ~JetColormapFunctor() {};

private:
  JetColormapFunctor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
  
} // end namespace functor

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkJetColormapFunctor.txx"
#endif

#endif
