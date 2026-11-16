# Umicom Framework
# File: cmake/UmicomComfySystem.cmake
#
# PURPOSE:
#   Compose device observations into the existing protocol SDK and register native ownership coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(NOT TARGET umicom_comfy_protocol)
    message(FATAL_ERROR "Comfy system observations require the canonical Comfy protocol library")
endif()
set(_umicom_system_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_system_root}/src/media_generation/comfy_system.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-system-test "${_umicom_system_root}/tests/media_generation/test_comfy_system.c")
    add_executable(umicom-comfy-system-example-test "${_umicom_system_root}/examples/comfy_system/main.c")
    foreach(_target umicom-comfy-system-test umicom-comfy-system-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_system_root}/src/media_generation/comfy_system.c"
                "${_umicom_system_root}/src/media_generation/comfy_response.c"
                "${_umicom_system_root}/src/language_runtime/json_tree.c")
        endif()
    endforeach()
    foreach(mode complete empty unknown-fields null-fields known-zero embedded-true inconsistent free-without-total repeated-devices missing-system missing-devices wrong-system wrong-devices device-type missing-name missing-type empty-name empty-type name-type version-type embedded-type duplicate-key escaped-duplicate opaque-duplicate decoded-nul unpaired trailing wrong-root exact-integer max-integer minus-zero os-boundary os-limit python-boundary python-limit torch-boundary torch-limit version-boundary version-limit name-boundary name-limit type-boundary type-limit numeric-types integer-overflow invalid-utf8 byte-limit exact-byte-limit device-limit maximum-devices depth-limit node-limit nulls zero-revision disabled wrong-provider origin-path cancel occupied ownership copied-output summary-refusal device-refusal connection-revision connection-content identity empty-text)
        add_test(NAME framework.media.comfy.system.${mode} COMMAND umicom-comfy-system-test "${mode}")
        set_tests_properties(framework.media.comfy.system.${mode} PROPERTIES
            TIMEOUT 45 LABELS "framework;media;comfy;system;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.system.${mode} TARGETS umicom-comfy-system-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.system.sdk-example COMMAND umicom-comfy-system-example-test)
    set_tests_properties(framework.media.comfy.system.sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-system-example-test
        PRODUCT_NAME "Umicom System Observation Example" INTERNAL_NAME "umicom-comfy-system-example"
        APPLICATION_ID "org.umicom.comfy-system-example"
        RESOURCE_ROOT "${_umicom_system_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${_umicom_system_root}/examples/comfy_system"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples"
        COMPONENT "${_component}" ${_selection})
    install(FILES "${_umicom_system_root}/docs/learning/comfyui-system-observations-sdk.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning"
        COMPONENT "${_component}" ${_selection})
endforeach()


# File admission, presentation and console commands compose the copied decoder.
target_sources(umicom_comfy_protocol PRIVATE
    "${_umicom_system_root}/src/media_generation/comfy_system_file.c"
    "${_umicom_system_root}/src/media_generation/comfy_system_json.c"
    "${_umicom_system_root}/src/media_generation/comfy_system_command.c")
if(BUILD_TESTING)
    target_link_libraries(umicom-comfy-system-example-test PRIVATE Umicom::platform)
    add_executable(umicom-comfy-system-file-test "${_umicom_system_root}/tests/media_generation/test_comfy_system_file.c")
    add_executable(umicom-comfy-system-json-test "${_umicom_system_root}/tests/media_generation/test_comfy_system_json.c")
    foreach(_target umicom-comfy-system-file-test umicom-comfy-system-json-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol Umicom::platform Umicom::protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_umicom_system_root}/src/media_generation/comfy_system.c"
                "${_umicom_system_root}/src/media_generation/comfy_system_file.c"
                "${_umicom_system_root}/src/media_generation/comfy_system_json.c"
                "${_umicom_system_root}/src/provider_connections/document_file.c")
        endif()
    endforeach()
    foreach(mode complete empty unicode-path disabled malformed-document malformed-system missing-file directory system-limit document-limit empty-file relative-before-read null-document null-system null-output cancel occupied)
        add_test(NAME framework.media.comfy.system_file.${mode} COMMAND "${CMAKE_COMMAND}"
            "-DTEST_EXE=$<TARGET_FILE:umicom-comfy-system-file-test>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/comfy-system-file/${mode}"
            "-DTEST_CASE=${mode}" -P "${_umicom_system_root}/tests/media_generation/check_comfy_system_file.cmake")
        set_tests_properties(framework.media.comfy.system_file.${mode} PROPERTIES
            TIMEOUT 45 LABELS "framework;media;comfy;system;native-file")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.system_file.${mode} TARGETS umicom-comfy-system-file-test)
        endif()
    endforeach()
    foreach(mode summary unknown known-zero null-fields inconsistent free-without-total exact-counters escaping private-values digests exact-capacity short-capacity zero-capacity null-owner null-output null-length maximum-escaped-devices)
        add_test(NAME framework.media.comfy.system_json.${mode} COMMAND umicom-comfy-system-json-test "${mode}")
        set_tests_properties(framework.media.comfy.system_json.${mode} PROPERTIES
            TIMEOUT 30 LABELS "framework;media;comfy;system;json")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.system_json.${mode} TARGETS umicom-comfy-system-json-test)
        endif()
    endforeach()
    umicom_add_comfy_system_command_tests(umicom-comfy-system-example-test framework.media.comfy.system_sdk)
    if(TARGET umicom)
        umicom_add_comfy_system_command_tests(umicom framework.media.comfy.system_command)
    endif()
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES
        "${_umicom_system_root}/tests/media_generation/comfy_system_fixture.cmake"
        "${_umicom_system_root}/tests/media_generation/check_comfy_system_command.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/tests/media_generation"
        COMPONENT "${_component}" ${_selection})
endforeach()

unset(_umicom_system_root)
