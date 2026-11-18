#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_data/check_jsonl_file.cmake
# 
# PURPOSE:
#   Exercise local dataset inspection without modifying inputs or exposing their text.
# 
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
# 
# LICENCE:
# MIT
# ---------------------------------------------------------------------------*/


if(NOT DEFINED SUBJECT OR NOT DEFINED WORK OR NOT DEFINED CASE)
    message(FATAL_ERROR "Provide SUBJECT, WORK and CASE")
endif()

# A wrapper can execute even when its child target was never linked. Diagnose
# that prerequisite before creating corpus fixtures; this is still a failure,
# never a skip, a fabricated pass or an automatic build of unrelated targets.
if(NOT EXISTS "${SUBJECT}" OR IS_DIRECTORY "${SUBJECT}")
    message(FATAL_ERROR "Training test subject is missing or not a file. Build umicom-training-jsonl-file-test and resolve prerequisite build errors before CTest. Subject: ${SUBJECT}")
endif()

file(MAKE_DIRECTORY "${WORK}")
set(file "${WORK}/corpus.jsonl")
set(first "{\"source_id\":\"private-source\",\"prompt\":\"private-prompt-17\",\"completion\":\"private-answer-123\"}")
set(second "{\"source_id\":\"private-source\",\"prompt\":\"\",\"completion\":\"private-answer-1234\"}")
set(bytes "${first}\n${second}\n")
set(expected 0)
set(service TRUE)
if(CASE STREQUAL "unicode")
    set(file "${WORK}/資料 café.jsonl")
elseif(CASE STREQUAL "crlf")
    set(bytes "${first}\r\n${second}\r\n")
elseif(CASE STREQUAL "invalid-second")
    set(bytes "${first}\n{bad}\n")
    set(expected 1)
elseif(CASE STREQUAL "blank")
    set(bytes "${first}\n\n")
    set(expected 1)
elseif(CASE STREQUAL "duplicate")
    set(bytes "{\"source_id\":\"private-source\",\"prompt\":\"x\",\"prompt\":\"y\",\"completion\":\"z\"}")
    set(expected 1)
elseif(CASE STREQUAL "empty")
    set(bytes "")
    set(expected 1)
elseif(CASE STREQUAL "oversize")
    string(REPEAT " " 16777217 bytes)
    set(expected 1)
elseif(CASE STREQUAL "bom")
    string(ASCII 239 187 191 bom)
    set(bytes "${bom}${bytes}")
    set(expected 1)
elseif(CASE STREQUAL "missing" OR CASE STREQUAL "relative" OR CASE STREQUAL "directory")
    set(expected 1)
elseif(CASE STREQUAL "help" OR CASE STREQUAL "extra" OR CASE STREQUAL "unknown")
    set(service FALSE)
elseif(NOT CASE STREQUAL "valid" AND NOT CASE STREQUAL "arguments" AND NOT CASE STREQUAL "cancel")
    message(FATAL_ERROR "Unknown case")
endif()
file(WRITE "${file}" "${bytes}")
file(SHA256 "${file}" before)
set(selected "${file}")
if(CASE STREQUAL "missing")
    set(selected "${WORK}/absent.jsonl")
elseif(CASE STREQUAL "relative")
    set(selected "corpus.jsonl")
elseif(CASE STREQUAL "directory")
    set(selected "${WORK}")
endif()
if(service)
    execute_process(COMMAND "${SUBJECT}" service "${CASE}" "${selected}"
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 20)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Service check failed: ${result}\n${output}\n${error}")
    endif()
endif()
if(NOT CASE STREQUAL "arguments" AND NOT CASE STREQUAL "cancel")
    set(arguments inspect "${selected}")
    if(CASE STREQUAL "help")
        set(arguments --help)
    elseif(CASE STREQUAL "extra")
        list(APPEND arguments --train)
        set(expected 2)
    elseif(CASE STREQUAL "unknown")
        set(arguments train "${selected}")
        set(expected 2)
    endif()
    execute_process(COMMAND "${SUBJECT}" training-data ${arguments}
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 20)
    if(NOT "${result}" STREQUAL "${expected}")
        message(FATAL_ERROR "CLI expected ${expected}, got ${result}\n${output}\n${error}")
    endif()
    if(output MATCHES "private-" OR error MATCHES "private-")
        message(FATAL_ERROR "CLI exposed dataset contents")
    endif()
    if(expected EQUAL 0 AND NOT CASE STREQUAL "help")
        foreach(fragment "Rows: 2 | Source groups: 1 | Largest group rows: 2 | Empty prompts: 1"
            "Prompt bytes: 17 | Completion bytes: 37" "not tokens" "No model or training started")
            string(FIND "${output}" "${fragment}" found)
            if(found LESS 0)
                message(FATAL_ERROR "Missing output ${fragment}\n${output}")
            endif()
        endforeach()
    elseif(expected EQUAL 1)
        if(NOT error MATCHES "Training data refused" OR output MATCHES "schema accepted")
            message(FATAL_ERROR "Rejected input was not reported correctly")
        endif()
    endif()
endif()
file(SHA256 "${file}" after)
if(NOT before STREQUAL after)
    message(FATAL_ERROR "Inspection changed the selected file")
endif()
