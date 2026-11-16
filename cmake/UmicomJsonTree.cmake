# Umicom Framework
# File: cmake/UmicomJsonTree.cmake
#
# PURPOSE:
# Register owned JSON regression coverage while preserving developer consumers.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

# The owned JSON service shares the existing developer parser/text owner.
include_guard(GLOBAL)
# The former developer ownership is retained for review. The protocol archive
# now owns the same implementation, and developer consumers retain link access.
if(FALSE)
    target_sources(umicom_developer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree.c")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomProtocolJsonOwnership.cmake")
target_link_libraries(umicom_developer PRIVATE Umicom::protocol)
if(BUILD_TESTING)
    add_executable(umicom-json-tree-test "${CMAKE_CURRENT_LIST_DIR}/../tests/json_tree/test_tree.c")
    target_link_libraries(umicom-json-tree-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-json-tree-test)
    umicom_apply_sanitizers(umicom-json-tree-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-json-tree-test)
    endif()
    foreach(case null ownership grammar unicode integers booleans bounds depth cancellation members siblings span)
        add_test(NAME framework.json_tree.${case} COMMAND umicom-json-tree-test ${case})
        set_tests_properties(framework.json_tree.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;json;ownership;regression")
    endforeach()
endif()


# Typed decimal observation shares the existing protocol owner and base converter.
if(BUILD_TESTING)
    add_executable(umicom-json-double-test "${CMAKE_CURRENT_LIST_DIR}/../tests/json_tree/test_double.c")
    add_executable(umicom-json-numbers-example-test "${CMAKE_CURRENT_LIST_DIR}/../examples/json_numbers/main.c")
    foreach(_target umicom-json-double-test umicom-json-numbers-example-test)
        target_link_libraries(${_target} PRIVATE Umicom::protocol Umicom::base)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${CMAKE_CURRENT_LIST_DIR}/../src/language_runtime/json_tree.c"
                "${CMAKE_CURRENT_LIST_DIR}/../src/base/ascii_number.c")
        endif()
    endforeach()
    foreach(mode decimal exponent integer-spelling negative-zero largest normal-minimum subnormal overflow underflow negative-overflow wrong-primitives wrong-containers string-number invalid-handles null-output spelling-limit spelling-too-long owned-span member-range exact-integer-preserved comparison-preserved json-grammar locale)
        add_test(NAME framework.json_tree.double.${mode} COMMAND umicom-json-double-test "${mode}")
        set_tests_properties(framework.json_tree.double.${mode} PROPERTIES
            TIMEOUT 30 LABELS "framework;json;numeric;regression" SKIP_RETURN_CODE 77)
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.json_tree.double.${mode} TARGETS umicom-json-double-test)
        endif()
    endforeach()
    add_test(NAME framework.json_tree.double.sdk-example COMMAND umicom-json-numbers-example-test)
    set_tests_properties(framework.json_tree.double.sdk-example PROPERTIES TIMEOUT 20 LABELS "framework;json;sdk")
    umicom_apply_application_branding(TARGET umicom-json-numbers-example-test
        PRODUCT_NAME "Umicom Numeric Settings Example" INTERNAL_NAME "umicom-json-numbers-example"
        APPLICATION_ID "org.umicom.json-numbers-example"
        RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../resources" NO_RUNTIME_DEPLOYMENT)
endif()
foreach(_component Framework FrameworkSDK FrameworkDocumentation)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../examples/json_numbers"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples"
        COMPONENT "${_component}" ${_selection})
endforeach()
