#pragma once

// This file defines some IFeatureResponse implementations used by the example code in
// Classification.h, DensityEstimation.h, etc. Note we represent IFeatureResponse
// instances using simple structs so that all tree data can be stored
// contiguously in a linear array.

#include <string>
#include <Eigen/Core>
#include <nifti1_io.h>

#include "Sherwood.h"


namespace MicrosoftResearch { namespace Cambridge { namespace Sherwood
{
  class Random;

  /// <summary>
  /// A feature that orders data points using one of their coordinates,
  /// i.e. by projecting them onto a coordinate axis.
  /// </summary>
  class AxisAlignedFeatureResponse
  {
    int axis_;

  public:
    AxisAlignedFeatureResponse()
    {
      axis_ = -1;
    }

    /// <summary>
    /// Create an AxisAlignedFeatureResponse instance for the specified axis.
    /// </summary>
    /// <param name="axis">The zero-based index of the axis.</param>
    AxisAlignedFeatureResponse(int axis)
    {
      axis_ = axis;
    }

    /// <summary>
    /// Create an AxisAlignedFeatureResponse instance with a random choice of axis.
    /// </summary>
    /// <param name="randomNumberGenerator">A random number generator.</param>
    /// <returns>A new AxisAlignedFeatureResponse instance.</returns>
    static AxisAlignedFeatureResponse CreateRandom(Random& random);

    int Axis() const
    {
      return axis_;
    }

    // IFeatureResponse implementation
    float GetResponse(const IDataPointCollection& data, unsigned int sampleIndex) const;

    std::string ToString() const;
  };

  /// <summary>
  /// A feature that orders data points using a linear combination of their
  /// coordinates, i.e. by projecting them onto a given direction vector.
  /// </summary>
  class LinearFeatureResponse2d
  {
    float dx_, dy_;

  public:
    LinearFeatureResponse2d()
    {
      dx_ = 0.0;
      dy_ = 0.0;
    }

    /// <summary>
    /// Create a LinearFeatureResponse2d instance for the specified direction vector.
    /// </summary>
    /// <param name="dx">The first element of the direction vector.</param>
    /// <param name="dx">The second element of the direction vector.</param> 
    LinearFeatureResponse2d(float dx, float dy)
    {
      dx_ = dx; dy_ = dy;
    }

    /// <summary>
    /// Create a LinearFeatureResponse2d instance with a random direction vector.
    /// </summary>
    /// <param name="randomNumberGenerator">A random number generator.</param>
    /// <returns>A new LinearFeatureResponse2d instance.</returns>
    static LinearFeatureResponse2d CreateRandom(Random& random);

    // IFeatureResponse implementation
    float GetResponse(const IDataPointCollection& data, unsigned int index) const;

    std::string ToString()  const;
  };	

  class BoxOffsetFeatureResponse
    {
	  Eigen::VectorXi offset_;
	  Eigen::Vector3i halfBoxSize_;
	  static double * dataset_;

    public:
      BoxOffsetFeatureResponse()
      {
        offset_ = Eigen::VectorXi::Zero(6);
        halfBoxSize_ = Eigen::Vector3i::Zero();
      }

      /// <summary>
      /// Create an AxisAlignedFeatureResponse instance for the specified axis.
      /// </summary>
      /// <param name="axis">The zero-based index of the axis.</param>
      BoxOffsetFeatureResponse(Eigen::VectorXi offset, Eigen::Vector3i halfBoxSize)
      {
        offset_ = offset;
        halfBoxSize_ = halfBoxSize;
      }

      static float ValueAtPixel(int patient, int i, int j, int k);

      /// <summary>
      /// Create an AxisAlignedFeatureResponse instance with a random choice of axis.
      /// </summary>
      /// <param name="randomNumberGenerator">A random number generator.</param>
      /// <returns>A new AxisAlignedFeatureResponse instance.</returns>
      static BoxOffsetFeatureResponse CreateRandom(Random& random);

      // IFeatureResponse implementation
      float GetResponse(const IDataPointCollection& data, unsigned int sampleIndex) const;

      std::string ToString() const;
    };

} } }
