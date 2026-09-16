#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDocumentSaveQualification.cmake
# PURPOSE: Check document identity and Save As conflicts using the real services.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-document-save-target-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_save_target.c")
target_link_libraries(umicom-document-save-target-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-save-target-test)
umicom_apply_sanitizers(umicom-document-save-target-test)
foreach(test_case IN ITEMS captured-document closed-document occupied-destination
        external-conflict normalised-conflict invalid-path)
    set(name "framework.document.save_target.${test_case}")
    add_test(NAME "${name}" COMMAND umicom-document-save-target-test "${test_case}")
    set_tests_properties("${name}" PROPERTIES TIMEOUT 30 LABELS "framework;document;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-save-target-test)
endif()
