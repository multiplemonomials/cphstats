#AMBER buildfile for cphstats

cmake_minimum_required(VERSION 3.1)
project(cphstats NONE)

set(cphstats_MAJOR_VERSION 1)
set(cphstats_MINOR_VERSION 5)
set(cphstats_TWEAK_VERSION 0)

set(cphstats_VERSION "${cphstats_MAJOR_VERSION}.${cphstats_MINOR_VERSION}.${cphstats_TWEAK_VERSION}")

#---------------------------------------------------------------------------------------------------------------------------------------------------------------------

if(NOT INSIDE_AMBER)
	include(cmake/AmberBuildSystemInit.cmake)
	enable_language(CXX Fortran)
	include(AmberBuildSystem2ndInit)
	
	include(CompilerFlags)
	
	set(NEEDED_3RDPARTY_TOOLS zlib)
	set(LINAG_LIBRARIES_REQUIRED FALSE)
	set(REQUIRED_3RDPARTY_TOOLS "")
	include(3rdPartyTools)
	
	set(PACKAGE_NAME cphstats)
	set(PACKAGE_FILENAME cphstats)
	set(BUNDLE_IDENTIFIER org.ambermd.cphstats)
	set(BUNDLE_SIGNATURE CPHS)
	include(Packaging)
endif()

#---------------------------------------------------------------------------------------------------------------------------------------------------------------------

set(CPHSTATS_CXX_SOURCES main.cpp cpin.cpp
	string_manip.cpp cloptions.cpp test.cpp cpout.cpp 
	prottraj.cpp utilities.cpp conprob.cpp)

set(CPHSTATS_FORTRAN_SOURCES parse_cpin.F90)
#---------------------------------------------------------------------------------------------------------------------------------------------------------------------

#cphstats uses optimization
set_property(SOURCE ${CPHSTATS_CXX_SOURCES} PROPERTY COMPILE_FLAGS ${OPT_CXXFLAGS_SPC})
set_property(SOURCE ${CPHSTATS_FORTRAN_SOURCES} PROPERTY COMPILE_FLAGS ${OPT_FFLAGS_SPC})

add_executable(cphstats ${CPHSTATS_FORTRAN_SOURCES} ${CPHSTATS_CXX_SOURCES})
add_executable(cestats ${CPHSTATS_FORTRAN_SOURCES} ${CPHSTATS_CXX_SOURCES})

# cphstats and cestats are built from the same source, but with different defines
target_compile_definitions(cphstats PUBLIC PH)
target_compile_definitions(cestats PUBLIC REDOX)


if(zlib_ENABLED)
	add_definitions(-DHASGZ)
	targets_link_libraries(cphstats cestats LIBRARIES zlib)
endif()

install(TARGETS cphstats cestats DESTINATION ${BINDIR})

if(NOT INSIDE_AMBER)
	print_build_report()
endif()
