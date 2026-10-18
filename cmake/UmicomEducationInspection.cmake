#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEducationInspection.cmake
# PURPOSE: Give educational file inspectors one reusable, installable C23 owner.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include_guard(GLOBAL)
include(GNUInstallDirs)

get_filename_component(_umicom_inspection_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Apply the parent build's policies when available. The standalone education
# tool uses the same C23 and warning settings without requiring the whole SDK.
function(umicom_education_inspection_configure_target target)
    set_target_properties(${target} PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()

# These parsers depend only on the C runtime and public status definitions.
# Keep site serving, process launch, GTK and kernel execution outside this
# owner so an IDE or educational application can link only what it needs.
add_library(umicom_education_inspection STATIC
    "${_umicom_inspection_root}/src/education/binary_inspect.c"
    "${_umicom_inspection_root}/src/education/archive_inspect.c"
    "${_umicom_inspection_root}/src/education/elf_symbols.c"
    "${_umicom_inspection_root}/src/education/elf_load_plan.c"
    "${_umicom_inspection_root}/src/education/memory_audit.c"
    "${_umicom_inspection_root}/src/education/ipc_trace.c")
add_library(Umicom::education_inspection ALIAS umicom_education_inspection)
set_target_properties(umicom_education_inspection PROPERTIES
    EXPORT_NAME education_inspection)
target_include_directories(umicom_education_inspection PUBLIC
    "$<BUILD_INTERFACE:${_umicom_inspection_root}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
umicom_education_inspection_configure_target(umicom_education_inspection)

# Use the normal Framework export set. The main SDK supplies its package config;
# the standalone CLI can still build this same library without configuring GTK.
install(TARGETS umicom_education_inspection EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_education_inspection
)
install(FILES
    "${_umicom_inspection_root}/include/umicom/education/binary_inspect.h"
    "${_umicom_inspection_root}/include/umicom/education/archive_inspect.h"
    "${_umicom_inspection_root}/include/umicom/education/elf_symbols.h"
    "${_umicom_inspection_root}/include/umicom/education/elf_load_plan.h"
    "${_umicom_inspection_root}/include/umicom/education/memory_audit.h"
    "${_umicom_inspection_root}/include/umicom/education/ipc_trace.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/education" COMPONENT Framework)
install(FILES "${_umicom_inspection_root}/include/umicom/base/status.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/base" COMPONENT Framework)
install(DIRECTORY "${_umicom_inspection_root}/examples/education_inspection/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/education_inspection"
    COMPONENT Framework FILES_MATCHING PATTERN "*.c" PATTERN "CMakeLists.txt")
install(FILES "${_umicom_inspection_root}/docs/guides/education-inspection.md"
    "${_umicom_inspection_root}/docs/guides/saving-displayed-reports.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
    COMPONENT Framework)

# Scheduler and timer trace readers share this owner too. Linking them here
# gives the SDK and education command the same bounded classroom parsers.
target_sources(umicom_education_inspection PRIVATE
    "${_umicom_inspection_root}/src/education/schedule_trace.c"
    "${_umicom_inspection_root}/src/education/timer_trace.c")
install(FILES
    "${_umicom_inspection_root}/include/umicom/education/schedule_trace.h"
    "${_umicom_inspection_root}/include/umicom/education/timer_trace.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/education" COMPONENT Framework)

# Driver teaching traces belong to the same SDK as binary and scheduling
# inspectors. The standalone package includes the existing hash implementation;
# a full Framework build reuses its base owner to avoid duplicate definitions.
target_sources(umicom_education_inspection PRIVATE
    "${_umicom_inspection_root}/src/education/interrupt_trace.c"
    "${_umicom_inspection_root}/src/education/evidence_trace.c")
if(TARGET Umicom::base)
    target_link_libraries(umicom_education_inspection PUBLIC Umicom::base)
else()
    target_sources(umicom_education_inspection PRIVATE "${_umicom_inspection_root}/src/base/sha256.c")
endif()
install(FILES
    "${_umicom_inspection_root}/include/umicom/education/interrupt_trace.h"
    "${_umicom_inspection_root}/include/umicom/education/evidence_trace.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/education" COMPONENT Framework)

# Device request completion is another bounded teaching format. Keep it in the
# shared owner so Studio, the CLI and installed SDK use the same parser.
target_sources(umicom_education_inspection PRIVATE
    "${_umicom_inspection_root}/src/education/device_io_trace.c")
install(FILES "${_umicom_inspection_root}/include/umicom/education/device_io_trace.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/education" COMPONENT Framework)
