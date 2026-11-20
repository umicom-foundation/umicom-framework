#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/os_image/check_firmware.cmake
#
# PURPOSE:
#   Verify the public firmware observation and pin commands against inert files.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Keep generated inputs inside the test directory and use fixed known bytes.
if(NOT DEFINED PROGRAM OR NOT DEFINED WORK)
    message(FATAL_ERROR "Supply PROGRAM and WORK")
endif()
file(MAKE_DIRECTORY "${WORK}")
set(_firmware "${WORK}/firmware sample.bin")
file(WRITE "${_firmware}" "abc")
set(_digest "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")

# The executable must report both values and no child launch.
execute_process(COMMAND "${PROGRAM}" firmware-info --file "${_firmware}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(NOT _result STREQUAL "0" OR
   NOT _output MATCHES "^UMICOM_OS_FIRMWARE_INFO 1[\n\r]+sha256=${_digest}[\n\r]+bytes=3[\n\r]+" OR
   NOT _output MATCHES "completed=1 process-launched=0")
    message(FATAL_ERROR "Unexpected firmware observation (${_result}): ${_output}${_error}")
endif()

# Verify the same file using the independent digest, then reject a wrong size.
execute_process(COMMAND "${PROGRAM}" firmware --file "${_firmware}" --sha256 "${_digest}" --bytes 3
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(NOT _result STREQUAL "0" OR NOT _output MATCHES "completed=1 process-launched=0")
    message(FATAL_ERROR "Known firmware pin was refused (${_result}): ${_output}${_error}")
endif()
execute_process(COMMAND "${PROGRAM}" firmware --file "${_firmware}" --sha256 "${_digest}" --bytes 4
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(NOT _result STREQUAL "1" OR NOT _output MATCHES "completed=0 process-launched=0")
    message(FATAL_ERROR "Wrong firmware size was not refused (${_result}): ${_output}${_error}")
endif()

# An empty input may produce a failure report but must never publish a digest.
file(WRITE "${_firmware}" "")
execute_process(COMMAND "${PROGRAM}" firmware-info --file "${_firmware}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
if(NOT _result STREQUAL "1" OR _output MATCHES "UMICOM_OS_FIRMWARE_INFO" OR
   NOT _output MATCHES "completed=0 process-launched=0")
    message(FATAL_ERROR "Empty firmware observation was accepted (${_result}): ${_output}${_error}")
endif()
