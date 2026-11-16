# Umicom Framework
# File: tests/provider_connections/check_document_file.cmake
#
# PURPOSE:
#   Create isolated saved-document fixtures and verify native consumers leave the input unchanged.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(NOT DEFINED TEST_EXE OR NOT DEFINED TEST_ROOT OR NOT DEFINED TEST_CASE OR
   NOT IS_ABSOLUTE "${TEST_ROOT}")
    message(FATAL_ERROR "TEST_EXE, an absolute build-owned TEST_ROOT and TEST_CASE are required")
endif()
if(TEST_CASE STREQUAL "unicode")
    string(APPEND TEST_ROOT "/équipe-東京")
endif()
file(MAKE_DIRECTORY "${TEST_ROOT}")
set(path "${TEST_ROOT}/settings.umiconnection")
# Independent canonical bytes: never generate the oracle with the codec under test.
set(content "umicom-provider-connection\n1\n7\nconnection\n2\n30000\n1\n636f6d66792d6578616d706c65\n636f6d66797569\n436f6d66795549206c6f63616c\n687474703a2f2f3132372e302e302e313a38313838\n\n\n")
if(TEST_CASE STREQUAL "empty")
    set(content "")
elseif(TEST_CASE STREQUAL "malformed")
    set(content "broken\n")
elseif(TEST_CASE STREQUAL "unknown-version")
    set(content "umicom-provider-connection\n9\n")
elseif(TEST_CASE STREQUAL "crlf")
    string(REPLACE "\n" "\r\n" content "${content}")
elseif(TEST_CASE STREQUAL "within-limit")
    string(REPEAT "x" 8191 content)
elseif(TEST_CASE STREQUAL "over-limit")
    string(REPEAT "x" 8192 content)
endif()
file(WRITE "${path}" "${content}")
file(SHA256 "${path}" before)
set(input "${path}")
if(TEST_CASE STREQUAL "missing")
    set(input "${TEST_ROOT}/absent/settings.umiconnection")
elseif(TEST_CASE STREQUAL "directory")
    set(input "${TEST_ROOT}")
elseif(TEST_CASE STREQUAL "relative")
    set(input "settings.umiconnection")
endif()
if(TEST_SDK)
    if(TEST_CASE STREQUAL "help")
        set(args --help)
    elseif(TEST_CASE STREQUAL "extra")
        set(args "${input}" extra)
    else()
        set(args "${input}")
    endif()
else()
    set(args "${TEST_CASE}" "${input}")
endif()
execute_process(COMMAND "${TEST_EXE}" ${args}
    WORKING_DIRECTORY "${TEST_ROOT}" RESULT_VARIABLE result
    OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 25)
file(SHA256 "${path}" after)
if(NOT before STREQUAL after)
    message(FATAL_ERROR "Reader changed its input file")
endif()
if(TEST_SDK)
    set(expected 1)
    if(TEST_CASE STREQUAL "valid" OR TEST_CASE STREQUAL "unicode" OR TEST_CASE STREQUAL "help")
        set(expected 0)
    elseif(TEST_CASE STREQUAL "extra")
        set(expected 2)
    endif()
    if((TEST_CASE STREQUAL "valid" OR TEST_CASE STREQUAL "unicode") AND
       NOT output MATCHES "Observed revision: 7")
        message(FATAL_ERROR "SDK reader did not report the historical revision")
    endif()
    if(output MATCHES "comfy-example|127\\.0\\.0\\.1|PRIVATE_SENTINEL" OR
       error MATCHES "comfy-example|127\\.0\\.0\\.1|PRIVATE_SENTINEL")
        message(FATAL_ERROR "SDK reader exposed raw connection fields")
    endif()
else()
    set(expected 0)
endif()
if(NOT result STREQUAL "${expected}")
    message(FATAL_ERROR "Document file ${TEST_CASE}: ${result}, expected ${expected}\n${output}\n${error}")
endif()
