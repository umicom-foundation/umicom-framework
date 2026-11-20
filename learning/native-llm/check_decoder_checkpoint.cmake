#-----------------------------------------------------------------------------
# Umicom Framework
# File: learning/native-llm/check_decoder_checkpoint.cmake
#
# PURPOSE:
#   Verify an exact decoder roundtrip and preserve a previously saved checkpoint.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(NOT IS_ABSOLUTE "${PROGRAM}" OR NOT EXISTS "${PROGRAM}" OR NOT IS_ABSOLUTE "${WORKSPACE}")
    message(FATAL_ERROR "The decoder check needs a built executable and an absolute fixture workspace.")
endif()
string(RANDOM LENGTH 12 ALPHABET "0123456789abcdef" _run)
set(_directory "${WORKSPACE}/${_run}")
if(EXISTS "${_directory}")
    message(FATAL_ERROR "Choose a fresh decoder checkpoint fixture directory.")
endif()
file(MAKE_DIRECTORY "${_directory}")
set(_model "${_directory}/decoder.umidec")

execute_process(COMMAND "${PROGRAM}" "${_model}"
    RESULT_VARIABLE _first OUTPUT_VARIABLE _firstOutput ERROR_VARIABLE _firstErrors TIMEOUT 20)
if(NOT "${_first}" STREQUAL "0" OR NOT EXISTS "${_model}" OR
        NOT _firstOutput MATCHES "trained_steps=420 heldout_logits=24 parity=exact")
    message(FATAL_ERROR "The native decoder checkpoint did not qualify.\n${_firstOutput}\n${_firstErrors}")
endif()
file(SHA256 "${_model}" _before)

# Reusing an existing output path is the expected refusal. Its bytes still
# belong to the first successful run.
execute_process(COMMAND "${PROGRAM}" "${_model}"
    RESULT_VARIABLE _second OUTPUT_VARIABLE _secondOutput ERROR_VARIABLE _secondErrors TIMEOUT 20)
if(NOT "${_second}" STREQUAL "1" OR NOT _secondErrors MATCHES "UmiAiTrainingDecoderCheckpointSaveNewFile")
    message(FATAL_ERROR "The existing checkpoint did not get the expected refusal.\n${_secondOutput}\n${_secondErrors}")
endif()
file(SHA256 "${_model}" _after)
if(NOT _before STREQUAL _after)
    message(FATAL_ERROR "The existing decoder checkpoint changed after refusal.")
endif()
message(STATUS "${_firstOutput}")
message(STATUS "Existing checkpoint preserved at SHA-256 ${_before}")
