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
#include <fstream>
#include <nifti1_io.h>

#include "PlotCanvas.h"
#include "Graphics.h"

#include "Sherwood.h"

#include "DataPointCollection.h"
#include "FeatureResponseFunctions.h"
#include "GaussianAggregator.h"
#include "RegressionGaussianNd.h"

namespace MicrosoftResearch { namespace Cambridge { namespace Sherwood
{
  class RegressionGaussianNdTrainingContext : public ITrainingContext<BoxOffsetFeatureResponse, GaussianAggregatorNd>
  {
  public:
    // Implementation of ITrainingContext
	  BoxOffsetFeatureResponse GetRandomFeature(Random& random)
    {
      return BoxOffsetFeatureResponse::CreateRandom(random);
    }

    GaussianAggregatorNd GetStatisticsAggregator()
    {
      return GaussianAggregatorNd(4, 6, 100, 100);
    }

    double ComputeInformationGain(const GaussianAggregatorNd& allStatistics, const GaussianAggregatorNd& leftStatistics, const GaussianAggregatorNd& rightStatistics)
    {
    	std::cout << "computing information gain" << std::endl;
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

    static std::auto_ptr<Forest<BoxOffsetFeatureResponse, GaussianAggregatorNd> > Train(
      const DataPointCollection& trainingData,
      const TrainingParameters& parameters)
    {
      std::cout << "Training the forest..." << std::endl;

      Random random;

      RegressionGaussianNdTrainingContext regressionTrainingContext;

      std::auto_ptr<Forest<BoxOffsetFeatureResponse, GaussianAggregatorNd> > forest
        = ForestTrainer<BoxOffsetFeatureResponse, GaussianAggregatorNd>::TrainForest(
        random, parameters, regressionTrainingContext, trainingData);
      std::cout << "finished training" << std::endl;
      return forest;
    }

    static void setValueAtIndex(int i, int j, int k, double value, nifti_image * nim, double * data) {
	  int dimx = nim -> nx;
	  int dimy = nim -> ny;
	  int dimz = nim -> nz;
	  data[i + dimx*j + dimx*dimy*k] = value;
    }


    static void Visualize(
      Forest<BoxOffsetFeatureResponse, GaussianAggregatorNd>& forest,
      const DataPointCollection& trainingData,
	  const DataPointCollection& testingData,
	  int patNum,
	  std::string filename)
    {

    	double hist[6][400] = {{0}};

		std::vector<std::vector<int> > leafNodeIndices;
		forest.Apply(testingData, leafNodeIndices);

		int dims[10][6] = {{340,340,340,340,133,133},
						   {320, 320, 320, 320, 129, 129},
						   {340, 340, 340, 340, 178, 178},
						   {440, 440, 440, 440, 184, 184},
						   {270, 270, 270, 270, 91, 91},
						   {270, 270, 270, 270, 117, 117},
						   {386, 386, 386, 386, 113, 113},
						   {386, 386, 386, 386, 150, 150},
						   {340, 340, 340, 340, 125, 125},
						   {340, 340, 340, 340, 160, 160}};
	    std::string read_filename = "../../dataset_full/training_axial_full_resampled_pat" + std::to_string(patNum) + ".nii.gz";
		nifti_image * nim = nifti_image_read(read_filename.c_str(), 1);
	    std::string write_filename = "../../dataset_full/test" + std::to_string(patNum) + ".nii.gz";
		nim -> fname = &write_filename[0];
		double * stuff = new double[dims[patNum][0]*dims[patNum][2]*dims[patNum][4]];

		for (int i = 0; i< testingData.Count(); i++) {
		    const float* datum = testingData.GetDataPoint(i);
			if (i%1000 == 0) {
				std::cout << "i = " << i << std::endl;
				std::cout << leafNodeIndices[0][i] << std::endl;
			}
			RegressionGaussianExampleNd::setValueAtIndex(int(datum[1]), int(datum[2]), int(datum[3]), leafNodeIndices[0][i], nim, stuff);
			// reorganize for loops
			// get only one std away from mean
			for (int dim = 0; dim < 6; dim ++) {
				for (int j = 0; j < dims[patNum][dim]; j++) {
					for (int t = 0; t < forest.TreeCount(); t++) {
						Node<BoxOffsetFeatureResponse, GaussianAggregatorNd> leafNodeCopy = forest.GetTree((t)).GetNode(leafNodeIndices[t][i]);
						const GaussianAggregatorNd& leafStatistics = leafNodeCopy.TrainingDataStatistics;
						// compute probability first and then shift
						hist[dim][j] += leafStatistics.GetPdf().GetMarginalProbability(dim, j-datum[dim/2+1]);
					}
				}
			}
		}
		nim -> data = stuff;
		std::cout << "set" << std::endl;
		nifti_image_write(nim);
//		Gnuplot gp;
//		gp.send2d(test);
//		gp.flush();
		std::ofstream myfile;
		myfile.open(filename);
		double maxProb[6];
		int maxInd[6];
		for (int dim = 0; dim < 6; dim ++) {
			for (int j = 0; j < dims[patNum][dim]; j++) {
				myfile << hist[dim][j] << " ";
				if (hist[dim][j] > maxProb[dim]) {
					maxProb[dim] = hist[dim][j];
					maxInd[dim] = j;
				}
			}
			myfile << "\n";
			std::cout << dim << " " << maxProb[dim] << " " << maxInd[dim] << std::endl;
		}

		myfile.close();
/*
		std::ofstream myfile;
		myfile.open(filename);
		myfile << "hi";
		myfile.close();
*/
	// Generate some test samples in a grid pattern (a useful basis for creating visualization images)
//		  PlotCanvas plotCanvas(trainingData.GetRange(0), trainingData.GetTargetRange(), PlotSize, PlotDilation);
//
//		  std::auto_ptr<DataPointCollection> testData = DataPointCollection::Generate1dGrid(plotCanvas.plotRangeX, PlotSize.Width);
//
//		  std::cout << "\nApplying the forest to test data..." << std::endl;
//
//		  std::vector<std::vector<int> > leafNodeIndices;
//		  forest.Apply(*testData.get(), leafNodeIndices);
//		  std::cout << "graphing" << std::endl;
//		  // Generate visualization image
//		  std::auto_ptr<Bitmap<PixelBgr> > result = std::auto_ptr<Bitmap<PixelBgr> >(new Bitmap<PixelBgr> (PlotSize.Width, PlotSize.Height));
//
//		  // Plot the learned density
//		  PixelBgr inverseDensityColor = PixelBgr::FromArgb(255 - DensityColor.R, 255 - DensityColor.G, 255 - DensityColor.B);
//
//		  std::vector<double> mean_y_given_x(PlotSize.Width);
//
//		  int index = 0;
//		  for (int i = 0; i < PlotSize.Width; i++)
//		  {
//			double totalProbability = 0.0;
//			for (int j = 0; j < PlotSize.Height; j++)
//			{
//			  // Map pixel coordinate (i,j) in visualization image back to point in input space
//			  float x = plotCanvas.plotRangeX.first + i * plotCanvas.stepX;
//			  float y = plotCanvas.plotRangeY.first + j * plotCanvas.stepY;
//
//			  double probability = 0.0;
//
//			  // Aggregate statistics for this sample over all trees
//			  for (int t = 0; t < forest.TreeCount(); t++)
//			  {
//				Node<BoxOffsetFeatureResponse, GaussianAggregatorNd> leafNodeCopy = forest.GetTree((t)).GetNode(leafNodeIndices[t][i]);
//
//				const GaussianAggregatorNd& leafStatistics = leafNodeCopy.TrainingDataStatistics;
//
//				Eigen::VectorXd v(6);
//				v << 0, 0, 0, 0, 0, 0; // will fix sometime later
//
//				probability += leafStatistics.GetPdf().GetProbability(v);
//
//			  }
//
//			  probability /= forest.TreeCount();
//
//			  mean_y_given_x[i] += probability * y;
//			  totalProbability += probability;
//
//			  float scale = 10.0f * (float)probability;
//
//			  PixelBgr weightedColor = PixelBgr::FromArgb(
//				(unsigned char)(std::min(scale * inverseDensityColor.R + 0.5f, 255.0f)),
//				(unsigned char)(std::min(scale * inverseDensityColor.G + 0.5f, 255.0f)),
//				(unsigned char)(std::min(scale * inverseDensityColor.B + 0.5f, 255.0f)));
//
//			  PixelBgr c = PixelBgr::FromArgb(255 - weightedColor.R, 255 - weightedColor.G, 255 - weightedColor.G);
//
//			  result->SetPixel(i, j, c);
//
//			  index++;
//			}
//
//			// NB We don't really compute the mean over y, just over the region of y that is plotted
//			mean_y_given_x[i] /= totalProbability;
//		  }
//
//		  std::cout << "here 1" << std::cout;
//
//		  // Also plot the mean curve and the original training data
//		  Graphics<PixelBgr> g(result->GetBuffer(), result->GetWidth(), result->GetHeight(), result->GetStride());
//
//		  for (int i = 0; i < PlotSize.Width-1; i++)
//		  {
//			g.DrawLine(
//			  MeanColor,
//			  (float)(i),
//			  (float)((mean_y_given_x[i] - plotCanvas.plotRangeY.first)/plotCanvas.stepY),
//			  (float)(i+1),
//			  (float)((mean_y_given_x[i+1] - plotCanvas.plotRangeY.first)/plotCanvas.stepY));
//		  }
//
//		  for (unsigned int s = 0; s < trainingData.Count(); s++)
//		  {
//			// Map sample coordinate back to a pixel coordinate in the visualization image
//			PointF x(
//			  (trainingData.GetDataPoint(s)[0] - plotCanvas.plotRangeX.first) / plotCanvas.stepX,
//			  (trainingData.GetTarget(s)[0] - plotCanvas.plotRangeY.first) / plotCanvas.stepY);
//
//			RectangleF rectangle(x.X - 2.0f, x.Y - 2.0f, 4.0f, 4.0f);
//			g.FillRectangle(DataPointColor, rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height);
//			g.DrawRectangle(DataPointBorderColor, rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height);
//		  }
//		  std::cout << "print result" << std::endl;
//		  return result;
    }
  };
} } }
