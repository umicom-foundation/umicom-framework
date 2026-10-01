# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/implementation_fingerprint/check_private_includes.cmake
# PURPOSE: Check nested private includes, cycles and independent source identities.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomImplementationFingerprint.cmake")
if(NOT DEFINED WORK_ROOT OR WORK_ROOT STREQUAL "")
    message(FATAL_ERROR "WORK_ROOT is required.")
endif()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run)
set(work "${WORK_ROOT}/private-includes-${run}")
file(MAKE_DIRECTORY "${work}/private" "${work}/include/umicom")
file(WRITE "${work}/owner.c" "#include \"private/body.inc\"\n#include \"include/umicom/public.h\"\n")
file(WRITE "${work}/private/body.inc" "#include \"value.h\"\n")
file(WRITE "${work}/private/value.h" "#include \"body.inc\"\n#define VALUE 41\n")
file(WRITE "${work}/include/umicom/public.h" "/* Public contracts use their own fingerprint. */\n")
file(WRITE "${work}/independent.c" "int independent(void) { return 7; }\n")
umicom_collect_private_implementation_inputs(inputs "${work}" owner.c)
list(LENGTH inputs count)
if(NOT count EQUAL 3)
    message(FATAL_ERROR "The nested private closure did not contain exactly three inputs: ${inputs}")
endif()
umicom_collect_private_implementation_inputs(independent "${work}" independent.c)
umicom_compute_implementation_fingerprint(before "${work}" ${inputs})
umicom_compute_implementation_fingerprint(independent_before "${work}" ${independent})
file(APPEND "${work}/private/value.h" "#define EXTRA 42\n")
umicom_compute_implementation_fingerprint(after "${work}" ${inputs})
umicom_compute_implementation_fingerprint(independent_after "${work}" ${independent})
if(before STREQUAL after OR NOT independent_before STREQUAL independent_after)
    message(FATAL_ERROR "A private edit did not change only its dependent implementation identity.")
endif()
