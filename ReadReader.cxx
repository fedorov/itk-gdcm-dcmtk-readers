// from https://itk.org/Wiki/ITK/Examples/DICOM/ResampleDICOM

#include "itkVersion.h"

#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"

#include "itkDCMTKImageIO.h"
#include "itkDCMTKSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"

#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"

#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
#include "itkShiftScaleImageFilter.h"
#endif

#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"

#include <itksys/SystemTools.hxx>

#if ITK_VERSION_MAJOR >= 4
#include "gdcmUIDGenerator.h"
#else
#include "gdcm/src/gdcmFile.h"
#include "gdcm/src/gdcmUtil.h"
#endif

#include <string>
#include <sstream>

static void CopyDictionary (itk::MetaDataDictionary &fromDict,
                     itk::MetaDataDictionary &toDict);


int main( int argc, char* argv[] )
{
  // Validate input parameters
  if( argc < 3 )
    {
    std::cerr << "Usage: "
              << argv[0]
              << " InputDicomDirectory OutputFile"
              << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int InputDimension = 3;

  typedef signed short PixelType;

  typedef itk::Image< PixelType, InputDimension >
    InputImageType;
  typedef itk::ImageSeriesReader< InputImageType >
    ReaderType;

  typedef itk::ImageFileWriter<InputImageType>
    WriterType;
  typedef itk::DCMTKImageIO
    ImageIOType;
  typedef itk::DCMTKSeriesFileNames
    InputNamesGeneratorType;
  typedef itk::NumericSeriesFileNames
    OutputNamesGeneratorType;
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
  typedef itk::ShiftScaleImageFilter< InputImageType, InputImageType >
    ShiftScaleType;
#endif

////////////////////////////////////////////////
// 1) Read the input series

  //ImageIOType::Pointer gdcmIO = ImageIOType::New();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory( argv[1] );

  const ReaderType::FileNamesContainer & filenames =
                            inputNames->GetInputFileNames();

  ReaderType::Pointer reader = ReaderType::New();

  //reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames );
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << "Exception thrown while reading the series" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(reader->GetOutput());
  writer->SetFileName(argv[2]);
  writer->Update();

  return 0;
}
