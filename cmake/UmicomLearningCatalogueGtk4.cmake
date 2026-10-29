#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningCatalogueGtk4.cmake
#
# PURPOSE:
#   Compose the optional native catalogue with its shared learning services.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# Do not consume the guard before optional owners exist: another host may retry.
if(NOT TARGET Umicom::ui_gtk4 OR NOT TARGET Umicom::developer OR NOT TARGET Umicom::browser)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomPathPicker.cmake")
get_filename_component(_learning_catalogue_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_learning_catalogue_gtk4 STATIC
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_gtk4.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_jobs.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_window.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_practice.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_selection.c")
add_library(Umicom::learning_catalogue_gtk4 ALIAS umicom_learning_catalogue_gtk4)
set_target_properties(umicom_learning_catalogue_gtk4 PROPERTIES
    EXPORT_NAME learning_catalogue_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_learning_catalogue_gtk4
    PUBLIC Umicom::ui_gtk4 Umicom::developer
    PRIVATE Umicom::browser Umicom::platform)
umicom_apply_warnings(umicom_learning_catalogue_gtk4)
umicom_apply_sanitizers(umicom_learning_catalogue_gtk4)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_learning_catalogue_gtk4 FILES
        "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_private.h")
endif()
install(TARGETS umicom_learning_catalogue_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_learning_catalogue_gtk4)
install(FILES "${_learning_catalogue_root}/include/umicom/ui/gtk4/learning_catalogue.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-gtk4-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_gtk4.c")
    set_target_properties(umicom-learning-catalogue-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-gtk4-test PRIVATE Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-gtk4-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-gtk4-test)
    endif()
    foreach(_case controls load duplicate missing-pack search open history changed-content
            binary retire-idle retire-pending destroy-pending reentrant-busy invalid-query-load
            copy copy-existing copy-changed copy-no-example copy-retired copy-reentrant
            copy-arguments copy-destination-captured
            help-open help-missing help-wrong-kind help-changed help-retired help-reentrant)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-catalogue/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.gtk4.${_case}"
            COMMAND umicom-learning-catalogue-gtk4-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.gtk4.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.gtk4.${_case}"
                TARGETS umicom-learning-catalogue-gtk4-test)
        endif()
    endforeach()
endif()

# A real GtkApplication run verifies that closing its last window cannot strand
# accepted asynchronous reads. These cases are distinct from panel-only tests.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-application-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_application_gtk4.c")
    set_target_properties(umicom-learning-catalogue-application-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-application-test PRIVATE
        Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-application-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-application-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-application-test)
    endif()
    foreach(_case close-idle close-pending parent-pending application-hold copy-application-hold)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-window/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.window.${_case}"
            COMMAND umicom-learning-catalogue-application-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.window.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;shutdown;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.window.${_case}"
                TARGETS umicom-learning-catalogue-application-test)
        endif()
    endforeach()
endif()

# Reusing a learning window is separate from application shutdown: preserve
# the loaded reader and reject reentrant presentation or a destroyed parent.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-shared-window-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_shared_window_gtk4.c")
    set_target_properties(umicom-learning-catalogue-shared-window-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-shared-window-test PRIVATE
        Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-shared-window-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-shared-window-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-shared-window-test)
    endif()
    foreach(_case reuse pending-reuse parent-separation host-version independent-present
            closed-retained retired-replacement destroyed-parent
            callback-child callback-parent callback-reentrant)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-shared-window/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.shared_window.${_case}"
            COMMAND umicom-learning-catalogue-shared-window-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.shared_window.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.shared_window.${_case}"
                TARGETS umicom-learning-catalogue-shared-window-test)
        endif()
    endforeach()
endif()

# Native navigation shares verified reader ownership. Presentation failure
# injection compiles the real worker with narrow allocation substitutions.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-navigation-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_navigation_gtk4.c")
    add_executable(umicom-learning-catalogue-presentation-failure-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_presentation_failure_gtk4.c")
    foreach(_target umicom-learning-catalogue-navigation-test
            umicom-learning-catalogue-presentation-failure-test)
        set_target_properties("${_target}" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries("${_target}" PRIVATE Umicom::learning_catalogue_gtk4)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-presentation-failure-test FILES
            "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_jobs.c"
            "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_private.h")
    endif()
    foreach(_case follow anchor empty-link refused-links copied-input follow-changed
            example example-current example-missing example-changed retired busy
            follow-retire-callback example-retire-callback native-controls)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-native-navigation/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_navigation.${_case}"
            COMMAND umicom-learning-catalogue-navigation-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_navigation.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;navigation;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_navigation.${_case}"
                TARGETS umicom-learning-catalogue-navigation-test)
        endif()
    endforeach()
    foreach(_case initial-preview open-preview follow-preview follow-buffer back-preview reload-preview)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-presentation-failure/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.presentation_failure.${_case}"
            COMMAND umicom-learning-catalogue-presentation-failure-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.presentation_failure.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;allocation;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.presentation_failure.${_case}"
                TARGETS umicom-learning-catalogue-presentation-failure-test)
        endif()
    endforeach()
endif()
