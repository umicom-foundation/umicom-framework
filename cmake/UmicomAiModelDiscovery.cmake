# Umicom Framework
# File: cmake/UmicomAiModelDiscovery.cmake
#
# PURPOSE:
# Compile shared provider model observations and package the independent SDK learning project.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
include_guard(GLOBAL)
set(_umicom_discovery_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_ai PRIVATE "${_umicom_discovery_root}/src/ai/model_discovery.c")
target_link_libraries(umicom_ai PRIVATE Umicom::protocol)
if(BUILD_TESTING)
    add_executable(umicom-ai-model-discovery-test "${_umicom_discovery_root}/tests/ai/test_model_discovery.c")
    add_executable(umicom-ai-model-discovery-example-test "${_umicom_discovery_root}/examples/model_discovery/main.c")
    foreach(_target umicom-ai-model-discovery-test umicom-ai-model-discovery-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::ai)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case openai available loaded empty ownership source-order case-sensitive unicode max-id long-id duplicate-model duplicate-escaped-model duplicate-key duplicate-extension wrong-root missing-list wrong-list wrong-item missing-id empty-id wrong-id wrong-object negative-size fractional-size overflow-size wrong-details null-name known-zero available-not-loaded unknown-fields trailing embedded-nul depth model-limit nulls cancel read-refusal)
        add_test(NAME framework.ai.model_discovery.${_case} COMMAND umicom-ai-model-discovery-test ${_case})
        set_tests_properties(framework.ai.model_discovery.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;ai;discovery;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai.model_discovery.${_case} TARGETS umicom-ai-model-discovery-test)
        endif()
    endforeach()
    add_test(NAME framework.ai.model_discovery.sdk-example COMMAND umicom-ai-model-discovery-example-test)
    set_tests_properties(framework.ai.model_discovery.sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;ai;sdk;example")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.ai.model_discovery.sdk-example TARGETS umicom-ai-model-discovery-example-test)
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    umicom_apply_application_branding(TARGET umicom-ai-model-discovery-example-test
        PRODUCT_NAME "Umicom Model Discovery Example" INTERNAL_NAME "umicom-ai-model-discovery-example-test"
        APPLICATION_ID "org.umicom.model-discovery-example"
        RESOURCE_ROOT "${_umicom_discovery_root}/resources" NO_RUNTIME_DEPLOYMENT)
    if(TARGET umicom-connection-catalogue-test)
        add_test(NAME framework.provider_connections.catalogue.repeated-model COMMAND umicom-connection-catalogue-test repeated-model)
        set_tests_properties(framework.provider_connections.catalogue.repeated-model PROPERTIES TIMEOUT 20 LABELS "framework;ai;catalogue;regression")
    endif()
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES
        "${_umicom_discovery_root}/examples/model_discovery/main.c"
        "${_umicom_discovery_root}/examples/model_discovery/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/model_discovery"
        COMPONENT "${_component}" ${_selection})
    install(FILES
        "${_umicom_discovery_root}/docs/learning/model-discovery-sdk.html"
        "${_umicom_discovery_root}/docs/learning/native-ai-applications-course.html"
        "${_umicom_discovery_root}/docs/learning/streamlit-state-and-native-forms.html"
        "${_umicom_discovery_root}/docs/learning/comfyui-workflows-and-native-clients.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning"
        COMPONENT "${_component}" ${_selection})
endforeach()

