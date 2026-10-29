#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningPacks.cmake
#
# PURPOSE:
#   Attach versioned offline learning packs and their focused regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT TARGET umicom_developer)
    message(FATAL_ERROR "Learning packs require the Framework developer target")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/UmicomJsonTree.cmake")
get_filename_component(_umicom_pack_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_developer PRIVATE
    "${_umicom_pack_root}/src/teacher/learning_pack.c"
    "${_umicom_pack_root}/src/teacher/learning_pack_files.c"
    "${_umicom_pack_root}/src/teacher/learning_shelf.c"
    "${_umicom_pack_root}/src/teacher/learning_navigation.c"
    "${_umicom_pack_root}/src/teacher/learning_reader.c"
    "${_umicom_pack_root}/src/teacher/learning_practice.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_developer
        FILES "${_umicom_pack_root}/src/teacher/learning_pack_internal.h")
endif()
if(BUILD_TESTING)
    add_executable(umicom-learning-pack-manifest-test
        "${_umicom_pack_root}/tests/learning_pack/test_manifest.c")
    add_executable(umicom-learning-pack-files-test
        "${_umicom_pack_root}/tests/learning_pack/test_files.c")
    add_executable(umicom-learning-pack-navigation-test
        "${_umicom_pack_root}/tests/learning_pack/test_navigation.c")
    add_executable(umicom-learning-pack-reader-test
        "${_umicom_pack_root}/tests/learning_pack/test_reader.c")
    add_executable(umicom-learning-pack-practice-test
        "${_umicom_pack_root}/tests/learning_pack/test_practice.c")
    foreach(_target umicom-learning-pack-manifest-test umicom-learning-pack-files-test
            umicom-learning-pack-navigation-test umicom-learning-pack-reader-test umicom-learning-pack-practice-test)
        set_target_properties("${_target}" PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries("${_target}" PRIVATE Umicom::developer)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
    endforeach()
    foreach(_case schema version duplicate-key escaped-duplicate unknown-field
            unknown-resource duplicate-resource missing-example wrong-example-kind
            self-reference traversal absolute backslash-traversal device invalid-hash
            negative-revision fractional-version reversed-range overflow-version locale
            title-control title-type invalid-id resources-type trailing-json
            incompatible-low incompatible-high invalid-arguments owned-metadata shelf
            minimum-version maximum-version embedded-nul)
        add_test(NAME "framework.learning_pack.manifest.${_case}"
            COMMAND umicom-learning-pack-manifest-test "${_case}" "${_umicom_pack_root}")
        set_tests_properties("framework.learning_pack.manifest.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.manifest.${_case}"
                TARGETS umicom-learning-pack-manifest-test)
        endif()
    endforeach()
    foreach(_case sibling nested-image nested-example fragment same-document empty-link
            empty-fragment unknown-current undeclared parent dot absolute network drive
            https script encoded query backslash control anchor-space anchor-escape
            two-fragments git-directory device path path-slashes ambiguous ambiguous-path
            long-anchor long-link invalid-arguments)
        add_test(NAME "framework.learning_pack.navigation.${_case}"
            COMMAND umicom-learning-pack-navigation-test "${_case}" "${_umicom_pack_root}")
        set_tests_properties("framework.learning_pack.navigation.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.navigation.${_case}"
                TARGETS umicom-learning-pack-navigation-test)
        endif()
    endforeach()
    foreach(_case invalid-create empty bounded copy binary invalid-link missing-id
            changed-reload history-limit changed-back missing-back branch history same-selection)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-pack-reader/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.reader.${_case}"
            COMMAND umicom-learning-pack-reader-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.reader.${_case}" PROPERTIES
            TIMEOUT 30 WORKING_DIRECTORY "${_directory}" LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.reader.${_case}"
                TARGETS umicom-learning-pack-reader-test)
        endif()
    endforeach()
    foreach(_case resolve copy lesson empty existing same-source hash-mismatch
            missing-source limit missing-id no-example path-refusal missing-parent arguments)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-pack-practice/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.practice.${_case}"
            COMMAND umicom-learning-pack-practice-test "${_case}")
        set_tests_properties("framework.learning_pack.practice.${_case}" PROPERTIES
            TIMEOUT 30 WORKING_DIRECTORY "${_directory}" LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.practice.${_case}"
                TARGETS umicom-learning-pack-practice-test)
        endif()
    endforeach()
    foreach(_case read binary changed-content bounded missing-file missing-id two-packs)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-pack-files/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.files.${_case}"
            COMMAND umicom-learning-pack-files-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.files.${_case}" PROPERTIES
            TIMEOUT 30 WORKING_DIRECTORY "${_directory}" LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.files.${_case}"
                TARGETS umicom-learning-pack-files-test)
        endif()
    endforeach()
endif()

# Keep the complete consumer example together so it also works after SDK
# installation. Its configure step hashes the copied guide and C source.
install(FILES
    "${_umicom_pack_root}/examples/learning_pack_inspector/CMakeLists.txt"
    "${_umicom_pack_root}/examples/learning_pack_inspector/main.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/history.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/practice.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/check_practice.cmake"
    "${_umicom_pack_root}/examples/learning_pack_inspector/guide.html"
    "${_umicom_pack_root}/examples/learning_pack_inspector/learning-pack.json.in"
    DESTINATION "share/umicom/framework/examples/learning_pack_inspector"
    COMPONENT FrameworkSDK)
