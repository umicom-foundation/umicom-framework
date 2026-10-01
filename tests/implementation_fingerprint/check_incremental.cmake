#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/implementation_fingerprint/check_incremental.cmake
# PURPOSE: Exercise source and private-header copies against an existing compiled library.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(argument WORK_ROOT CC GENERATOR)
    if(NOT DEFINED ${argument} OR "${${argument}}" STREQUAL "")
        message(FATAL_ERROR "Missing incremental fixture argument: ${argument}")
    endif()
endforeach()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run)
set(work "${WORK_ROOT}/incremental-${run}")
set(build "${work}/build")
set(fixture "${CMAKE_CURRENT_LIST_DIR}/fixture")
file(MAKE_DIRECTORY "${work}")
file(COPY "${fixture}/CMakeLists.txt" "${fixture}/main.c" "${fixture}/independent.c"
    "${fixture}/before/owner.c" "${fixture}/before/private.h" DESTINATION "${work}")

function(RunStep label)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "${label} failed (${result}):\n${output}\n${error}")
    endif()
endfunction()

function(Qualify expected)
    set(options "")
    if(DEFINED MAKE_PROGRAM AND NOT MAKE_PROGRAM STREQUAL "")
        list(APPEND options "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}")
    endif()
    RunStep("Configure expected value ${expected}" "${CMAKE_COMMAND}"
        -S "${work}" -B "${build}" -G "${GENERATOR}"
        "-DCMAKE_C_COMPILER=${CC}" "-DCMAKE_BUILD_TYPE=Debug" "-DEXPECTED_VALUE=${expected}"
        "-DFINGERPRINT_MODULE=${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomImplementationFingerprint.cmake" ${options})
    RunStep("Build expected value ${expected}" "${CMAKE_COMMAND}" --build "${build}" --config Debug --target consumer)
    file(READ "${build}/consumer-Debug.txt" executable)
    RunStep("Run expected value ${expected}" "${executable}")
endfunction()

Qualify(0)
file(STRINGS "${build}/objects-Debug.txt" objects)
set(independent "")
foreach(object IN LISTS objects)
    if(object MATCHES "independent[.]c[.](o|obj)$")
        set(independent "${object}")
    endif()
endforeach()
if(independent STREQUAL "" OR NOT EXISTS "${independent}")
    message(FATAL_ERROR "The fixture did not expose its independent object.")
endif()
file(TIMESTAMP "${independent}" independent_before "%s")
function(RequireIndependentObject)
    file(TIMESTAMP "${independent}" independent_after "%s")
    if(NOT independent_before STREQUAL independent_after)
        message(FATAL_ERROR "An unrelated object rebuilt after an owner source or private-header edit.")
    endif()
endfunction()
# Separate timestamp observations even on filesystems with coarse resolution.
RunStep("Separate object timestamps" "${CMAKE_COMMAND}" -E sleep 2)
file(READ "${build}/owner-Debug.txt" archive)
file(TIMESTAMP "${archive}" built_time "%s")
# Preserve the earlier fixture file for inspection. COPY retains the committed
# replacement's timestamp, which predates the object built immediately above.
# Merely touching the new file would hide the regression this fixture targets.
file(RENAME "${work}/owner.c" "${work}/owner-before.c")
file(COPY "${fixture}/after/owner.c" DESTINATION "${work}")
file(TIMESTAMP "${fixture}/after/owner.c" source_time "%s")
file(TIMESTAMP "${work}/owner.c" copied_time "%s")
if(NOT source_time STREQUAL copied_time)
    message(FATAL_ERROR "The source copy did not preserve its timestamp.")
endif()
if(copied_time GREATER built_time)
    message(FATAL_ERROR "The replacement is newer than the old archive; this fixture needs an older source copy.")
endif()
Qualify(41)
RequireIndependentObject()
file(RENAME "${work}/private.h" "${work}/private-before.h")
file(COPY "${fixture}/after/private.h" DESTINATION "${work}")
file(TIMESTAMP "${fixture}/after/private.h" private_time "%s")
file(TIMESTAMP "${work}/private.h" private_copy_time "%s")
if(NOT private_time STREQUAL private_copy_time)
    message(FATAL_ERROR "The private-header copy did not preserve its timestamp.")
endif()
Qualify(42)
RequireIndependentObject()
# A repeated configuration of identical content must leave the same identity.
file(READ "${build}/owner-identity.txt" identity_before)
Qualify(42)
RequireIndependentObject()
file(READ "${build}/owner-identity.txt" identity_after)
if(NOT identity_before STREQUAL identity_after)
    message(FATAL_ERROR "An unchanged fixture configuration changed its implementation identity.")
endif()
