#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning_pack_inspector/check_practice.cmake
#
# PURPOSE:
#   Check installed example preview, explicit copy and refusal to overwrite.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
foreach(_required PROGRAM PACK_ROOT TEST_ROOT)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing ${_required}")
    endif()
endforeach()
# Each execution reserves a separate location, keeping earlier evidence and
# allowing CTest reruns without removing a learner's or another test's file.
string(RANDOM LENGTH 24 ALPHABET 0123456789abcdef _nonce)
set(_destination "${TEST_ROOT}/practice-${_nonce}")
if(EXISTS "${_destination}")
    message(FATAL_ERROR "Practice fixture already exists; run the check again")
endif()
file(MAKE_DIRECTORY "${_destination}")
file(SHA256 "${PACK_ROOT}/main.c" _before)
execute_process(COMMAND "${PROGRAM}" "${PACK_ROOT}" learning-pack.json guide
    "${_destination}" example.c RESULT_VARIABLE _preview
    OUTPUT_FILE "${_destination}/preview.log" ERROR_FILE "${_destination}/preview-errors.log"
    TIMEOUT 30)
if(NOT "${_preview}" STREQUAL "0" OR EXISTS "${_destination}/example.c")
    message(FATAL_ERROR "Preview failed or created a file; read ${_destination}")
endif()
execute_process(COMMAND "${PROGRAM}" "${PACK_ROOT}" learning-pack.json guide
    "${_destination}" example.c --copy RESULT_VARIABLE _copy
    OUTPUT_FILE "${_destination}/copy.log" ERROR_FILE "${_destination}/copy-errors.log"
    TIMEOUT 30)
if(NOT "${_copy}" STREQUAL "0" OR NOT EXISTS "${_destination}/example.c")
    message(FATAL_ERROR "Explicit copy failed; read ${_destination}")
endif()
file(SHA256 "${_destination}/example.c" _copied)
if(NOT _copied STREQUAL _before)
    message(FATAL_ERROR "The practice copy differs from the verified example")
endif()
# Preserve a learner edit, not only identical original bytes.
file(APPEND "${_destination}/example.c" "\n/* My practice notes. */\n")
file(SHA256 "${_destination}/example.c" _edited)
execute_process(COMMAND "${PROGRAM}" "${PACK_ROOT}" learning-pack.json guide
    "${_destination}" example.c --copy RESULT_VARIABLE _repeat
    OUTPUT_FILE "${_destination}/repeat.log" ERROR_FILE "${_destination}/repeat-errors.log"
    TIMEOUT 30)
if(NOT "${_repeat}" STREQUAL "1")
    message(FATAL_ERROR "An existing destination was not refused; read ${_destination}")
endif()
file(SHA256 "${_destination}/example.c" _after_copy)
file(SHA256 "${PACK_ROOT}/main.c" _after_source)
if(NOT _after_copy STREQUAL _edited OR NOT _after_source STREQUAL _before)
    message(FATAL_ERROR "A repeat operation modified existing content")
endif()
message(STATUS "Practice copy evidence: ${_destination}")
