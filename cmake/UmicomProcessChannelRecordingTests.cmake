#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomProcessChannelRecordingTests.cmake
#
# PURPOSE:
#   Register shared native stderr recording and finalization regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING OR NOT (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    return()
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomProcessChannel.cmake")
set(_umi_recording_tests "${CMAKE_CURRENT_LIST_DIR}/../tests/process_channel_recording")
add_executable(umicom-channel-recording-producer "${_umi_recording_tests}/producer.c")
add_executable(umicom-channel-recording-test "${_umi_recording_tests}/test_recording.c")
add_executable(umicom-channel-recording-finish-test "${_umi_recording_tests}/test_finish.c")
target_link_libraries(umicom-channel-recording-test PRIVATE Umicom::process_channel Umicom::output_file)
# Fault injection compiles the real file and shared-channel sources directly;
# linking the normal implementations would defeat its isolated OS boundary.
target_include_directories(umicom-channel-recording-finish-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../include")
foreach(target IN ITEMS umicom-channel-recording-producer umicom-channel-recording-test
        umicom-channel-recording-finish-test)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
endforeach()
if(MINGW)
    target_link_options(umicom-channel-recording-producer PRIVATE -static-libgcc)
endif()
add_dependencies(umicom-channel-recording-test umicom-channel-recording-producer)
foreach(case IN ITEMS binary large unicode existing relative empty null-path missing-parent
        invalid-request null-output terminate destroy exit closed-stdio borrowed-path legacy)
    add_test(NAME framework.process_channel.recording.${case}
        COMMAND umicom-channel-recording-test "${case}" $<TARGET_FILE:umicom-channel-recording-producer>)
    set_tests_properties(framework.process_channel.recording.${case} PROPERTIES
        TIMEOUT 40 SKIP_RETURN_CODE 77 LABELS "framework;process;recording;ownership;regression")
endforeach()
foreach(case IN ITEMS success flush close)
    add_test(NAME framework.process_channel.recording_finish.${case}
        COMMAND umicom-channel-recording-finish-test "${case}")
    set_tests_properties(framework.process_channel.recording_finish.${case} PROPERTIES
        TIMEOUT 10 LABELS "framework;process;recording;fault-injection;regression")
endforeach()
unset(_umi_recording_tests)