# Preserve provider-response erasure when consumers share the protocol owner.
if(BUILD_TESTING)
    add_executable(umicom-json-owned-erasure-test "${_umicom_discovery_root}/tests/json_tree/test_owned_text_erasure.c")
    target_link_libraries(umicom-json-owned-erasure-test PRIVATE Umicom::protocol Umicom::security)
    set_target_properties(umicom-json-owned-erasure-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-json-owned-erasure-test)
    umicom_apply_sanitizers(umicom-json-owned-erasure-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-json-owned-erasure-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-json-owned-erasure-test FILES "${_umicom_discovery_root}/src/language_runtime/json_tree.c")
    endif()
    foreach(_case success parse-failure node-limit cancel allocation empty)
        add_test(NAME framework.json_tree.erasure.${_case} COMMAND umicom-json-owned-erasure-test ${_case})
        set_tests_properties(framework.json_tree.erasure.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;json;ownership;erasure")
    endforeach()
endif()

# Browser filtering shares the discovery snapshot instead of duplicating a catalogue.
if(BUILD_TESTING)
    add_executable(umicom-ai-model-query-test "${_umicom_discovery_root}/tests/ai/test_model_discovery_query.c")
    target_link_libraries(umicom-ai-model-query-test PRIVATE Umicom::ai)
    set_target_properties(umicom-ai-model-query-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-ai-model-query-test)
    umicom_apply_sanitizers(umicom-ai-model-query-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-model-query-test)
    endif()
    foreach(_case source id-order name-order size-order context-order search-name search-id search-family search-unicode no-unicode-fold family facet-case format quantization combined known size-zero size-range context-min context-too-high no-match ownership lookup-after-sort source-unchanged empty openai unterminated-search unterminated-family invalid-utf8 unknown-known invalid-order invalid-enable disabled-limit cancel occupied nulls)
        add_test(NAME framework.ai.model_query.${_case} COMMAND umicom-ai-model-query-test ${_case})
        set_tests_properties(framework.ai.model_query.${_case} PROPERTIES TIMEOUT 20 LABELS "framework;ai;model-query;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai.model_query.${_case} TARGETS umicom-ai-model-query-test)
        endif()
    endforeach()
endif()


# Per-model declarations complement inventory/loaded observations without
# mutating application policy or inferring effects from provider metadata.
target_sources(umicom_ai PRIVATE "${_umicom_discovery_root}/src/ai/ollama_model_info.c")
if(BUILD_TESTING)
    add_executable(umicom-ollama-model-info-test "${_umicom_discovery_root}/tests/ai/test_ollama_model_info.c")
    add_executable(umicom-ollama-model-info-example-test "${_umicom_discovery_root}/examples/ollama_model_info/main.c")
    foreach(_target umicom-ollama-model-info-test umicom-ollama-model-info-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::ai)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_discovery_root}/src/ai/ollama_model_info.c"
                "${_umicom_discovery_root}/src/ai/model_observation_json.h"
                "${_umicom_discovery_root}/src/language_runtime/json_tree.c")
        endif()
    endforeach()
    foreach(mode complete absent empty-capabilities known-zero exact-integers missing-architecture foreign-architecture unknown-capability case-sensitive wrong-root details-type model-info-type capabilities-type capability-type empty-capability duplicate-capability escaped-duplicate duplicate-key nested-duplicate numeric-negative numeric-fraction numeric-overflow architecture-null detail-null provider-error empty-error wrong-error trailing embedded-nul invalid-utf8 capability-max capability-overflow capability-count-max capability-count-overflow model-max model-overflow model-utf8 model-empty byte-limit exact-byte-limit depth node-limit ownership occupied cancel read-refusal capability-bounds query-refusal nulls)
        add_test(NAME framework.ai.ollama_model_info.${mode} COMMAND umicom-ollama-model-info-test "${mode}")
        set_tests_properties(framework.ai.ollama_model_info.${mode} PROPERTIES TIMEOUT 45 LABELS "framework;ai;ollama;model-info;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai.ollama_model_info.${mode} TARGETS umicom-ollama-model-info-test)
        endif()
    endforeach()
    add_test(NAME framework.ai.ollama_model_info.sdk-example COMMAND umicom-ollama-model-info-example-test)
    set_tests_properties(framework.ai.ollama_model_info.sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;ai;sdk;example")
    umicom_apply_application_branding(TARGET umicom-ollama-model-info-example-test
        PRODUCT_NAME "Umicom Model Detail Example" INTERNAL_NAME "umicom-ollama-model-info-example"
        APPLICATION_ID "org.umicom.ollama-model-info-example"
        RESOURCE_ROOT "${_umicom_discovery_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${_umicom_discovery_root}/examples/ollama_model_info"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples" COMPONENT "${_component}" ${_selection})
endforeach()
