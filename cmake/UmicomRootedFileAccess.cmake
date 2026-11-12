#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRootedFileAccess.cmake
#
# PURPOSE:
#   Attach rooted file access and portable path qualification to the native platform.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# Rooted file access belongs to the existing shared native platform owner.
include_guard(GLOBAL)
target_sources(umicom_platform PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/platform/rooted_files.c")

if(BUILD_TESTING)
    add_executable(umicom-rooted-files-test "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_rooted_files.c")
    target_link_libraries(umicom-rooted-files-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-rooted-files-test)
    umicom_apply_sanitizers(umicom-rooted-files-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-rooted-files-test)
    endif()
    foreach(case roundtrip replace unicode empty limit missing missing-parent invalid invalid-write directory remove staging staging-collision staging-name locked large leaf-link parent-link root-link readonly permissions hardlink)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-files/${case}")
        add_test(NAME framework.filesystem.rooted.${case} COMMAND umicom-rooted-files-test ${case})
        set_tests_properties(framework.filesystem.rooted.${case} PROPERTIES TIMEOUT 60 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-files/${case}"
            LABELS "framework;platform;filesystem;workspace;regression")
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomCodingLocalFiles.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/reviewed-workspace-files.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)

# Exclusive practice copies retain the same parent ownership as rooted edits.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomRootedFileCreation.cmake")

# Pure lexical admission is qualified without creating filesystem fixtures.
if(BUILD_TESTING)
    add_executable(umicom-relative-path-relation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_relative_path_relation.c")
    target_link_libraries(umicom-relative-path-relation-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-relative-path-relation-test)
    umicom_apply_sanitizers(umicom-relative-path-relation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-relative-path-relation-test)
    endif()
    foreach(case equal ascii-case separator first-parent second-parent nested-parent boundary siblings extension unicode-bytes unicode-not-folded empty absolute drive traversal dot double-separator trailing-separator trailing-dot trailing-space reserved git stream control null capacity)
        add_test(NAME framework.filesystem.relative-path.${case}
            COMMAND umicom-relative-path-relation-test ${case})
        set_tests_properties(framework.filesystem.relative-path.${case}
            PROPERTIES TIMEOUT 10 LABELS "framework;platform;paths;regression")
    endforeach()
endif()

# Each native directory case owns a fresh retained fixture for diagnosis.
if(BUILD_TESTING)
    add_executable(umicom-rooted-directories-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/platform/test_rooted_directories.c")
    target_link_libraries(umicom-rooted-directories-test PRIVATE Umicom::platform)
    umicom_apply_warnings(umicom-rooted-directories-test)
    umicom_apply_sanitizers(umicom-rooted-directories-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-rooted-directories-test)
    endif()
    foreach(case one nested existing mixed separators unicode write-child arguments missing-root file-root file-component git reserved traversal dot trailing-dot trailing-space empty-component capacity partial-progress leaf-link parent-link root-link root-link-slash create-one create-nested create-mixed create-existing create-file create-link create-parent-link create-invalid create-arguments create-missing-root create-unicode create-write-child)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-directories/${case}")
        add_test(NAME framework.filesystem.rooted-directory.${case}
            COMMAND umicom-rooted-directories-test ${case})
        set_tests_properties(framework.filesystem.rooted-directory.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/rooted-directories/${case}"
            LABELS "framework;platform;filesystem;workspace;regression")
    endforeach()
endif()
