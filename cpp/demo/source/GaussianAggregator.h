#pragma once

// This file defines a IStatisticsAggregator implementation used by the
// example code in Classification.h, DensityEstimation.h, etc. Note we
// represent IStatisticsAggregator instances using simple structs so that all
// tree data can be stored contiguously in a linear array.

#include <Eigen/Core>
#include <math.h>

#include <limits>
#include <vector>

#include "Sherwood.h"

#include "DataPointCollection.h"

namespace MicrosoftResearch { namespace Cambridge { namespace Sherwood
{

  class GaussianPdfNd
  {
  private:
	unsigned int dim_;
	unsigned int data_dim_;
	unsigned int target_dim_;
    Eigen::VectorXd means_;
    Eigen::MatrixXd cov_; // symmetric 2x2 covariance matrix
    Eigen::MatrixXd inv_cov_; // symmetric 2x2 inverse covariance matrix
    double det_Sigma_;
    double log_det_Sigma_;

  public:
    GaussianPdfNd() { }

    GaussianPdfNd(unsigned int dim, Eigen::VectorXd& means, Eigen::MatrixXd& cov);

    double Mean(unsigned int i) const
    {
      return means_(i);
    }

    double Variance(unsigned int i, unsigned int j) const
    {
      return cov_(i,j);
    }

    double GetProbability(Eigen::VectorXd& x) const;

    double GetNegativeLogProbability(Eigen::VectorXd& x) const;

    double Entropy() const;
  };

  struct GaussianAggregatorNd
  {
  private:
    unsigned int sampleCount_;
    unsigned int data_dim_;
    unsigned int target_dim_;
    unsigned int dim_;
    Eigen::VectorXd sum_;    // sum
    Eigen::MatrixXd squares_;  // sum squares

    double a_;
	double b_;      // hyperparameters of prior

  public:
    GaussianAggregatorNd()
    {
      std::cout << "aggregator constructor" << std::endl;
      dim_ = 1;
      Clear();
    }

    GaussianAggregatorNd(unsigned int data_dim, unsigned int target_dim, double a, double b);

    GaussianPdfNd GetPdf() const;

    unsigned int SampleCount() const {  return sampleCount_; }

    // IStatisticsAggregator implementation
    void Clear();

    void Aggregate(const IDataPointCollection& data, unsigned int index);

    void Aggregate(const GaussianAggregatorNd& aggregator);

    GaussianAggregatorNd DeepClone() const;
  };
}}}
