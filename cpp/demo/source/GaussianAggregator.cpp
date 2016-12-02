#include "GaussianAggregator.h"
#include <Eigen/Core>
#include <Eigen/LU>
#include <iostream>

#include "DataPointCollection.h"

namespace MicrosoftResearch { namespace Cambridge { namespace Sherwood
{
  using namespace Eigen;

  GaussianPdfNd::GaussianPdfNd(unsigned int dim, VectorXd& means, MatrixXd& cov)
  {
//	std::cout << "create PDF" << std::endl;
    dim_ = dim;
	means_ = means;
	cov_ = cov;

	det_Sigma_ = cov_.determinant();

//	std::cout << "means: " << means_ << std::endl;
//	std::cout << "cov: " << cov_ << std::endl;
//	std::cout << "det: " << det_Sigma_ << std::endl;

    if (det_Sigma_ < 0.0)
      throw std::runtime_error("Gaussian covaraince matrix must have determinant>0.0.");

    log_det_Sigma_ = log(det_Sigma_);

    inv_cov_ = cov_.inverse();

  }

  double GaussianPdfNd::GetProbability(Eigen::VectorXd& x) const
  {
//	std::cout << "PDF get probability" << std::endl;

    VectorXd v = x - means_;
    MatrixXd val = (-0.5 * v.transpose() * inv_cov_) * v;
    double exponent = val(0, 0);

    return pow(2.0 * 3.14159265, -0.5*dim_) * pow(det_Sigma_, -0.5) * exp(exponent);
  }

  double GaussianPdfNd::GetNegativeLogProbability(Eigen::VectorXd& x) const
  {
//	std::cout << "PDF get negative log probability" << std::endl;

	VectorXd v = x - means_;

    MatrixXd val = (-0.5 * v.transpose() * inv_cov_) * v;
    double exponent = val(0, 0);

    double result = 0.5 * log_det_Sigma_ + exponent;

    return result;
  }

  double GaussianPdfNd::Entropy() const
  {
	std::cout << "PDF get entropy" << std::endl;

//    double determinant = Sigma_11_ * Sigma_22_ - Sigma_12_ * Sigma_12_;
//
//    if (determinant <= 0.0)
//    {
//      // If we used a sensible prior, this wouldn't happen. So the user can test
//      // without a prior, we fail gracefully.
//      return std::numeric_limits<double>::infinity();
//    }

    return 0.5 * (dim_ * log(2.0 * 3.141593 * 2.718282) + log_det_Sigma_);

  }

  GaussianAggregatorNd::GaussianAggregatorNd(unsigned int dim, double a, double b)
  {
	std::cout << "create aggregator" << std::endl;
	std::cout << a << b << std::endl;

    assert(a > 0);
    assert(b > 0); // Hyperparameters a and b must be greater than or equal to zero.

    dim_ = dim;
    sum_ = VectorXd::Zero(dim_);
    squares_ = MatrixXd::Zero(dim_, dim_);

    sampleCount_ = 0;

    a_ = a;
    b_ = b;

  }

  GaussianPdfNd GaussianAggregatorNd::GetPdf() const
  {
//	std::cout << "get PDF" << std::endl;

    // Compute maximum likelihood mean and covariance matrix
//	std::cout << "1sum: "<< sum_ << std::endl;
    VectorXd means = sum_/sampleCount_;
//    std::cout << "1means: "<< means << std::endl;
    MatrixXd variances(dim_, dim_);
    for (int i = 0; i < dim_; i++) {
      for (int j = 0; j < dim_; j++) {
        variances(i, j) = squares_(i,j) / sampleCount_ - sum_(i) * sum_(j) / (sampleCount_*sampleCount_);
      }
    }

    // Adapt using conjugate prior
    double alpha = sampleCount_/(sampleCount_ + a_);

    for (int i = 0; i < dim_; i++) {
      for (int j = 0; j < dim_; j++) {
        if (i==j) {
          variances(i, i) = alpha*variances(i, i) + (1-alpha)*b_;
        }
        else {
          variances(i, j) = alpha*variances(i, j);
        }
      }
    }

//    std::cout << mx << " " << my << std::endl;
    return GaussianPdfNd(dim_, means, variances);
  }

  // IStatisticsAggregator implementation
  void GaussianAggregatorNd::Clear()
  {
	std::cout << "clear aggregator" << std::endl;
	std::cout << dim_ << std::endl;

    sum_ = VectorXd::Zero(dim_);
    squares_ = MatrixXd::Zero(dim_, dim_);
    sampleCount_ = 0;
  }

  void GaussianAggregatorNd::Aggregate(const IDataPointCollection& data, unsigned int index)
  {
//	std::cout << "aggregate a data point" << std::endl;

    const DataPointCollection& concreteData = (const DataPointCollection&)(data);

    const float* datum = concreteData.GetDataPoint((int)index);
    float target = concreteData.GetTarget((int)index);

    VectorXf vector(dim_);
    for (int i = 0; i < dim_-1; i++) {
    	vector(i) = datum[i];
    	sum_(i) += datum[i];
    }
    vector(dim_-1) = target;
    sum_(dim_-1) += target;

    //TODO: write in linalg
    for (int i = 0; i < dim_; i++) {
      for (int j = 0; j < dim_; j++) {
        squares_(i, j) += vector(i)*vector(j);
      }
    }

//    std::cout << concreteData.GetDataPoint((int)index)[0] << " " << concreteData.GetDataPoint((int)index)[1] << " " << concreteData.GetTarget((int)index) << std::endl;

    sampleCount_ += 1;

//	std::cout << sum_ << std::endl;
//	std::cout << squares_ << std::endl;


  }

  void GaussianAggregatorNd::Aggregate(const GaussianAggregatorNd& aggregator)
  {
	std::cout << "aggregate two aggregators" << std::endl;

    sum_ += aggregator.sum_;
    squares_ += aggregator.squares_;

    sampleCount_ += aggregator.sampleCount_;

	std::cout << sum_ << std::endl;
	std::cout << squares_ << std::endl;

  }

  GaussianAggregatorNd GaussianAggregatorNd::DeepClone() const
  {
	std::cout << "clone aggregator" << std::endl;

    GaussianAggregatorNd result(dim_, a_, b_);

    VectorXd sum = VectorXd::Zero(dim_);
    MatrixXd squares = MatrixXd::Zero(dim_, dim_);

    for (int i = 0; i < dim_; i++) {
      sum(i) = sum_(i);
      for (int j = 0; j < dim_; j++) {
        squares(i, j) = squares_(i, j);
      }
    }

    result.sum_ = sum;
    result.squares_ = squares;

    result.sampleCount_ = sampleCount_;

    return result;
  }

}}}
