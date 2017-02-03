#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>

#include "Platform.h"

#include "Graphics.h"
#include "dibCodec.h"

#include "Sherwood.h"

#include "CommandLineParser.h"
#include "DataPointCollection.h"

#include "StatisticsAggregators.h"
#include "RegressionGaussianNd.h"

using namespace MicrosoftResearch::Cambridge::Sherwood;

void DisplayHelp();

void DisplayTextFiles(const std::string& relativePath);

std::auto_ptr<DataPointCollection> LoadTrainingData(
  const std::string& filename,
  const std::string& alternativePath,
  int dimension,
  int targetDimension,
  DataDescriptor::e descriptor);

// Store (Linux-friendly) relative paths to training data
const std::string CLAS_DATA_PATH = "/data/supervised classification";
const std::string SSCLAS_DATA_PATH = "/data/semi-supervised classification";
const std::string REGRESSION_DATA_PATH = "/data/regression";
const std::string DENSITY_DATA_PATH = "/data/density estimation";

int main(int argc, char* argv[])
{
  if(argc<2 || std::string(argv[1])=="/?" || toLower(argv[1])=="help")
  {
    DisplayHelp();
    return 0;
  }

  std::string mode = toLower(argv[1]); //  argv[0] is name of exe, argv[1] defines command line mode

  // These command line parameters are reused over several command line modes...
  StringParameter trainingDataPath("path", "Path of file containing training data.");
  StringParameter forestOutputPath("forest", "Path of file containing forest.");
  StringParameter forestPath("forest", "Path of file containing forest.");
  StringParameter testDataPath("data", "Path of file containing test data.");
  StringParameter outputPath("output", "Path of file containing output.");
  NaturalParameter T("t", "No. of trees in the forest (default = {0}).", 10);
  NaturalParameter D("d", "Maximum tree levels (default = {0}).", 10, 20);
  NaturalParameter F("f", "No. of candidate feature response functions per split node (default = {0}).", 10);
  NaturalParameter L("l", "No. of candidate thresholds per feature response function (default = {0}).", 1);
  SingleParameter a("a", "The number of 'effective' prior observations (default = {0}).", true, false, 10.0f);
  SingleParameter b("b", "The variance of the effective observations (default = {0}).", true, true, 400.0f);
  SimpleSwitchParameter verboseSwitch("Enables verbose progress indication.");
  SingleParameter plotPaddingX("padx", "Pad plot horizontally (default = {0}).", true, false, 0.1f);
  SingleParameter plotPaddingY("pady", "Pad plot vertically (default = {0}).", true, false, 0.1f);

  EnumParameter split(
    "s",
    "Specify what kind of split function to use (default = {0}).",
    "axis;linear",
    "axis-aligned split;linear split",
    "axis");

  // Behaviour depends on command line mode...

  if (mode == "regression")
  {
    // Regression
    CommandLineParser parser;
    int i = 0;
    parser.SetCommand("SW " + toUpper(mode));

    parser.AddArgument(trainingDataPath);
    parser.AddSwitch("T", T);
    parser.AddSwitch("D", D);
    parser.AddSwitch("F", F);
    parser.AddSwitch("L", L);

    parser.AddSwitch("PADX", plotPaddingX);
    parser.AddSwitch("PADY",  plotPaddingY);
    parser.AddSwitch("VERBOSE", verboseSwitch);

    // Override defaults
    T.Value = 10;
    D.Value = 5;
    a.Value = 0;
    b.Value = 900;

    if (argc == 2)
    {
      parser.PrintHelp();
      DisplayTextFiles(REGRESSION_DATA_PATH);
      return 0;
    }

    if (parser.Parse(argc, argv, 2) == false)
      return 0;

    TrainingParameters parameters;
    parameters.MaxDecisionLevels = D.Value-1;
    parameters.NumberOfCandidateFeatures = F.Value;
    parameters.NumberOfCandidateThresholdsPerFeature = L.Value;
    parameters.NumberOfTrees = T.Value;
    parameters.Verbose = verboseSwitch.Used();

    // Load training data for a 2D density estimation problem.
    std::auto_ptr<DataPointCollection> trainingData = std::auto_ptr<DataPointCollection>(LoadTrainingData(
      trainingDataPath.Value,
      REGRESSION_DATA_PATH + "/" + trainingDataPath.Value,
      16,
	  6,
      DataDescriptor::HasTargetValues ) );

    if (trainingData.get()==0)
      return 0; // LoadTrainingData() generates its own progress/error messages

    std::auto_ptr<Forest<AxisAlignedFeatureResponse, GaussianAggregatorNd> > forest = RegressionGaussianExampleNd::Train(
      *trainingData.get(), parameters);

//	std::auto_ptr<Forest<AxisAlignedFeatureResponse, LinearFitAggregator1d> > forest = RegressionExample::Train(
//	  *trainingData.get(), parameters);

    PointF plotDilation(plotPaddingX.Value, plotPaddingY.Value);
    std::auto_ptr<Bitmap<PixelBgr> > result = RegressionGaussianExampleNd::Visualize(*forest.get(), *trainingData.get(), Size(300,300), plotDilation);
//	std::auto_ptr<Bitmap<PixelBgr> > result = RegressionExample::Visualize(*forest.get(), *trainingData.get(), Size(300,300), plotDilation);

    std::cout << "\nSaving output image to result.dib" << std::endl;
    result->Save("result.dib");
  }
  else
  {
    std::cout << "Unrecognized command line argument, try SW HELP." << std::endl;
    return 0;
  }

  return 0;
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

void DisplayHelp()
{
  // Create a dummy command line parser so we can display command line
  // help in the usual format.
  EnumParameter mode(
    "mode",
    "Select mode of operation.",
    "clas;density;regression;ssclas",
    "Supervised 2D classfication;2D density estimation;1D to 1D regression;Semi-supervised 2D classification");

  StringParameter args("args...", "Other mode-specific arguments");

  CommandLineParser parser;
  parser.SetCommand("SW");
  parser.AddArgument(mode);
  parser.AddArgument(args);

  std::cout << "Sherwood decision forest library demos." << std::endl << std::endl;
  parser.PrintHelp();

  std::cout << "To get more help on a particular mode of operation, omit the arguments, e.g.\nsw density" << std::endl;
}
