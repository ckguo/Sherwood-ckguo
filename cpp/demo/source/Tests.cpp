/*
 * Tests.cpp
 *
 *  Created on: Jan 10, 2017
 *      Author: courtneyguo
 */
#include <assert.h>

#include "Sherwood.h"
#include "GaussianAggregator.h"
#include "Platform.h"
#include "FeatureResponseFunctions.h"
#include "RegressionGaussianNd.h"

using namespace MicrosoftResearch::Cambridge::Sherwood;

std::auto_ptr<DataPointCollection> LoadTrainingData(
  const std::string& filename,
  const std::string& alternativePath,
  int dimension,
  int targetDimension,
  DataDescriptor::e descriptor);

int main() {
	GaussianAggregatorNd test1 = GaussianAggregatorNd(2, 2, 1, 1);
	GaussianAggregatorNd test2 = GaussianAggregatorNd(2, 2, 1, 1);
	GaussianAggregatorNd test3 = GaussianAggregatorNd(2, 2, 1, 1);
    std::auto_ptr<DataPointCollection> trainingData = std::auto_ptr<DataPointCollection>(LoadTrainingData(
      "demo/data/regression/test.txt",
	  "/data/regression/test.txt",
      2,
	  2,
      DataDescriptor::HasTargetValues ) );
    test1.Aggregate(*trainingData, 1);
    test1.Aggregate(*trainingData, 2);
    test2.Aggregate(*trainingData, 2);
    test3.Aggregate(*trainingData, 1);
    test3.Aggregate(test2);
	for (int i = 0; i < 2; i++) {
		assert(test1.GetPdf().Mean(i) == test3.GetPdf().Mean(i));
	    std::cout << "means " << i << ": " << test1.GetPdf().Mean(i) << " " << test3.GetPdf().Mean(i) << std::endl;
		for (int j = 0; j < 2; j++) {
			assert(test1.GetPdf().Variance(i,j) == test3.GetPdf().Variance(i,j));
			std::cout << "covariance " << i << j << ": " << test1.GetPdf().Variance(i, j) << " " << test3.GetPdf().Variance(i, j) << std::endl;
		}
	}

    std::auto_ptr<DataPointCollection> trainingData2 = std::auto_ptr<DataPointCollection>(LoadTrainingData(
      "demo/data/regression/test2.txt",
	  "/data/regression/test2.txt",
      1,
	  1,
      DataDescriptor::HasTargetValues ) );

    TrainingParameters parameters;
    parameters.MaxDecisionLevels = 1;
    parameters.NumberOfCandidateFeatures = 10;
    parameters.NumberOfCandidateThresholdsPerFeature = 1;
    parameters.NumberOfTrees = 1;

    std::auto_ptr<Forest<AxisAlignedFeatureResponse, GaussianAggregatorNd> > forest = RegressionGaussianExampleNd::Train(
      *trainingData2.get(), parameters);
    Forest<AxisAlignedFeatureResponse, GaussianAggregatorNd>& forestGet = *forest.get();

	Node<AxisAlignedFeatureResponse, GaussianAggregatorNd> node1 = forestGet.GetTree(0).GetNode(1);
	Node<AxisAlignedFeatureResponse, GaussianAggregatorNd> node2 = forestGet.GetTree(0).GetNode(2);

	const GaussianAggregatorNd& leafStatistics1 = node1.TrainingDataStatistics;
	const GaussianAggregatorNd& leafStatistics2 = node2.TrainingDataStatistics;

	std::cout << leafStatistics1.GetPdf().Mean(0) << std::endl;
//	std::cout << leafStatistics1.GetPdf().Mean(1) << std::endl;

	std::cout << leafStatistics2.GetPdf().Mean(0) << std::endl;
//	std::cout << leafStatistics2.GetPdf().Mean(1) << std::endl;


}

std::auto_ptr<DataPointCollection> LoadTrainingData(
  const std::string& filename,
  const std::string& alternativePath,
  int dimension,
  int targetDimension,
  DataDescriptor::e descriptor)
{
  std::ifstream r;

  r.open(filename.c_str());

  if(r.fail())
  {
    std::string path;

    try
    {
      path = GetExecutablePath();
    }
    catch(std::runtime_error& e)
    {
      std::cout<< "Failed to determine executable path. " << e.what();
      return std::auto_ptr<DataPointCollection>(0);
    }

    path = path + alternativePath;

    r.open(path.c_str());

    if(r.fail())
    {
      std::cout << "Failed to open either \"" << filename << "\" or \"" << path.c_str() << "\"." << std::endl;
      return std::auto_ptr<DataPointCollection>(0);
    }
  }

  std::auto_ptr<DataPointCollection> trainingData;
  try
  {
    trainingData = DataPointCollection::Load (
      r,
      dimension,
	  targetDimension,
      descriptor );
  }
  catch (std::runtime_error& e)
  {
    std::cout << "Failed to read training data. " << e.what() << std::endl;
    return std::auto_ptr<DataPointCollection>(0);
  }

  if (trainingData->Count() < 1)
  {
    std::cout << "Insufficient training data." << std::endl;
    return std::auto_ptr<DataPointCollection>(0);
  }

  return trainingData;
}

void DisplayTextFiles(const std::string& relativePath)
{
  std::string path;

  try
  {
    path = GetExecutablePath();
  }
  catch(std::runtime_error& e)
  {
    std::cout<< "Failed to find demo data files. " << e.what();
    return;
  }

  path = path + relativePath;

  std::vector<std::string> filenames;

  try
  {
    GetDirectoryListing(path, filenames, ".txt");
  }
  catch(std::runtime_error& e)
  {
    std::cout<< "Failed to list demo data files. " << e.what();
    return;
  }

  if (filenames.size() > 0)
  {
    std::cout << "The following demo data files can be specified as if they were on your current path:-" << std::endl;

    for(std::vector<std::string>::size_type i=0; i<filenames.size(); i++)
      std::cout << "  " << filenames[i].c_str() << std::endl;
  }
}
