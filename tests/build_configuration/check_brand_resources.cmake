#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build_configuration/check_brand_resources.cmake
#
# PURPOSE:
#   Check Ninja resource graph composition and automatic header regeneration.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(_required IN ITEMS ROOT DEST COMPILER ORDER)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing resource fixture argument: ${_required}")
    endif()
endforeach()
find_program(_ninja NAMES ninja ninja-build REQUIRED)

# Copy only the embedding inputs into a private fixture. Changing these files
# tests dependency tracking without touching the developer's canonical artwork.
set(_fixture "${DEST}/source framework")
set(_build "${DEST}/build")
foreach(_file IN ITEMS
        cmake/UmicomGtkBrandResources.cmake
        cmake/UmicomCreativeMediaResources.cmake
        adapters/gtk4/brand_resources.h.in
        adapters/gtk4/media_workspace_resources.h.in
        adapters/gtk4/media_workspace_style.c
        resources/brand/umicom-icon.svg
        resources/brand/umicom-icon-on-dark.svg
        resources/brand/umicom-logo.svg
        resources/brand/umicom-logo-on-dark.svg
        resources/styles/media-workspace.css)
    get_filename_component(_directory "${_fixture}/${_file}" DIRECTORY)
    file(MAKE_DIRECTORY "${_directory}")
    configure_file("${ROOT}/${_file}" "${_fixture}/${_file}" COPYONLY)
endforeach()

function(brand_step name)
    execute_process(COMMAND ${ARGN}
        RESULT_VARIABLE _result
        OUTPUT_FILE "${DEST}/${name}.log"
        ERROR_FILE "${DEST}/${name}-errors.log")
    if(NOT _result STREQUAL "0")
        message(FATAL_ERROR "Resource ${name} failed (${_result}); read ${DEST}/${name}-errors.log")
    endif()
endfunction()

brand_step(configure "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/build_configuration/brand_resources" -B "${_build}"
    -G Ninja "-DCMAKE_MAKE_PROGRAM=${_ninja}" "-DCMAKE_C_COMPILER=${COMPILER}"
    "-DRESOURCE_FRAMEWORK=${_fixture}" "-DORDER=${ORDER}")
# Ninja must parse the generated graph; a successful CMake generation alone
# did not detect duplicate output identities in the original failure.
brand_step(graph "${CMAKE_COMMAND}" --build "${_build}" --target brand-resource-graph)

set(_gtk_header "${_build}/generated/gtk-brand/umicom_gtk_brand_resources.h")
set(_media_header "${_build}/generated/creative-media/umicom_media_workspace_resources.h")
file(SHA256 "${_gtk_header}" _gtk_before)
file(SHA256 "${_media_header}" _media_before)

# Separate timestamps even on filesystems with coarse modification times.
brand_step(timestamp "${CMAKE_COMMAND}" -E sleep 1.1)
file(APPEND "${_fixture}/resources/brand/umicom-icon-on-dark.svg"
    "\n<!-- resource dependency regeneration probe -->\n")
brand_step(regenerate "${CMAKE_COMMAND}" --build "${_build}" --target brand-resource-graph)
file(SHA256 "${_gtk_header}" _gtk_after)
file(SHA256 "${_media_header}" _media_after)
if(_gtk_before STREQUAL _gtk_after OR _media_before STREQUAL _media_after)
    message(FATAL_ERROR "Changing shared SVG bytes must regenerate both embedding headers")
endif()

brand_step(style-timestamp "${CMAKE_COMMAND}" -E sleep 1.1)
file(APPEND "${_fixture}/resources/styles/media-workspace.css"
    "\n/* stylesheet dependency regeneration probe */\n")
brand_step(style-regenerate "${CMAKE_COMMAND}" --build "${_build}" --target brand-resource-graph)
file(SHA256 "${_gtk_header}" _gtk_style)
file(SHA256 "${_media_header}" _media_style)
if(NOT _gtk_after STREQUAL _gtk_style OR _media_after STREQUAL _media_style)
    message(FATAL_ERROR "A stylesheet edit must update Media without changing the shared SVG header")
endif()
