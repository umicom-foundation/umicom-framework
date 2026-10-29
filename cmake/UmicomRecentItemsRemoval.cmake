#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRecentItemsRemoval.cmake
#
# PURPOSE:
#   Register transactional recent-history removal and its private source input.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_platform FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/platform/recent_items_remove.inc")
endif()
if(BUILD_TESTING)
    add_executable(umicom-recent-items-remove-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_recent_items_remove.c")
    target_link_libraries(umicom-recent-items-remove-test PRIVATE Umicom::platform)
    set_target_properties(umicom-recent-items-remove-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-recent-items-remove-test)
    umicom_apply_sanitizers(umicom-recent-items-remove-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-recent-items-remove-test)
    endif()
    foreach(_case IN ITEMS first middle last pinned unicode repeat stale other-changed missing
            null-registry null-id empty-id zero-revision null-path relative directory missing-parent)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/recent-remove-${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME framework.recent_items.remove.${_case}
            COMMAND umicom-recent-items-remove-test "${_case}" "${_directory}")
        set_tests_properties(framework.recent_items.remove.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;recent-items;ownership;persistence;regression")
    endforeach()
endif()

# Publish only reviewed teaching inputs; never include build fixtures or local state.
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../examples/recent_history/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/recent_history/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/examples/recent_history"
    COMPONENT FrameworkSDK)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/local-recent-history.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning"
    COMPONENT FrameworkSDK)

# Pinning uses the same local persistence contract as explicit history removal.
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_platform FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/platform/recent_items_pin.inc")
endif()
if(BUILD_TESTING)
    add_executable(umicom-recent-items-pin-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_recent_items_pin.c")
    target_link_libraries(umicom-recent-items-pin-test PRIVATE Umicom::platform)
    set_target_properties(umicom-recent-items-pin-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-recent-items-pin-test)
    umicom_apply_sanitizers(umicom-recent-items-pin-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-recent-items-pin-test)
    endif()
    foreach(_case IN ITEMS pin unpin already-pinned unicode repeat retry stale other-changed negative
            missing null-registry null-id empty-id zero-revision null-path relative directory missing-parent)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/recent-pin-${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME framework.recent_items.pin.${_case}
            COMMAND umicom-recent-items-pin-test "${_case}" "${_directory}")
        set_tests_properties(framework.recent_items.pin.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;recent-items;ownership;persistence;regression")
    endforeach()
endif()

# The uninstalled layout header is used only to seed unreachable-in-practice
# counter boundaries. Public mutation functions still come from the library.
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_platform FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/platform/recent_items_internal.h")
endif()
if(BUILD_TESTING)
    add_executable(umicom-recent-items-revision-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_recent_items_revision.c")
    target_link_libraries(umicom-recent-items-revision-test PRIVATE Umicom::platform)
    set_target_properties(umicom-recent-items-revision-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-recent-items-revision-test)
    umicom_apply_sanitizers(umicom-recent-items-revision-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-recent-items-revision-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-recent-items-revision-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/platform/recent_items_internal.h")
    endif()
    foreach(_case IN ITEMS upsert-new upsert-existing remove-first remove-middle remove-last
            touch touch-final touch-count-saturation pin trim trim-budget trim-kind trim-kind-budget
            trim-one trim-kind-one trim-fit no-op pinned-only kind-empty pin-local remove-local)
        add_test(NAME framework.recent_items.revision.${_case}
            COMMAND umicom-recent-items-revision-test "${_case}" "${CMAKE_CURRENT_BINARY_DIR}")
        set_tests_properties(framework.recent_items.revision.${_case} PROPERTIES
            TIMEOUT 30 LABELS "framework;recent-items;revision;ownership;regression")
    endforeach()
endif()
