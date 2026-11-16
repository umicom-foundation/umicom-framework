# Umicom Framework
# File: cmake/UmicomComfyNodePages.cmake
#
# PURPOSE:
#   Compose captured node browsing in the existing protocol SDK and register reusable native contracts.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(NOT TARGET umicom_comfy_protocol)
    message(FATAL_ERROR "Node pages require the canonical Comfy protocol owner")
endif()
set(_node_pages_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_comfy_protocol PRIVATE
    "${_node_pages_root}/src/media_generation/comfy_node_pages_file.c"
    "${_node_pages_root}/src/media_generation/comfy_node_pages_json.c"
    "${_node_pages_root}/src/media_generation/comfy_node_pages_command.c")
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_comfy_protocol include/umicom/media_generation/comfy_node_pages.h)
endif()
if(BUILD_TESTING)
    add_executable(umicom-comfy-node-pages-json-test "${_node_pages_root}/tests/media_generation/test_comfy_node_pages_json.c")
    add_executable(umicom-comfy-node-pages-file-test "${_node_pages_root}/tests/media_generation/test_comfy_node_pages_file.c")
    add_executable(umicom-comfy-node-pages-example-test "${_node_pages_root}/examples/comfy_node_pages/main.c")
    foreach(_target umicom-comfy-node-pages-json-test umicom-comfy-node-pages-file-test umicom-comfy-node-pages-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol Umicom::platform Umicom::protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_node_pages_root}/src/media_generation/comfy_node_pages_file.c"
                "${_node_pages_root}/src/media_generation/comfy_node_pages_json.c"
                "${_node_pages_root}/src/media_generation/comfy_node_pages_command.c"
                "${_node_pages_root}/src/media_generation/comfy_nodes.c"
                "${_node_pages_root}/src/media_generation/comfy_palette.c"
                "${_node_pages_root}/src/provider_connections/document_file.c")
        endif()
    endforeach()
    foreach(mode summary unknown-flags known-false escaping private-values required-true required-false category search empty no-match maximum-escaped-page copied-lifetime page last-page end different-order exact-capacity short-capacity zero-capacity null-owner null-output null-length zero-limit large-limit beyond huge-offset)
        add_test(NAME framework.media.comfy.node_pages_json.${mode} COMMAND umicom-comfy-node-pages-json-test "${mode}")
        set_tests_properties(framework.media.comfy.node_pages_json.${mode} PROPERTIES TIMEOUT 30 LABELS "framework;comfy;nodes;json")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.node_pages_json.${mode} TARGETS umicom-comfy-node-pages-json-test)
        endif()
    endforeach()
    foreach(mode complete empty unicode-path filter disabled malformed-document malformed-catalogue missing-file directory catalogue-limit document-limit empty-file relative null-document null-input null-query null-output cancel occupied invalid-query)
        add_test(NAME framework.media.comfy.node_pages_file.${mode} COMMAND "${CMAKE_COMMAND}"
            "-DTEST_EXE=$<TARGET_FILE:umicom-comfy-node-pages-file-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/comfy-node-pages-file/${mode}"
            "-DTEST_CASE=${mode}" -P "${_node_pages_root}/tests/media_generation/check_comfy_node_pages_file.cmake")
        set_tests_properties(framework.media.comfy.node_pages_file.${mode} PROPERTIES TIMEOUT 45 LABELS "framework;comfy;nodes;file")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.node_pages_file.${mode} TARGETS umicom-comfy-node-pages-file-test)
        endif()
    endforeach()
    umicom_add_comfy_node_pages_command_tests(umicom-comfy-node-pages-example-test framework.media.comfy.node_pages_sdk)
    if(TARGET umicom)
        umicom_add_comfy_node_pages_command_tests(umicom framework.media.comfy.node_pages_command)
    endif()
    umicom_apply_application_branding(TARGET umicom-comfy-node-pages-example-test
        PRODUCT_NAME "Umicom Node Browser Example" INTERNAL_NAME "umicom-comfy-node-pages-example"
        APPLICATION_ID "org.umicom.comfy-node-pages-example"
        RESOURCE_ROOT "${_node_pages_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${_node_pages_root}/examples/comfy_node_pages"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples"
        COMPONENT "${_component}" ${_selection})
    install(FILES
        "${_node_pages_root}/tests/media_generation/comfy_node_pages_fixture.cmake"
        "${_node_pages_root}/tests/media_generation/check_comfy_node_pages_command.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/tests/media_generation"
        COMPONENT "${_component}" ${_selection})
endforeach()
unset(_node_pages_root)
