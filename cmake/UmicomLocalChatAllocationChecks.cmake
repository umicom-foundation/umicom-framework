#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLocalChatAllocationChecks.cmake
#
# PURPOSE:
# Register precise JSON conversion allocation diagnostics against the real native codec.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
if(BUILD_TESTING AND UMICOM_AI_WORKSPACE_HTTP_BUILT)
    add_executable(umicom-local-chat-allocation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_workspace/test_local_chat_allocations.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/text.c")
    target_include_directories(umicom-local-chat-allocation-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace")
    target_compile_definitions(umicom-local-chat-allocation-test PRIVATE UMICOM_AI_WORKSPACE_HAS_HTTP=1)
    target_link_libraries(umicom-local-chat-allocation-test PRIVATE Umicom::base PkgConfig::UMICOM_AI_LOCAL_JSONC)
    target_compile_features(umicom-local-chat-allocation-test PRIVATE c_std_23)
    umicom_apply_warnings(umicom-local-chat-allocation-test)
    umicom_apply_sanitizers(umicom-local-chat-allocation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-local-chat-allocation-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-local-chat-allocation-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/ai_workspace/local_chat_codec.c")
    endif()
    foreach(_codec IN ITEMS chat health delta)
        foreach(_allocation IN ITEMS 1 2)
            add_test(NAME "framework.ai_workspace.json_allocation.${_codec}.${_allocation}"
                COMMAND umicom-local-chat-allocation-test "${_codec}" "${_allocation}")
            set_tests_properties("framework.ai_workspace.json_allocation.${_codec}.${_allocation}"
                PROPERTIES TIMEOUT 45 LABELS "framework;ai-workspace;memory;protocol")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.ai_workspace.json_allocation.${_codec}.${_allocation}"
                    TARGETS umicom-local-chat-allocation-test)
            endif()
        endforeach()
    endforeach()
endif()
