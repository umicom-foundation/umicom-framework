# Umicom Framework
# File: cmake/UmicomAiConversationExamples.cmake
#
# PURPOSE:
# Package the complete standalone conversation consumer and register its offline public-API exercise.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


include_guard(GLOBAL)
set(_umicom_conversation_example_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(BUILD_TESTING)
    add_executable(umicom-ai-conversation-example-test
        "${_umicom_conversation_example_root}/examples/conversation_archive/main.c")
    target_link_libraries(umicom-ai-conversation-example-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-conversation-example-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-conversation-example-test)
    umicom_apply_sanitizers(umicom-ai-conversation-example-test)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-ai-conversation-example-test
        PRODUCT_NAME "Umicom Conversation Example" INTERNAL_NAME "umicom-ai-conversation-example-test"
        APPLICATION_ID "org.umicom.conversation-example"
        RESOURCE_ROOT "${_umicom_conversation_example_root}/resources" NO_RUNTIME_DEPLOYMENT)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-conversation-example-test)
    endif()
    add_test(NAME framework.ai.conversation.sdk-example COMMAND umicom-ai-conversation-example-test)
    set_tests_properties(framework.ai.conversation.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;ai;conversation;example")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.ai.conversation.sdk-example TARGETS umicom-ai-conversation-example-test)
    endif()
endif()

# Install the complete example next to the canonical lesson in each supported
# package selection. Documentation-only packages contain source, not binaries.
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES
        "${_umicom_conversation_example_root}/examples/conversation_archive/CMakeLists.txt"
        "${_umicom_conversation_example_root}/examples/conversation_archive/main.c"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/conversation_archive"
        COMPONENT "${_component}" ${_selection})
endforeach()
