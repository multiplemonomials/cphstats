#AMBER buildfile for cphstats
project(cphstats CXX Fortran)

set(CPHSTATS_CXX_SOURCES main.cpp cpin.cpp
	string_manip.cpp cloptions.cpp test.cpp cpout.cpp 
	prottraj.cpp utilities.cpp conprob.cpp)

set(CPHSTATS_FORTRAN_SOURCES parse_cpin.F90)
#---------------------------------------------------------------------------------------------------------------------------------------------------------------------

#cphstats uses optimization
set_property(SOURCE ${CPHSTATS_CXX_SOURCES} PROPERTY COMPILE_OPTIONS ${OPT_CXXFLAGS})
set_property(SOURCE ${CPHSTATS_FORTRAN_SOURCES} PROPERTY COMPILE_OPTIONS ${OPT_FFLAGS})

add_executable(cphstats ${CPHSTATS_FORTRAN_SOURCES} ${CPHSTATS_CXX_SOURCES})
add_executable(cestats ${CPHSTATS_FORTRAN_SOURCES} ${CPHSTATS_CXX_SOURCES})

# cphstats and cestats are built from the same source, but with different defines
target_compile_definitions(cphstats PUBLIC PH)
target_compile_definitions(cphstats PUBLIC REDOX)


if(zlib_ENABLED)
	add_definitions(-DHASGZ)
	targets_link_libraries(cphstats cestats LIBRARIES zlib)
endif()

install(TARGETS cphstats cestats DESTINATION ${BINDIR})