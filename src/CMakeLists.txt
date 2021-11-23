include(../cmake/latticezk_defaults.cmake)

set(EIGEN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/eigen)
set(GCEM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/gcem)
set(QCRYPTO_SOURCE_DIR ${CMAKE_SOURCE_DIR}/quasis-crypto)

set(LATTICEZK_SRC_FILES
	usampler.cpp
	gsampler.cpp
	facct.cpp
	prover.cpp
)

add_executable(prover
	prover.cpp
)

add_executable(facct
	facct.cpp
)

add_executable(gsampler
	gsampler.cpp
)

add_executable(usampler
	usampler.cpp
)

foreach(LATTICEZK_TARGET
	prover
	facct
	gsampler
	usampler
)
	target_include_directories(${LATTICEZK_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/include ${EIGEN_SOURCE_DIR} ${GCEM_SOURCE_DIR}/include ${QCRYPTO_SOURCE_DIR})
endforeach()

foreach(LATTICEZK_FILE
		${LATTICEZK_SRC_FILES}
)
	set_source_files_properties(${LATTICEZK_FILE} PROPERTIES COMPILE_FLAGS "${LATTICEZK_DEFAULT_FILE_COMPILE_FLAGS}")
endforeach()
