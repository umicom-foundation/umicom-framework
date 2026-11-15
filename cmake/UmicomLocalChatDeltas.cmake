#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalChatDeltas.cmake
#
# PURPOSE:
# Register native local-chat delta and offline capability regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING)
    function(umicom_local_delta_test_target target source)
        add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_workspace/${source}")
        target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace")
        target_compile_features(${target} PRIVATE c_std_23)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endfunction()
    function(umicom_local_delta_case name target)
        add_test(NAME "${name}" COMMAND ${target} ${ARGN})
        set_tests_properties("${name}" PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
            LABELS "framework;ai-workspace;stream;protocol")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${name}" TARGETS ${target})
        endif()
    endfunction()
    umicom_local_delta_test_target(umicom-local-chat-delta-test test_local_chat_delta.c)
    target_link_libraries(umicom-local-chat-delta-test PRIVATE Umicom::ai_workspace)
    foreach(_case IN ITEMS content unicode empty-content role null-content empty-delta stop length usage usage-only null-usage null-refusal wrong-model missing-model missing-id empty-id wrong-object empty-choices multiple-choices choice-index negative-index fractional-index missing-index missing-delta array-delta wrong-role null-role array-content nul-content control-content missing-finish unknown-finish usage-mismatch usage-negative error tool-call function-call tool-finish audio refusal reasoning trailing-json malformed)
        umicom_local_delta_case("framework.ai_workspace.delta.${_case}" umicom-local-chat-delta-test "${_case}")
    endforeach()
    umicom_local_delta_test_target(umicom-local-chat-delta-boundary-test test_local_chat_delta_boundaries.c)
    target_link_libraries(umicom-local-chat-delta-boundary-test PRIVATE Umicom::ai_workspace)
    foreach(_case IN ITEMS done padded-done arguments raw-nul invalid-utf8 body-limit content-limit encode-stream encode-full encode-tools encode-role encode-count encode-temperature)
        umicom_local_delta_case("framework.ai_workspace.delta_boundary.${_case}" umicom-local-chat-delta-boundary-test "${_case}")
    endforeach()
    # Compile the real codec without its optional HTTP definition. Link only
    # Base so the normal AI workspace archive does not supply another copy.
    umicom_local_delta_test_target(umicom-local-chat-delta-offline-test test_local_chat_delta_offline.c)
    target_sources(umicom-local-chat-delta-offline-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_chat_codec.c")
    target_link_libraries(umicom-local-chat-delta-offline-test PRIVATE Umicom::base)
    umicom_local_delta_case(framework.ai_workspace.delta_offline umicom-local-chat-delta-offline-test)
endif()
