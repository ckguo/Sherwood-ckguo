#include "FeatureResponseFunctions.h"

#include <cmath>

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
	VectorXi intOffset = randomOffset.cwiseAbs().cast<int>();
	Vector3i intHalfBoxSize = randomHalfBoxSize.cast<int>();

    return BoxOffsetFeatureResponse(intOffset, intHalfBoxSize);
  }

  MatrixXf BoxOffsetFeatureResponse::dataset_ = MatrixXf::Random(3, 3);

  float BoxOffsetFeatureResponse::GetResponse(const IDataPointCollection& data, unsigned int sampleIndex) const
  {
//    const DataPointCollection& concreteData = (DataPointCollection&)(data);
//    int sum = 0;
//    int count = 0;
//	for (int i = offset(2*dim); i < offset(2*dim+1), i++) {
//
//		sum += concreteData.GetDataPoint(j)[0];
//
//	}
//
//    return concreteData.GetDataPoint((int)sampleIndex)[axis_];
	  std::cout << offset_ << std::endl;
	  std::cout << halfBoxSize_ << std::endl;
	  std::cout << dataset_ << std::endl;
	  return 0;
  }

  std::string BoxOffsetFeatureResponse::ToString() const
  {
    std::stringstream s;
//    s << "BoxOffsetFeatureResponse(" << axis_ << ")";
    s << "BoxOffsetFeatureResponse";
    return s.str();
  }

} } }
