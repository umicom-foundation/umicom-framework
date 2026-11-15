#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiResponseStream.cmake
#
# PURPOSE:
# Build and qualify request-owned incremental AI text in the shared AI SDK component.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_ai PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ai/response_stream.c")

# Mirror the source-relative example/docs/resources relationship in the SDK.
# The existing brand masters live under this same Framework resource root.
# No example is compiled or executed as part of producer configuration/install.
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../examples/sdk_ai_stream/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/sdk_ai_stream"
    COMPONENT Framework
    PATTERN "build" EXCLUDE PATTERN "build-*" EXCLUDE PATTERN ".git" EXCLUDE)
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../examples/sdk_ai_stream/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/sdk_ai_stream"
    COMPONENT FrameworkSDK EXCLUDE_FROM_ALL
    PATTERN "build" EXCLUDE PATTERN "build-*" EXCLUDE PATTERN ".git" EXCLUDE)

if(BUILD_TESTING)
    function(umicom_ai_response_stream_test_target target source)
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../${source}")
        target_compile_features(${target} PRIVATE c_std_23)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    function(umicom_ai_response_stream_case name target)
        add_test(NAME "${name}" COMMAND ${target} ${ARGN})
        set_tests_properties("${name}" PROPERTIES TIMEOUT 60 LABELS "framework;ai;stream;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${name}" TARGETS ${target})
        endif()
    endfunction()
    umicom_ai_response_stream_test_target(umicom-ai-response-stream-test tests/ai/test_response_stream.c)
    target_link_libraries(umicom-ai-response-stream-test PRIVATE Umicom::ai)
    foreach(_case IN ITEMS range-arguments range-ascii range-utf8 range-interior range-budget range-pending range-cancelled range-failed range-owned range-empty arguments empty-final ascii utf8-two utf8-three utf8-four invalid-leading invalid-continuation overlong surrogate out-of-range nul truncated-final whole-chunk-atomic capacity exact wrong-request wrong-revision duplicate gap zero-sequence duplicate-final after-final-cancel cancel cancel-pending cancel-idempotent failure failure-pending late-after-failure owned-copy byte-by-byte empty-event)
        umicom_ai_response_stream_case("framework.ai.response_stream.${_case}"
            umicom-ai-response-stream-test "${_case}")
    endforeach()
    umicom_ai_response_stream_test_target(umicom-ai-response-stream-threads-test tests/ai/test_response_stream_threads.c)
    target_link_libraries(umicom-ai-response-stream-threads-test PRIVATE Umicom::ai)
    foreach(_case IN ITEMS producer-reader producer-range-reader cancel-producer competing-sequence)
        umicom_ai_response_stream_case("framework.ai.response_stream_threads.${_case}"
            umicom-ai-response-stream-threads-test "${_case}")
    endforeach()
    umicom_ai_response_stream_test_target(umicom-ai-response-stream-sequence-test tests/ai/test_response_stream_sequence.c)
    # This white-box target includes the implementation once. It links only the
    # lower-level owners, avoiding another copy from the AI archive.
    target_link_libraries(umicom-ai-response-stream-sequence-test PRIVATE Umicom::platform)
    umicom_ai_response_stream_case(framework.ai.response_stream_sequence
        umicom-ai-response-stream-sequence-test)
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-ai-response-stream-sequence-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai/response_stream.c")
    endif()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE AND NOT MSVC)
        umicom_ai_response_stream_test_target(umicom-ai-response-stream-fault-test tests/ai/test_response_stream_faults.c)
        target_link_libraries(umicom-ai-response-stream-fault-test PRIVATE Umicom::ai)
        target_link_options(umicom-ai-response-stream-fault-test PRIVATE
            -Wl,--wrap=calloc -Wl,--wrap=malloc -Wl,--wrap=free
            -Wl,--wrap=umi_mutex_create -Wl,--wrap=umi_mutex_lock -Wl,--wrap=umi_mutex_unlock)
        foreach(_case IN ITEMS range-allocation range-lock range-unlock owner-allocation buffer-allocation mutex-create copy-allocation push-lock push-unlock copy-unlock inspect-unlock)
            umicom_ai_response_stream_case("framework.ai.response_stream_fault.${_case}"
                umicom-ai-response-stream-fault-test "${_case}")
        endforeach()
    endif()
endif()
