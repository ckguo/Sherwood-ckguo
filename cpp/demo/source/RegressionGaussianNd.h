#pragma once

// This file defines types used to illustrate the use of the decision forest
// library in simple 1D to 1D regression task.
#include <Eigen/Core>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

#include "PlotCanvas.h"
#include "Graphics.h"

#include "Sherwood.h"

#include "DataPointCollection.h"
#include "FeatureResponseFunctions.h"
#include "GaussianAggregator.h"
#include "RegressionGaussian.h"

namespace MicrosoftResearch { namespace Cambridge { namespace Sherwood
{
  class RegressionGaussianNdTrainingContext : public ITrainingContext<AxisAlignedFeatureResponse, GaussianAggregatorNd>
  {
  public:
    // Implementation of ITrainingContext
	  AxisAlignedFeatureResponse GetRandomFeature(Random& random)
    {
      return AxisAlignedFeatureResponse(random.Next()%3);
    }

    GaussianAggregatorNd GetStatisticsAggregator()
    {
      return GaussianAggregatorNd(4, 1, 1);
    }

    double ComputeInformationGain(const GaussianAggregatorNd& allStatistics, const GaussianAggregatorNd& leftStatistics, const GaussianAggregatorNd& rightStatistics)
    {
      double entropyBefore = ((GaussianAggregatorNd)(allStatistics)).GetPdf().Entropy();

      GaussianAggregatorNd leftLineFitStats = (GaussianAggregatorNd)(leftStatistics);
      GaussianAggregatorNd rightLineFitStatistics = (GaussianAggregatorNd)(rightStatistics);

      unsigned int nTotalSamples = leftLineFitStats.SampleCount() + rightLineFitStatistics.SampleCount();

      double entropyAfter = (leftLineFitStats.SampleCount() * leftLineFitStats.GetPdf().Entropy() + rightLineFitStatistics.SampleCount() * rightLineFitStatistics.GetPdf().Entropy()) / nTotalSamples;

      return entropyBefore - entropyAfter;
    }

    bool ShouldTerminate(const GaussianAggregatorNd& parent, const GaussianAggregatorNd& leftChild, const GaussianAggregatorNd& rightChild, double gain)
    {
      return gain < 0.05;
    }
  };

  class RegressionGaussianExampleNd
  {
  public:
    static const PixelBgr DensityColor;
    static const PixelBgr DataPointColor;
    static const PixelBgr DataPointBorderColor;
    static const PixelBgr MeanColor;

    static std::auto_ptr<Forest<AxisAlignedFeatureResponse, GaussianAggregatorNd> > Train(
      const DataPointCollection& trainingData,
      const TrainingParameters& parameters)
    {
//        std::cout << "hello" << std::endl;
      std::cout << "Training the forest..." << std::endl;

      Random random;

      RegressionGaussianNdTrainingContext regressionTrainingContext;

      std::auto_ptr<Forest<AxisAlignedFeatureResponse, GaussianAggregatorNd> > forest
        = ForestTrainer<AxisAlignedFeatureResponse, GaussianAggregatorNd>::TrainForest(
        random, parameters, regressionTrainingContext, trainingData);

      return forest;
    }

    static std::auto_ptr<Bitmap<PixelBgr> > Visualize(
      Forest<AxisAlignedFeatureResponse, GaussianAggregatorNd>& forest,
      const DataPointCollection& trainingData,
      Size PlotSize,
      PointF PlotDilation )
    {
      // Generate some test samples in a grid pattern (a useful basis for creating visualization images)
      PlotCanvas plotCanvas(trainingData.GetRange(0), trainingData.GetTargetRange(), PlotSize, PlotDilation);

      float[3] bottom = {0, 0, 0};
      float[3] top = {0, 0 0};
      std::auto_ptr<DataPointCollection> testData = DataPointCollection::GenerateNdGrid(3, bottom, top, 1);

      std::cout << "\nApplying the forest to test data..." << std::endl;

      std::vector<std::vector<int> > leafNodeIndices;
      forest.Apply(*testData.get(), leafNodeIndices);
      std::cout << "graphing" << std::endl;

          // Aggregate statistics for this sample over all trees
          for (int t = 0; t < forest.TreeCount(); t++)
          {

//          	std::cout << "t = " << t << " tree count = " << forest.TreeCount() << std::endl;


            Node<AxisAlignedFeatureResponse, GaussianAggregatorNd> leafNodeCopy = forest.GetTree((t)).GetNode(leafNodeIndices[t][i]);

//            std::cout << "got node" << std::endl;

            const GaussianAggregatorNd& leafStatistics = leafNodeCopy.TrainingDataStatistics;

//            std::cout << "got stats" << std::endl;
            Eigen::VectorXd v(4);
            v << 0.2, 0.7, 0.3, 1.2;
//            std::cout << "got v" << std::endl;

            probability += leafStatistics.GetPdf().GetProbability(v);

//            std::cout << "got probabilitiy" << std::endl;


          }

          probability /= forest.TreeCount();
          std::cout << "prob" << probabilitiy << std::endl;

          index++;
        }
  }
} } }
