// Resample a DICOM study
//   Usage: ResampleDICOM InputDirectory OutputDirectory
//                        xSpacing ySpacing zSpacing
//
//   Example: ResampleDICOM CT CTResample 0 0 1.5
//            will read a series from the CT directory and create a
//            new series in the CTResample directory. The new series
//            will have the same x,y spacing as the input series, but
//            will have a z-spacing of 1.5.
//
// Description:
// ResampleDICOM resamples a DICOM series with user-specified
// spacing. The program outputs a new DICOM series with a series
// number set to 1001. All non-private DICOM tags are moved from the input
// series to the output series. The Image Position Patient is adjusted
// for each slice to reflect the z-spacing. The number of slices in
// the output series may be larger or smaller due to changes in the
// z-spacing. To retain the spacing for a given dimension, specify 0.
//
// The program progresses as follows:
// 1) Read the input series
// 2) Resample the series according to the user specified x-y-z
//    spacing.
// 3) Create a MetaDataDictionary for each slice.
// 4) Shift data to undo the effect of a rescale intercept by the
//    DICOM reader (only for ITK < 4.6)
// 5) Write the new DICOM series
//

// from https://itk.org/Wiki/ITK/Examples/DICOM/ResampleDICOM

#include "itkVersion.h"

#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"

#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
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
  typedef itk::GDCMImageIO
    ImageIOType;
  typedef itk::GDCMSeriesFileNames
    InputNamesGeneratorType;
  typedef itk::NumericSeriesFileNames
    OutputNamesGeneratorType;
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
  typedef itk::ShiftScaleImageFilter< InputImageType, InputImageType >
    ShiftScaleType;
#endif

////////////////////////////////////////////////
// 1) Read the input series

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory( argv[1] );

  const ReaderType::FileNamesContainer & filenames =
                            inputNames->GetInputFileNames();

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetImageIO( gdcmIO );
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
