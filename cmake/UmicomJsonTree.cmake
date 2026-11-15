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
