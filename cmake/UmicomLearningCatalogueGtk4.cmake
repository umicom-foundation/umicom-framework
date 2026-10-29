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
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_reading.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_find.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_images.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_replacement.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_progress.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_libraries.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_window.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_practice.c"
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_selection.c")
add_library(Umicom::learning_catalogue_gtk4 ALIAS umicom_learning_catalogue_gtk4)
set_target_properties(umicom_learning_catalogue_gtk4 PROPERTIES
    EXPORT_NAME learning_catalogue_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_learning_catalogue_gtk4
    PUBLIC Umicom::ui_gtk4 Umicom::developer
    PRIVATE Umicom::browser Umicom::platform Umicom::editor)
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
            copy-path-dot copy-path-parent copy-path-empty copy-path-trailing copy-path-relative
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

# Candidate replacement is reviewed separately from current reader state.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-replacement-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_replacement_gtk4.c")
    set_target_properties(umicom-learning-catalogue-replacement-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-replacement-test PRIVATE
        Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-replacement-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-replacement-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-replacement-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-replacement-test FILES
            "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_gtk4.c"
            "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_private.h")
    endif()
    foreach(_case arguments retired dispatch-retirement native-controls busy destroy-pending
            retire-pending missing-id product-change locale-change same-revision discard
            failed-prepare-keeps-review stale-shelf superseded-review search-allocation
            query-refusal accept-retirement accept reader-kept changed-resource)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-native-replacement/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_replacement.${_case}"
            COMMAND umicom-learning-catalogue-replacement-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_replacement.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_replacement.${_case}"
                TARGETS umicom-learning-catalogue-replacement-test)
        endif()
    endforeach()
endif()

# Retry optional checkpoint composition after the native target exists.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningProgressGtk4.cmake")

# Expander setup is part of these native fixtures and their affected-test inputs.
if(BUILD_TESTING AND COMMAND umicom_target_qualification_inputs)
    foreach(_target umicom-learning-catalogue-gtk4-test umicom-learning-catalogue-replacement-test)
        umicom_target_qualification_inputs("${_target}" FILES
            "${_learning_catalogue_root}/tests/support/gtk4_expanders.h")
    endforeach()
endif()

# Saved-library jobs share the catalogue lifecycle but never adopt execution trust.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-libraries-gtk4-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_libraries_gtk4.c")
    set_target_properties(umicom-learning-catalogue-libraries-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-libraries-gtk4-test PRIVATE Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-libraries-gtk4-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-libraries-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-libraries-gtk4-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-libraries-gtk4-test FILES
            "${_learning_catalogue_root}/tests/learning_pack/library_choices_fixture.h"
            "${_learning_catalogue_root}/tests/support/gtk4_expanders.h")
    endif()
    foreach(_case controls no-selection remember forget duplicate arguments busy
            destroy-pending retire-pending captured-path existing-save reader-retained
            duplicate-shelf invalid-settings missing-settings reentrant load-only
            changed-identity changed-revision invalid-query reopen dispatch-retirement invalid-load-preserves)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-catalogue-libraries/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_libraries.${_case}"
            COMMAND umicom-learning-catalogue-libraries-gtk4-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_libraries.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_libraries.${_case}"
                TARGETS umicom-learning-catalogue-libraries-gtk4-test)
        endif()
    endforeach()
endif()

# Public reading metadata uses the shared browser contract.
target_link_libraries(umicom_learning_catalogue_gtk4 PUBLIC Umicom::browser)

# Structured reading remains optional with the native catalogue, while its core
# model is independently usable through the browser SDK component.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-reading-gtk4-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_reading_gtk4.c")
    set_target_properties(umicom-learning-catalogue-reading-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-reading-gtk4-test PRIVATE Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-reading-gtk4-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-reading-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-reading-gtk4-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-reading-gtk4-test FILES
            "${_learning_catalogue_root}/tests/support/gtk4_expanders.h")
    endif()
    foreach(_case empty formatted unicode controls missing-anchor link-anchor link-source link-control history remote invalid-index fallback changed-resource zoom zoom-controls independent-zoom retired missing-requested-anchor busy destroy-pending reentrant retire-publication retire-tag-creation)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-catalogue-reading/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_reading.${_case}"
            COMMAND umicom-learning-catalogue-reading-gtk4-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_reading.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_reading.${_case}"
                TARGETS umicom-learning-catalogue-reading-gtk4-test)
        endif()
    endforeach()
endif()

# Contextual keyboard and explicit Help actions share one native route binding.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningHelpGtk4.cmake")

# Retry the optional illustration connection after this owner exists.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningPngGtk4.cmake")

# Explicit image selections share Media's optional bounded PNG codec.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-images-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_images_gtk4.c")
    set_target_properties(umicom-learning-catalogue-images-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-images-test PRIVATE Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-images-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-images-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-images-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-images-test FILES
            "${_learning_catalogue_root}/tests/media/png_fixture.h"
            "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_private.h")
    endif()
    foreach(_case image rgba-bytes rgb changed history linked-image bad-crc oversize
        unsupported retire-pending destroy-pending retire-publication arguments)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-catalogue-images/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_images.${_case}"
            COMMAND umicom-learning-catalogue-images-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_images.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;media;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_images.${_case}"
                TARGETS umicom-learning-catalogue-images-test)
        endif()
    endforeach()
endif()

# Guide navigation uses the same literal matcher as document editors. This
# fixture exercises native selection ownership rather than duplicating core tests.
if(BUILD_TESTING)
    add_executable(umicom-learning-catalogue-find-test
        "${_learning_catalogue_root}/tests/learning_pack/test_catalogue_find_gtk4.c")
    set_target_properties(umicom-learning-catalogue-find-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-catalogue-find-test PRIVATE Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-catalogue-find-test)
    umicom_apply_sanitizers(umicom-learning-catalogue-find-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-catalogue-find-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-catalogue-find-test FILES
            "${_learning_catalogue_root}/tests/support/gtk4_expanders.h")
    endif()
    foreach(_case empty next previous wrapped ascii-insensitive case-sensitive unicode
            nonascii-case no-match hidden-content invalid-query capacity busy retired history
            controls enter key modified-key reentrant retire-selection notice-reentrant retained)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-catalogue-find/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_find.${_case}"
            COMMAND umicom-learning-catalogue-find-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_find.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;editor;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_find.${_case}"
                TARGETS umicom-learning-catalogue-find-test)
        endif()
    endforeach()
endif()
