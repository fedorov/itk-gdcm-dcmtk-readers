cmake_minimum_required(VERSION 2.8)

project(gdcmdcmtk)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif()

add_executable(ReadGDCM ReadGDCM.cxx)
target_link_libraries(ReadGDCM ${ITK_LIBRARIES})

add_executable(ReadDCMTK ReadDCMTK.cxx)
target_link_libraries(ReadDCMTK ${ITK_LIBRARIES})
