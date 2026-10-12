# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomInstalledEducationInspectionConsumer LANGUAGES C)
include(CTest)

# Require the copied install prefix explicitly. Finding a global package or the
# source-tree build would hide a broken installation and make this check pass
# for the wrong SDK.
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS education_inspection
    PATHS "${INSPECTION_PREFIX}" NO_DEFAULT_PATH)
file(REAL_PATH "${INSPECTION_PREFIX}" _prefix)
get_target_property(_imported Umicom::education_inspection IMPORTED)
if(NOT _imported)
    message(FATAL_ERROR "The consumer must use the installed inspection library")
endif()
get_target_property(_includes Umicom::education_inspection INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(_configs Umicom::education_inspection IMPORTED_CONFIGURATIONS)
set(_installed_paths ${_includes})
foreach(_config IN LISTS _configs)
    get_target_property(_location Umicom::education_inspection "IMPORTED_LOCATION_${_config}")
    list(APPEND _installed_paths "${_location}")
endforeach()
# All headers and library locations must resolve within the copied prefix.
# This catches absolute build/source paths even while those originals exist.
foreach(_path IN LISTS _installed_paths)
    if(NOT EXISTS "${_path}")
        message(FATAL_ERROR "Missing installed inspection file: ${_path}")
    endif()
    file(REAL_PATH "${_path}" _resolved)
    cmake_path(IS_PREFIX _prefix "${_resolved}" NORMALIZE _inside)
    if(NOT _inside)
        message(FATAL_ERROR "Inspection consumer escaped its install prefix: ${_path}")
    endif()
endforeach()

# Build the installed educational example, never the repository's copy.
add_subdirectory("${INSPECTION_PREFIX}/share/umicom-framework/examples/education_inspection"
    "${CMAKE_CURRENT_BINARY_DIR}/example")
add_executable(inspection-sdk-link ../test_inspection_sdk_link.c)
target_link_libraries(inspection-sdk-link PRIVATE Umicom::education_inspection)
set_target_properties(inspection-sdk-link PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(inspection-sdk-link PRIVATE /W4)
else()
    target_compile_options(inspection-sdk-link PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
add_test(NAME inspection.sdk.link COMMAND inspection-sdk-link)
add_test(NAME inspection.sdk.example
    COMMAND umicom-education-inspect-example binary "$<TARGET_FILE:inspection-sdk-link>")
set_tests_properties(inspection.sdk.example PROPERTIES PASS_REGULAR_EXPRESSION "Format: (ELF|PE)")
