/*=========================================================================

  Program:   Advanced Normalization Tools
  Module:    $RCSfile: antsGaussianListSampleFunction.txx,v $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) ConsortiumOfANTS. All rights reserved.
  See accompanying COPYING.txt or
  http://sourceforge.net/projects/advants/files/ANTS/ANTSCopyright.txt
  for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __antsGaussianListSampleFunction_txx
#define __antsGaussianListSampleFunction_txx

#include "antsGaussianListSampleFunction.h"

#include "itkCovarianceSampleFilter.h"
#include "itkMeanSampleFilter.h"
#include "itkWeightedCovarianceSampleFilter.h"
#include "itkWeightedMeanSampleFilter.h"

namespace itk {
namespace ants {
namespace Statistics {

template <class TListSample, class TOutput, class TCoordRep>
GaussianListSampleFunction<TListSample, TOutput, TCoordRep>
::GaussianListSampleFunction()
{
  this->m_Gaussian = GaussianType::New();
}

template <class TListSample, class TOutput, class TCoordRep>
GaussianListSampleFunction<TListSample, TOutput, TCoordRep>
::~GaussianListSampleFunction()
{
}

template <class TListSample, class TOutput, class TCoordRep>
void
GaussianListSampleFunction<TListSample, TOutput, TCoordRep>
::SetInputListSample( const InputListSampleType * ptr )
{
  Superclass::SetInputListSample( ptr );

  if( !this->GetInputListSample() )
    {
    return;
    }

  if( this->GetInputListSample()->Size() > 1 )
    {
    if( this->GetListSampleWeights()->Size() == this->GetInputListSample()->Size() )
      {
      typedef typename itk::Statistics::
        WeightedCovarianceSampleFilter<InputListSampleType> CovarianceCalculatorType;
      typename CovarianceCalculatorType::Pointer covarianceCalculator =
        CovarianceCalculatorType::New();

      covarianceCalculator->SetWeights( *this->GetListSampleWeights() );
      covarianceCalculator->SetInput( this->GetInputListSample() );
      covarianceCalculator->Update();

      typename GaussianType::MeanType mean;
      NumericTraits<typename GaussianType::MeanType>::SetLength( mean,
        this->GetInputListSample()->GetMeasurementVectorSize() );
      for( unsigned int d = 0; d < this->GetInputListSample()->
        GetMeasurementVectorSize(); d++ )
        {
        mean[d] = covarianceCalculator->GetMean()[d];
        }
      this->m_Gaussian->SetMean( mean );
      this->m_Gaussian->SetCovariance( covarianceCalculator->GetCovarianceMatrix() );
      }
    else
      {
      typedef itk::Statistics::CovarianceSampleFilter<InputListSampleType>
        CovarianceCalculatorType;
      typename CovarianceCalculatorType::Pointer covarianceCalculator =
        CovarianceCalculatorType::New();
      covarianceCalculator->SetInput( this->GetInputListSample() );
      covarianceCalculator->Update();

      typename GaussianType::MeanType mean;
      NumericTraits<typename GaussianType::MeanType>::SetLength( mean,
        this->GetInputListSample()->GetMeasurementVectorSize() );
      for( unsigned int d = 0; d < this->GetInputListSample()->
        GetMeasurementVectorSize(); d++ )
        {
        mean[d] = covarianceCalculator->GetMean()[d];
        }
      this->m_Gaussian->SetMean( mean );
      this->m_Gaussian->SetCovariance( covarianceCalculator->GetCovarianceMatrix() );
      }
    }
  else
    {
    itkWarningMacro( "The input list sample has <= 1 element." <<
      "Function evaluations will be equal to 0." );
    }

}

template <class TListSample, class TOutput, class TCoordRep>
TOutput
GaussianListSampleFunction<TListSample, TOutput, TCoordRep>
::Evaluate( const InputMeasurementVectorType &measurement ) const
{
  try
    {
    return this->m_Gaussian->Evaluate( measurement );
    }
  catch(...)
    {
    return 0.0;
    }
}

/**
 * Standard "PrintSelf" method
 */
template <class TListSample, class TOutput, class TCoordRep>
void
GaussianListSampleFunction<TListSample, TOutput, TCoordRep>
::PrintSelf(
  std::ostream& os,
  Indent indent) const
{
  os << indent << "mean = " << this->m_Gaussian->GetMean() << ", ";

  typename GaussianType::CovarianceType covariance =
    this->m_Gaussian->GetCovariance();
  os << "covariance = [";
  for( unsigned int r = 0; r < covariance.Rows(); r++ )
    {
    for( unsigned int c = 0; c < covariance.Cols() - 1; c++ )
      {
      os << covariance( r, c ) << ", ";
      }
    if( r == covariance.Rows() - 1 )
      {
      os << covariance( r, covariance.Cols() - 1 ) << "]" << std::endl;
      }
    else
      {
      os << covariance( r, covariance.Cols() - 1 ) << "; ";
      }
    }

}

} // end of namespace Statistics
} // end of namespace ants
} // end of namespace itk

#endif
