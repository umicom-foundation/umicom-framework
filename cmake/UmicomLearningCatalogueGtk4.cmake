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
    "${_learning_catalogue_root}/adapters/gtk4/learning_catalogue_window.c")
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
            binary retire-idle retire-pending destroy-pending reentrant-busy invalid-query-load)
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
