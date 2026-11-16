# Umicom Framework
# File: cmake/UmicomAiSearchSnapshots.cmake
#
# PURPOSE:
#   Package owned retrieval observations and an independent source-selection SDK project.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(NOT TARGET umicom_ai_workspace)
    message(FATAL_ERROR "Search snapshots require the canonical AI workspace")
endif()
set(_umicom_search_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_ai_workspace PRIVATE "${_umicom_search_root}/src/ai_workspace/search_snapshot.c")
if(BUILD_TESTING)
    add_executable(umicom-ai-search-snapshot-test "${_umicom_search_root}/tests/ai_workspace/test_search_snapshot.c")
    add_executable(umicom-ai-retrieval-sources-example-test "${_umicom_search_root}/examples/retrieval_sources/main.c")
    foreach(_target umicom-ai-search-snapshot-test umicom-ai-retrieval-sources-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::ai_workspace)
        umicom_ai_workspace_configure_target(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_search_root}/src/ai_workspace/search_snapshot.c"
                "${_umicom_search_root}/src/ai_workspace/retrieval.c"
                "${_umicom_search_root}/src/ai_workspace/evidence_selection.c")
        endif()
    endforeach()
    foreach(mode lexical limit empty collection-scope unicode unknown-collection invalid-collection null-query invalid-utf8 long-query stopwords zero-limit large-limit null-workspace occupied null-output vector missing-vector model-mismatch invalid-vector null-model rerank negative-rerank nan-rerank infinite-rerank invalid-reranker reentrant ownership input-copy changed-source source-copy summary-refusal row-refusal select-order select-maximum select-duplicate select-late-invalid select-zero select-overflow select-null select-alias prepare stale-selected-source stale-other-source capacity)
        add_test(NAME framework.ai_workspace.search_snapshot.${mode} COMMAND umicom-ai-search-snapshot-test "${mode}")
        set_tests_properties(framework.ai_workspace.search_snapshot.${mode} PROPERTIES
            TIMEOUT 30 LABELS "framework;ai-workspace;retrieval;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.ai_workspace.search_snapshot.${mode} TARGETS umicom-ai-search-snapshot-test)
        endif()
    endforeach()
    add_test(NAME framework.ai_workspace.search_snapshot.sdk-example COMMAND umicom-ai-retrieval-sources-example-test)
    set_tests_properties(framework.ai_workspace.search_snapshot.sdk-example PROPERTIES
        TIMEOUT 30 LABELS "framework;ai-workspace;sdk;example")
    umicom_apply_application_branding(TARGET umicom-ai-retrieval-sources-example-test
        PRODUCT_NAME "Umicom Retrieval Sources Example" INTERNAL_NAME "umicom-retrieval-sources-example"
        APPLICATION_ID "org.umicom.retrieval-sources-example"
        RESOURCE_ROOT "${_umicom_search_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${_umicom_search_root}/examples/retrieval_sources"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples"
        COMPONENT "${_component}" ${_selection})
    install(FILES "${_umicom_search_root}/docs/learning/local-document-retrieval-sdk.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning"
        COMPONENT "${_component}" ${_selection})
endforeach()
unset(_umicom_search_root)
