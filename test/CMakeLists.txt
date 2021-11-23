include(../cmake/latticezk_defaults.cmake)

set(EIGEN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/eigen)

set(LATTICEZK_TEST_FILES
       	latticezk_catch.cpp
	matrix_catch.cpp
)

add_executable(latticezk_catch
	catch_main.cpp
	${LATTICEZK_TEST_FILES}
)

foreach(LATTICEZK_FILE ${LATTICEZK_TEST_FILES})
	set_source_files_properties(${LATTICEZK_FILE} PROPERTIES COMPILE_FLAGS ${LATTICEZK_DEFAULT_FILE_COMPILE_FLAGS})
endforeach()

foreach(LATTICEZK_TARGET
	latticezk_catch
)
	target_include_directories(${LATTICEZK_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/include ${EIGEN_SOURCE_DIR})
endforeach()

find_package(Catch2 REQUIRED)
target_link_libraries(latticezk_catch PUBLIC Catch2::Catch2)
