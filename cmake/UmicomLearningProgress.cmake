#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningProgress.cmake
#
# PURPOSE:
#   Compose explicit learning record storage without coupling it to a GUI.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(NOT TARGET Umicom::developer OR NOT TARGET Umicom::data)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
get_filename_component(_learning_progress_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_learning_progress STATIC
    "${_learning_progress_root}/src/teacher/learning_progress.c"
    "${_learning_progress_root}/src/teacher/learning_progress_storage.c"
    "${_learning_progress_root}/src/teacher/learning_progress_archive.c"
    "${_learning_progress_root}/src/teacher/learning_progress_local.c")
add_library(Umicom::learning_progress ALIAS umicom_learning_progress)
set_target_properties(umicom_learning_progress PROPERTIES
    EXPORT_NAME learning_progress C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_learning_progress PUBLIC Umicom::developer Umicom::data)
umicom_apply_warnings(umicom_learning_progress)
umicom_apply_sanitizers(umicom_learning_progress)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_learning_progress FILES
        "${_learning_progress_root}/src/teacher/learning_progress_internal.h")
endif()
install(TARGETS umicom_learning_progress EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_learning_progress)
install(FILES "${_learning_progress_root}/include/umicom/teacher/learning_progress.h"
    "${_learning_progress_root}/include/umicom/teacher/learning_progress_local.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/teacher" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-learning-pack-progress-test
        "${_learning_progress_root}/tests/learning_pack/test_progress.c")
    set_target_properties(umicom-learning-pack-progress-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-pack-progress-test PRIVATE Umicom::learning_progress)
    umicom_apply_warnings(umicom-learning-pack-progress-test)
    umicom_apply_sanitizers(umicom-learning-pack-progress-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-pack-progress-test)
    endif()
    foreach(_case empty record reopen duplicate latest-evidence changed-revision changed-digest
            changed-resource changed-pack learner-separation checkpoint wrong-kind invalid-arguments
            invalid-tokens transaction-busy stale-writer changed-history refresh-invalid corrupt-head
            missing-event orphan-event unknown-record extra-event malformed-event duplicate-stored-event capacity sqlite readonly)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-pack-progress/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.progress.${_case}"
            COMMAND umicom-learning-pack-progress-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.progress.${_case}" PROPERTIES
            TIMEOUT 90 WORKING_DIRECTORY "${_directory}" SKIP_RETURN_CODE 77
            LABELS "framework;learning;storage;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.progress.${_case}"
                TARGETS umicom-learning-pack-progress-test)
        endif()
    endforeach()
endif()

# Fault tests compile the real journal with local storage call substitutions.
# They deliberately do not link its library a second time.
if(BUILD_TESTING)
    add_executable(umicom-learning-pack-progress-faults-test
        "${_learning_progress_root}/tests/learning_pack/test_progress_faults.c")
    set_target_properties(umicom-learning-pack-progress-faults-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-pack-progress-faults-test PRIVATE Umicom::developer Umicom::data)
    umicom_apply_warnings(umicom-learning-pack-progress-faults-test)
    umicom_apply_sanitizers(umicom-learning-pack-progress-faults-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-pack-progress-faults-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-pack-progress-test FILES
            "${_learning_progress_root}/tests/learning_pack/progress_reader_fixture.h")
        umicom_target_qualification_inputs(umicom-learning-pack-progress-faults-test FILES
            "${_learning_progress_root}/tests/learning_pack/progress_reader_fixture.h"
            "${_learning_progress_root}/src/teacher/learning_progress_internal.h"
            "${_learning_progress_root}/src/teacher/learning_progress.c"
            "${_learning_progress_root}/src/teacher/learning_progress_storage.c")
    endif()
    foreach(_case event-write head-write commit-before commit-after rollback-write rollback-refresh
            import-event-write import-head-write import-commit-before import-commit-after import-rollback)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-progress-faults/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.progress_faults.${_case}"
            COMMAND umicom-learning-pack-progress-faults-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.progress_faults.${_case}" PROPERTIES
            TIMEOUT 45 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;storage;fault-injection;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.progress_faults.${_case}"
                TARGETS umicom-learning-pack-progress-faults-test)
        endif()
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-learning-pack-progress-archive-test
        "${_learning_progress_root}/tests/learning_pack/test_progress_archive.c")
    set_target_properties(umicom-learning-pack-progress-archive-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-pack-progress-archive-test PRIVATE Umicom::learning_progress)
    umicom_apply_warnings(umicom-learning-pack-progress-archive-test)
    umicom_apply_sanitizers(umicom-learning-pack-progress-archive-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-pack-progress-archive-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-pack-progress-archive-test FILES
            "${_learning_progress_root}/tests/learning_pack/progress_reader_fixture.h")
        umicom_target_qualification_inputs(umicom-learning-pack-progress-faults-test FILES
            "${_learning_progress_root}/src/teacher/learning_progress_archive.c")
    endif()
    foreach(_case roundtrip empty crlf short-buffer wrong-learner nonempty stale-nonempty
            transaction-busy version head-count leading-zero missing-newline trailing-data
            embedded-nul malformed-event duplicate-event over-limit arguments changed-revision source-independent)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-progress-archive/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.progress_archive.${_case}"
            COMMAND umicom-learning-pack-progress-archive-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.progress_archive.${_case}" PROPERTIES
            TIMEOUT 45 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;storage;archive;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.progress_archive.${_case}"
                TARGETS umicom-learning-pack-progress-archive-test)
        endif()
    endforeach()
endif()

# Local requests own a fresh SQLite connection for one explicit operation.
if(BUILD_TESTING)
    add_executable(umicom-learning-progress-local-test
        "${_learning_progress_root}/tests/learning_pack/test_progress_local.c")
    set_target_properties(umicom-learning-progress-local-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-progress-local-test PRIVATE Umicom::learning_progress)
    umicom_apply_warnings(umicom-learning-progress-local-test)
    umicom_apply_sanitizers(umicom-learning-progress-local-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-progress-local-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-progress-local-test FILES
            "${_learning_progress_root}/tests/learning_pack/progress_reader_fixture.h")
    endif()
    foreach(_case arguments wrong-kind action relative path-control unterminated-path learner
            checkpoint evidence unterminated-id inspect-evidence read-missing empty-journal
            corrupt-journal duplicate new-evidence reader-snapshot record-reopen revision learner-separation)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-progress-local/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.progress_local.${_case}"
            COMMAND umicom-learning-progress-local-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.progress_local.${_case}" PROPERTIES
            TIMEOUT 90 SKIP_RETURN_CODE 77 WORKING_DIRECTORY "${_directory}"
            LABELS "framework;learning;storage;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.progress_local.${_case}"
                TARGETS umicom-learning-progress-local-test)
        endif()
    endforeach()
endif()

# Preserve the earlier order-dependent binding for review. Both target owners
# now retry one guarded composition module, so either include order is supported.
if(FALSE)
# The catalogue remains usable without persistence. Enable its optional controls
# only after the core progress owner exists; no dependency cycle is introduced.
if(TARGET umicom_learning_catalogue_gtk4)
    target_link_libraries(umicom_learning_catalogue_gtk4 PRIVATE Umicom::learning_progress)
    target_compile_definitions(umicom_learning_catalogue_gtk4 PRIVATE UMICOM_LEARNING_PROGRESS_AVAILABLE=1)
endif()
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningProgressGtk4.cmake")
