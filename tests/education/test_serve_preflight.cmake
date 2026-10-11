#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/education/test_serve_preflight.cmake
# PURPOSE: Prove that missing generated pages do not launch the HTTP server,
#          that --build remains explicit, and that unrelated commands refuse it.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

if(NOT DEFINED EDUCATION_PROGRAM OR NOT DEFINED EDUCATION_FIXTURE_ROOT)
    message(FATAL_ERROR "Provide EDUCATION_PROGRAM and EDUCATION_FIXTURE_ROOT")
endif()

# The fixture is wholly within the CMake test tree, never the user's checkout.
set(repo "${EDUCATION_FIXTURE_ROOT}/missing-site-checkout")
file(MAKE_DIRECTORY "${repo}")
file(WRITE "${repo}/mkdocs.yml" "site_name: Isolated learning fixture\n")

execute_process(
    COMMAND "${EDUCATION_PROGRAM}" serve --repo "${repo}" --port 8765
    RESULT_VARIABLE serveResult OUTPUT_VARIABLE serveOut ERROR_VARIABLE serveErr
    TIMEOUT 10)
if(NOT "${serveResult}" STREQUAL "2" OR
   NOT serveErr MATCHES "no generated website exists" OR
   NOT serveErr MATCHES "doctor.*does not build pages")
    message(FATAL_ERROR "Expected informative missing-site refusal. rc=${serveResult}\n${serveOut}\n${serveErr}")
endif()

# The selected interpreter deliberately cannot launch. This test must never
# start a socket or unexpectedly fall back to another Python installation.
execute_process(
    COMMAND "${EDUCATION_PROGRAM}" serve --repo "${repo}" --build
            --python "umicom-education-test-missing-interpreter-404" --port 8765
    RESULT_VARIABLE buildResult OUTPUT_VARIABLE buildOut ERROR_VARIABLE buildErr
    TIMEOUT 10)
if(NOT "${buildResult}" STREQUAL "1" OR
   NOT buildErr MATCHES "Site build failed; the local server was not started")
    message(FATAL_ERROR "--build must propagate renderer failure. rc=${buildResult}\n${buildOut}\n${buildErr}")
endif()

execute_process(
    COMMAND "${EDUCATION_PROGRAM}" serve --repo "${repo}" --build --directory "${repo}"
    RESULT_VARIABLE directoryResult OUTPUT_VARIABLE directoryOut ERROR_VARIABLE directoryErr
    TIMEOUT 10)
if(NOT "${directoryResult}" STREQUAL "2" OR
   NOT directoryErr MATCHES "--directory is for existing exports only")
    message(FATAL_ERROR "--build and --directory must be incompatible. rc=${directoryResult}\n${directoryOut}\n${directoryErr}")
endif()

execute_process(
    COMMAND "${EDUCATION_PROGRAM}" verify --repo "${repo}" --build
    RESULT_VARIABLE verifyResult OUTPUT_VARIABLE verifyOut ERROR_VARIABLE verifyErr
    TIMEOUT 10)
if(NOT "${verifyResult}" STREQUAL "2" OR
   NOT verifyErr MATCHES "--build applies only to serve")
    message(FATAL_ERROR "verify must not silently accept --build. rc=${verifyResult}\n${verifyOut}\n${verifyErr}")
endif()

message(STATUS "Native preview preflight checks passed; no HTTP port opened")
