cmake_minimum_required(VERSION 2.8)

project("BTrees Perf test")

#Set the version number
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

#Activate warnings
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -fpermissive -D_REENTRANT -fno-strict-aliasing -funroll-loops -pthread -std=c++0x -pedantic -Wall -Wextra -Wno-long-long")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -march=native -funroll-loops -pthread -std=c++0x -pedantic -Wall -Wextra -Wno-long-long")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -std=c++0x -pedantic -Wall -Wextra -Wno-long-long")
endif()

set(EXECUTABLE_OUTPUT_PATH bin)

include_directories(include)

file(
	GLOB_RECURSE
	btrees_memory_source_files
	src/*.cpp
)

file(
	GLOB_RECURSE
	btrees_source_files
	src/test.cpp
	src/main.cpp
	src/HazardManager.cpp
)

file(
     	GLOB_RECURSE
        pcm_source_files
	../common/pcmpower.c
        ../intelpcm/lib/pci.o
	../intelpcm/lib/cpucounters.o
	../intelpcm/lib/msr.o
	../intelpcm/lib/client_bw.o
)

file(GLOB to_remove src/genzipf.cpp)
list(REMOVE_ITEM btrees_memory_source_files ${to_remove})
list(REMOVE_ITEM btrees_source_files ${to_remove})

file(GLOB to_remove src/memory.cpp)
list(REMOVE_ITEM btrees_source_files ${to_remove})

file(GLOB to_remove src/main.cpp)
list(REMOVE_ITEM btrees_memory_source_files ${to_remove})

set_source_files_properties(${pcm_source_files} PROPERTIES LANGUAGE CXX)

add_executable(btrees ${btrees_source_files})
add_executable(btrees.pcm ${btrees_source_files} ${pcm_source_files})
SET_TARGET_PROPERTIES(btrees.pcm PROPERTIES COMPILE_FLAGS "-D__USEPCM -I../intelpcm/include")


#add_executable(memory ${btrees_memory_source_files})
#add_executable(gen_zip src/genzipf.cpp)

target_link_libraries(btrees m)
#target_link_libraries(gen_zip m)
