cmake_minimum_required(VERSION 3.1)

project(polatory CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Boost 1.65.1 REQUIRED)
find_package(Eigen3 REQUIRED)

option(COVERAGE "Compile with --coverage option" OFF)

if(UNIX)
    set(MKL_ROOT /opt/intel/mkl)

    if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|GNU)$")
        set(CMAKE_CXX_FLAGS "-std=c++14 -fopenmp")
        set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
        if(COVERAGE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        endif()
    else()
        message(FATAL_ERROR "Not supported compiler.")
    endif()
elseif(MSVC)
    set(_PF86 "ProgramFiles(x86)")
    file(TO_CMAKE_PATH "$ENV{${_PF86}}" _PROG_FILES_X86)

    set(MKL_ROOT "${_PROG_FILES_X86}/IntelSWTools/compilers_and_libraries/windows/mkl")
    set(MKL_DLL_DIR "${_PROG_FILES_X86}/IntelSWTools/compilers_and_libraries/windows/redist/intel64/mkl")
    set(VCPKG_ROOT "" CACHE STRING "Path to vcpkg/installed/x64-windows")
    set(DLLS
        ${MKL_DLL_DIR}/mkl_avx.dll
        ${MKL_DLL_DIR}/mkl_avx2.dll
        ${MKL_DLL_DIR}/mkl_avx512.dll
        ${MKL_DLL_DIR}/mkl_core.dll
        ${MKL_DLL_DIR}/mkl_def.dll
        ${MKL_DLL_DIR}/mkl_mc.dll
        ${MKL_DLL_DIR}/mkl_mc3.dll
        ${MKL_DLL_DIR}/mkl_sequential.dll
    )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "/MP /EHsc /std:c++14 /openmp")
        set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG /GL")
    else()
        message(FATAL_ERROR "Not supported compiler.")
    endif()
else()
    message(FATAL_ERROR "Not supported system.")
endif()

add_definitions(
    -DEIGEN_DONT_PARALLELIZE
    -DEIGEN_MPL2_ONLY
    -DEIGEN_USE_MKL_ALL
    -DPOLATORY_FTZ
)

include_directories(
    include
    ${Boost_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR}
    ${MKL_ROOT}/include
    ${VCPKG_ROOT}/include
)

if(UNIX)
    link_libraries(
        "-Wl,--start-group"
        ${MKL_ROOT}/lib/intel64/libmkl_intel_lp64.a ${MKL_ROOT}/lib/intel64/libmkl_sequential.a ${MKL_ROOT}/lib/intel64/libmkl_core.a pthread m dl
        ceres flann_cpp glog
        "-Wl,--end-group"
    )
elseif(MSVC)
    include_directories(
        ${MKL_ROOT}/include
    )
    link_libraries(
        ${MKL_ROOT}/lib/intel64/mkl_intel_lp64_dll.lib ${MKL_ROOT}/lib/intel64/mkl_sequential_dll.lib ${MKL_ROOT}/lib/intel64/mkl_core_dll.lib
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_libraries(
            ${VCPKG_ROOT}/debug/lib/ceres-debug.lib
            ${VCPKG_ROOT}/debug/lib/flann_cpp-gd.lib
            ${VCPKG_ROOT}/debug/lib/glog.lib
        )
    else()
        link_libraries(
            ${VCPKG_ROOT}/lib/ceres.lib
            ${VCPKG_ROOT}/lib/flann_cpp.lib
            ${VCPKG_ROOT}/lib/glog.lib
        )
    endif()
endif()

add_subdirectory(benchmark)
add_subdirectory(examples)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
