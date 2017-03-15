#include "FeatureResponseFunctions.h"

#include <cmath>
#include <algorithm>

#include <sstream>
#include <Eigen/Core>
#include <nifti1_io.h>

#include "DataPointCollection.h"
#include "Random.h"

namespace MicrosoftResearch { namespace Cambridge { namespace Sherwood
{

  using namespace Eigen;


  AxisAlignedFeatureResponse AxisAlignedFeatureResponse ::CreateRandom(Random& random)
  {
    return AxisAlignedFeatureResponse(random.Next(0, 2));
  }

  float AxisAlignedFeatureResponse::GetResponse(const IDataPointCollection& data, unsigned int sampleIndex) const
  {
    const DataPointCollection& concreteData = (DataPointCollection&)(data);
    return concreteData.GetDataPoint((int)sampleIndex)[axis_];
  }

  std::string AxisAlignedFeatureResponse::ToString() const
  {
    std::stringstream s;
    s << "AxisAlignedFeatureResponse(" << axis_ << ")";

    return s.str();
  }

  /// <returns>A new LinearFeatureResponse2d instance.</returns>
  LinearFeatureResponse2d LinearFeatureResponse2d::CreateRandom(Random& random)
  {
    double dx = 2.0 * random.NextDouble() - 1.0;
    double dy = 2.0 * random.NextDouble() - 1.0;

    double magnitude = sqrt(dx * dx + dy * dy);

    return LinearFeatureResponse2d((float)(dx / magnitude), (float)(dy / magnitude));
  }

  float LinearFeatureResponse2d::GetResponse(const IDataPointCollection& data, unsigned int index) const
  {
    const DataPointCollection& concreteData = (const DataPointCollection&)(data);
    return dx_ * concreteData.GetDataPoint((int)index)[0] + dy_ * concreteData.GetDataPoint((int)index)[1];
  }

  std::string LinearFeatureResponse2d::ToString() const
  {
    std::stringstream s;
    s << "LinearFeatureResponse(" << dx_ << "," << dy_ << ")";

    return s.str();
  }

  BoxOffsetFeatureResponse BoxOffsetFeatureResponse::CreateRandom(Random& random)
  {
	std::srand((unsigned int) time(0));
	VectorXf randomOffset = 50*VectorXf::Random(6);
    std::srand((unsigned int) time(0));
    Vector3f randomHalfBoxSize = 6*Vector3f::Random();
	VectorXi intOffset = randomOffset.cast<int>();
	Vector3i intHalfBoxSize = randomHalfBoxSize.cwiseAbs().cast<int>();

    return BoxOffsetFeatureResponse(intOffset, intHalfBoxSize);
  }

  nifti_image * nim = nifti_image_read("../../dataset_full/training_axial_full_resampled_pat0.nii.gz", 1);
  double * BoxOffsetFeatureResponse::dataset_ = (double*)nim->data;
//  std::cout << "hi" << std::endl;

  // returns the value at the pixel, if it is out of bounds it will return a padded value
  float BoxOffsetFeatureResponse::ValueAtPixel(int patient, int i, int j, int k) {
	  int dimx = nim->nx;
	  int dimy = nim->ny;
	  int dimz = nim->nz;

	  i = std::max(0, std::min(dimx-1, i));
	  j = std::max(0, std::min(dimy-1, j));
	  k = std::max(0, std::min(dimz-1, k));
	  double val = dataset_[i + dimx*j + dimx*dimy*k];
//	  if (val != 0) {
//		  std::cout << "value at pixel" << val << std::endl;
//	  }
	  return val;
  }

  float BoxOffsetFeatureResponse::GetResponse(const IDataPointCollection& data, unsigned int sampleIndex) const
  {
    const DataPointCollection& concreteData = (DataPointCollection&)(data);
    // data: p, i, j, k, (bounding offset)
    // p = patient number, (i,j,k) = coordinates
    float sum = 0;
    int count = 0;

    const float* datum = concreteData.GetDataPoint((int)sampleIndex);

    const float p = datum[0];
//    std::cout << "start" << std::endl;
//    std::cout << p << " " << datum[1] << " " << datum[2] << " " << datum[3] << std::endl;

    for (int i = offset_[0]-halfBoxSize_[0]; i <= offset_[0]+halfBoxSize_[0]; i++) {
        for (int j = offset_[1]-halfBoxSize_[1]; j <= offset_[1]+halfBoxSize_[1]; j++) {
            for (int k = offset_[2]-halfBoxSize_[2]; k <= offset_[2]+halfBoxSize_[2]; k++) {
            	sum += BoxOffsetFeatureResponse::ValueAtPixel(p, datum[1]+i, datum[2]+j, datum[3]+k);
            	count ++;
            }
        }
    }


    // check if numbers are right
//    std::cout << sum/count << std::endl;
//    std::cout << count << std::endl;

//	  std::cout << offset_ << std::endl;
//	  std::cout << halfBoxSize_ << std::endl;
//	  std::cout << "end" << std::endl;
	  return sum/count;
  }

  std::string BoxOffsetFeatureResponse::ToString() const
  {
    std::stringstream s;
//    s << "BoxOffsetFeatureResponse(" << axis_ << ")";
    s << "BoxOffsetFeatureResponse";
    return s.str();
  }

} } }
