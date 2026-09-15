#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningWorkshop.cmake
# PURPOSE: Package the shared lessons and register their native checks.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(UMICOM_LEARNING_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
option(UMICOM_BUILD_LEARNING_EXAMPLES "Build the small C and Assembly lesson exercises" OFF)

# The existing developer target already owns the curriculum and learning plan.
if(BUILD_TESTING)
    add_executable(umicom-teacher-foundations-resources-test
        "${UMICOM_LEARNING_ROOT}/tests/teacher/test_foundations_resources.c")
    target_link_libraries(umicom-teacher-foundations-resources-test PRIVATE umicom_developer)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-teacher-foundations-resources-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-teacher-foundations-resources-test)
    endif()
    add_test(NAME framework.teacher.foundations.resources
        COMMAND umicom-teacher-foundations-resources-test "${UMICOM_LEARNING_ROOT}")
    set_tests_properties(framework.teacher.foundations.resources PROPERTIES
        LABELS "framework;learning;resources;regression")
endif()

if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_subdirectory("${UMICOM_LEARNING_ROOT}/examples/learning"
        "${CMAKE_CURRENT_BINARY_DIR}/umicom-learning")
endif()

# Retain the same relative docs/examples relationship in an installed copy.
install(DIRECTORY "${UMICOM_LEARNING_ROOT}/docs/learning/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/learning"
    COMPONENT Learning FILES_MATCHING PATTERN "*.html" PATTERN "*.md")
install(DIRECTORY "${UMICOM_LEARNING_ROOT}/examples/learning/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/learning"
    COMPONENT Learning
    PATTERN "build" EXCLUDE PATTERN "build-*" EXCLUDE PATTERN ".git" EXCLUDE)
