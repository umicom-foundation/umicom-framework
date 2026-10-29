#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningHelpGtk4.cmake
#
# PURPOSE:
#   Compose native contextual-help policy and focused regression fixtures.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------

# Native help bindings are a projection of the existing immutable route model.
# No application-specific command text or private runtime context enters it.
if(NOT TARGET umicom_learning_catalogue_gtk4)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_learning_help_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_learning_catalogue_gtk4 PRIVATE
    "${_learning_help_root}/adapters/gtk4/learning_help_binding.c")
install(FILES "${_learning_help_root}/include/umicom/ui/gtk4/learning_help_binding.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-learning-help-binding-gtk4-test
        "${_learning_help_root}/tests/learning_pack/test_help_binding_gtk4.c")
    set_target_properties(umicom-learning-help-binding-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-help-binding-gtk4-test PRIVATE Umicom::learning_catalogue_gtk4)
    umicom_apply_warnings(umicom-learning-help-binding-gtk4-test)
    umicom_apply_sanitizers(umicom-learning-help-binding-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-help-binding-gtk4-test)
    endif()
    foreach(_case arguments explicit copied-policy f1 modified-f1 unrelated-key missing-pack
            missing-topic locale busy retired dead-destination nearest unbind destroy-context
            reentrant unbind-observer retire-observer)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-help-binding/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.native_help.${_case}"
            COMMAND umicom-learning-help-binding-gtk4-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.native_help.${_case}" PROPERTIES
            TIMEOUT 60 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;gtk4;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.native_help.${_case}"
                TARGETS umicom-learning-help-binding-gtk4-test)
        endif()
    endforeach()
endif()
