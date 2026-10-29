#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/windows_deployment/prepare_startup_test.cmake
#
# PURPOSE:
#   Stage private resource fixtures for the real Windows startup test.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
get_filename_component(_name "${ROOT}" NAME)
if(_name STREQUAL "bin")
    get_filename_component(ROOT "${ROOT}" DIRECTORY)
endif()
file(MAKE_DIRECTORY "${ROOT}/share/umicom/runtime"
    "${ROOT}/share/umicom/framework/resources")
file(WRITE "${ROOT}/share/umicom/runtime/deployment.marker" "UMICOM_WINDOWS_RUNTIME=1\n")
file(WRITE "${ROOT}/share/umicom/runtime/gdk-pixbuf-loaders.cache.in"
    "\"@UMICOM_RUNTIME_ROOT@/lib/fixture-loader.dll\"\n")

# The startup fixture only checks environment ownership. This scanner-shaped
# file is never loaded or executed; real playback has separate native tests.
file(MAKE_DIRECTORY "${ROOT}/lib/gstreamer-1.0" "${ROOT}/libexec/gstreamer-1.0")
file(WRITE "${ROOT}/libexec/gstreamer-1.0/gst-plugin-scanner.exe" "environment fixture only\n")
