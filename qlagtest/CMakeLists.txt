cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}" )

PROJECT(lagtest)

# This needs to be set to release in order to use qwt library
# If debug, qwtd has to be linked
SET(CMAKE_BUILD_TYPE Release )
#SET(CMAKE_BUILD_TYPE Debug )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    message( "=== Detected Windows build ===")

    # WINDOWS: Choose dynamic or static qt libraries
    # set( QT5_DYNAMIC_INST "C:/Projects/QT/5.0.2/msvc2012_64/lib" )
    set( QT5_DYNAMIC_INST "C:/Projects/qt5_msvc2010/5.0.2/msvc2010_opengl" )
    set( QT5_STATIC_INST "C:/Users/pasieka/Projects/qt5/qtbase" )
    set( QT5_INST ${QT5_DYNAMIC_INST} )

    # WINDOWS: set path to cmake find modules
    set(CMAKE_PREFIX_PATH ${QT5_INST} )
    set(CMAKE_MODULE_PATH "${QT5_INST}/cmake;${CMAKE_MODULE_PATH}" )
    set(CMAKE_MODULE_PATH "C:/Projects/eigen3/cmake;${CMAKE_MODULE_PATH}" )

    # WINDOWS: cant use Eigen3 cmake search script, specify path directly.
    set(EIGEN3_INCLUDE_DIR "C:/Projects/eigen3")

    # qwt plotting library
    set( QWT_INC_PATH "C:/Projects/qwt/src")
    find_library( QWT_LIBRARY NAMES qwt PATHS "C:/Projects/qwt/lib" )
    #add_definitions( -DWINVER=0x0501 )

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message( "=== Detected Linux build ===")

    # Define QT5 installation
    set( QT5_INST "/opt/Qt5.0.2/5.0.2/gcc_64" )

    #set path to cmake find modules
    set(CMAKE_PREFIX_PATH ${QT5_INST} )
    set(CMAKE_MODULE_PATH "${QT5_INST}/cmake;${CMAKE_MODULE_PATH}" )

    find_package(Eigen3 REQUIRED )
    set( QWT_INC_PATH "/usr/include/qwt")
    find_library( QWT_LIBRARY NAMES qwt PATHS "/usr/lib" )


ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


message( "\nCMAKE_MODULE_PATH = " ${CMAKE_MODULE_PATH} )
message( "\nCMAKE_PREFIX_PATH = " ${CMAKE_PREFIX_PATH} )


# Instruct CMake to run moc automatically when needed.
set( CMAKE_AUTOMOC ON ) # http://blogs.kde.org/2011/11/01/cool-new-stuff-cmake-286-automoc
#set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static ")

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5Network REQUIRED)


# Needed define by Qwt
add_definitions(-DQWT_DLL)

set( CMAKE_VERBOSE_MAKEFILE on )

#set the default path for built executables to the "bin" directory
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )
set( CMAKE_FIND_LIBRARY_SUFFIXES ".lib" )
#enable_testing()
#find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
#link_directories ( ${Boost_LIBRARY_DIRS} )

# ============ Main Source files ============== #

set (RRJ_SOURCES    "src/timemodel.cpp"                                        		    
                    "src/rs232.cpp"                    
                    "src/serialporthandler.cpp"
                    "src/latencymodel.cpp"
                    "src/window.cpp"
                    "src/lagtest.cpp"
                    "src/main.cpp"
                                        )
set (PRJ_INCLUDE "src" )

add_executable( lagtest WIN32 ${RRJ_SOURCES} )
include_directories(${PRJ_INCLUDE} ${OPENGL_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} ${QWT_INC_PATH} )

target_link_libraries(lagtest ${OPENGL_LIBRARIES} ${QWT_LIBRARY}  Qt5::WinMain )
qt5_use_modules(lagtest Widgets OpenGL Network)

# ============ Test files ============== #

#rosbuild_add_executable(IPPImage_Test src/IMPFramework/Test/IPPImage_Test.cpp src/IMPFramework/IPPImage.cpp )
#target_link_libraries(IPPImage_Test ${Boost_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${IPP_LIBRARIES} )
#set_target_properties( IPPImage_Test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/Test )
#add_test(IPPImage bin/Test/IPPImage_Test)
#message("Run test with \n    ctest . --output-on-failure")




