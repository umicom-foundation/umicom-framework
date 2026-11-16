# Umicom Framework
# File: cmake/UmicomComfyReferences.cmake
#
# PURPOSE:
#   Register native reference observations and their installed SDK teaching project
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
set(_umicom_reference_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_comfy_protocol PRIVATE "${_umicom_reference_root}/src/media_generation/comfy_references.c")
if(BUILD_TESTING)
    add_executable(umicom-comfy-references-test "${_umicom_reference_root}/tests/media_generation/test_comfy_references.c")
    add_executable(umicom-comfy-references-example-test "${_umicom_reference_root}/examples/comfy_references/main.c")
    foreach(_target umicom-comfy-references-test umicom-comfy-references-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::comfy_protocol)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    foreach(_case empty resolved missing empty-target target-case unknown-class empty-catalogue zero-outputs out-of-range maximum-slot overflow-slot negative-slot negative-zero float-slot exponent-slot boolean-slot null-slot text-slot container-slot independent-unknowns other-arrays self-reference cycle-observation raw-link-metadata unicode escaped-target source-order maximum-candidates too-many-candidates maximum-nodes maximum-name long-name long-target last-output byte-identities owner-lifetime copy-isolation occupied nulls accessor-refusal cancel current-revision catalogue-revision current-settings catalogue-settings)
        add_test(NAME framework.media.comfy.references.${_case} COMMAND umicom-comfy-references-test ${_case})
        set_tests_properties(framework.media.comfy.references.${_case} PROPERTIES
            TIMEOUT 60 LABELS "framework;media;comfy;references;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.media.comfy.references.${_case} TARGETS umicom-comfy-references-test)
        endif()
    endforeach()
    add_test(NAME framework.media.comfy.references.sdk-example COMMAND umicom-comfy-references-example-test)
    set_tests_properties(framework.media.comfy.references.sdk-example PROPERTIES
        TIMEOUT 20 LABELS "framework;media;comfy;sdk")
    umicom_apply_application_branding(TARGET umicom-comfy-references-example-test
        PRODUCT_NAME "Umicom ComfyUI Reference Example" INTERNAL_NAME "umicom-comfy-references-example-test"
        APPLICATION_ID "org.umicom.comfy-references-example"
        RESOURCE_ROOT "${_umicom_reference_root}/resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(FILES "${_umicom_reference_root}/examples/comfy_references/main.c"
        "${_umicom_reference_root}/examples/comfy_references/CMakeLists.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/comfy_references"
        COMPONENT "${_component}" ${_selection})
endforeach()
