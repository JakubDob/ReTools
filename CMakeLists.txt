cmake_minimum_required(VERSION 3.22.1)

project(WinRE)

add_library(REutils STATIC)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(ConsoleClient)

target_compile_features(REutils PRIVATE cxx_std_20)
target_compile_options(REutils PRIVATE -DUNICODE -D_UNICODE)

add_dependencies(REutils detours Zydis)

set (DETOURS_PATH ${CMAKE_BINARY_DIR}/detours/src/detours)

if(${CMAKE_GENERATOR_PLATFORM} STREQUAL "x64")
	set(DETOURS_TARGET_PROCESSOR X64)
	target_link_libraries(REutils PRIVATE ${DETOURS_PATH}/lib.X64/detours.lib)
elseif(${CMAKE_GENERATOR_PLATFORM} STREQUAL "Win32")
	set(DETOURS_TARGET_PROCESSOR X86)
	target_link_libraries(REutils PRIVATE ${DETOURS_PATH}/lib.X86/detours.lib)
else()
message(STATUS "Generator platform: " ${CMAKE_GENERATOR_PLATFORM})
	message(FATAL_ERROR "Unknown generator platform")
endif()

#Detours does not use CMake
include(ExternalProject)
ExternalProject_Add(
	detours
	GIT_REPOSITORY "https://github.com/microsoft/Detours.git"
	PREFIX ${CMAKE_BINARY_DIR}/detours
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	SOURCE_SUBDIR src #do not build examples
	BUILD_COMMAND ${CMAKE_COMMAND} -E env DETOURS_TARGET_PROCESSOR=${DETOURS_TARGET_PROCESSOR} nmake
	INSTALL_COMMAND ""
)

#Zydis uses CMake
include(FetchContent)
FetchContent_Declare(
	Zydis
	GIT_REPOSITORY https://github.com/zyantific/zydis.git
	GIT_TAG v3.2.1
)
FetchContent_MakeAvailable(Zydis)
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)

target_link_libraries(REutils PRIVATE Zydis)

include(GNUInstallDirs)

target_include_directories(
	REutils
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

	$<BUILD_INTERFACE:${DETOURS_PATH}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/REutils/detours/include>
)

install(
	TARGETS REutils
	EXPORT REutilsTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_DIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	EXPORT REutilsTargets
	FILE REutilsTargets.cmake
	NAMESPACE REutils::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/REutils
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/REutilsConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/REutils
)

install(
	FILES
	${CMAKE_CURRENT_BINARY_DIR}/REutilsConfig.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/REutils
)

install(
	DIRECTORY ${DETOURS_PATH}/include
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/REutils/detours
)