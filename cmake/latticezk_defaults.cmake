if(NOT DEFINED LATTICEZK_MACHINE_COMPILE_FLAGS)
	set(LATTICEZK_MACHINE_COMPILE_FLAGS "-march=native -msse -msse2 -maes -mfma -O3")
endif()

set(LATTICEZK_DEFAULT_FILE_COMPILE_FLAGS "${LATTICEZK_MACHINE_COMPILE_FLAGS}")

if(LATTICEZK_ENABLE_OPENMP)
        find_package(OpenMP REQUIRED)
        if(OpenMP_FOUND)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -fopenmp -fopenmp-simd")
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp -fopenmp-simd")
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        endif()
endif()
