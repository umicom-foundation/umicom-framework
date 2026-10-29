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
    "${_umicom_pack_root}/src/teacher/learning_pack_copy.c"
    "${_umicom_pack_root}/src/teacher/learning_pack_changes.c"
    "${_umicom_pack_root}/src/teacher/learning_shelf.c"
    "${_umicom_pack_root}/src/teacher/learning_navigation.c"
    "${_umicom_pack_root}/src/teacher/learning_reader.c"
    "${_umicom_pack_root}/src/teacher/learning_reader_selection.c"
    "${_umicom_pack_root}/src/teacher/learning_practice.c"
    "${_umicom_pack_root}/src/teacher/learning_search.c"
    "${_umicom_pack_root}/src/teacher/learning_help.c")
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
    add_executable(umicom-learning-pack-search-test
        "${_umicom_pack_root}/tests/learning_pack/test_search.c")
    add_executable(umicom-learning-pack-capacity-test
        "${_umicom_pack_root}/tests/learning_pack/test_capacity.c")
    add_executable(umicom-learning-pack-shelf-reader-test
        "${_umicom_pack_root}/tests/learning_pack/test_shelf_reader.c")
    foreach(_target umicom-learning-pack-manifest-test umicom-learning-pack-files-test
            umicom-learning-pack-navigation-test umicom-learning-pack-reader-test umicom-learning-pack-practice-test umicom-learning-pack-search-test umicom-learning-pack-capacity-test umicom-learning-pack-shelf-reader-test)
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
            changed-reload history-limit changed-back missing-back branch history same-selection
            clone-empty clone-history clone-independent clone-source-gone clone-stale-disk
            clone-binary clone-bound clone-invalid clone-pack clone-reader-pack clone-reader-pack-arguments)
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
    foreach(_case browse snapshot removed outputs empty-shelf spaces title terms identity pack-title product locale locale-no-fallback filters kind literal all-terms unicode unicode-case truncated eight-terms many-terms unterminated product-unterminated locale-unterminated control arguments)
        add_test(NAME "framework.learning_pack.search.${_case}"
            COMMAND umicom-learning-pack-search-test "${_case}" "${_umicom_pack_root}")
        set_tests_properties("framework.learning_pack.search.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.search.${_case}"
                TARGETS umicom-learning-pack-search-test)
        endif()
    endforeach()
    foreach(_case valid crlf failed-process timeout wrong-count wrong-id empty truncated prefixed)
        add_test(NAME "framework.learning_pack.search_evidence.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK_ROOT=${_umicom_pack_root}" "-DCASE=${_case}"
                -P "${_umicom_pack_root}/tests/learning_pack/check_search_evidence.cmake")
        set_tests_properties("framework.learning_pack.search_evidence.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.search_evidence.${_case}"
                TARGETS umicom-learning-pack-search-test)
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-pack-search-test FILES
            "${_umicom_pack_root}/tests/learning_pack/check_search_evidence.cmake"
            "${_umicom_pack_root}/examples/learning_pack_inspector/check_search.cmake")
    endif()
    foreach(_case resources-below resources-exact resources-over manifest-exact
            manifest-over shelf search-exact search-truncated max-shelf-search)
        add_test(NAME "framework.learning_pack.capacity.${_case}"
            COMMAND umicom-learning-pack-capacity-test "${_case}" "${_umicom_pack_root}")
        set_tests_properties("framework.learning_pack.capacity.${_case}" PROPERTIES
            TIMEOUT 60 LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.capacity.${_case}"
                TARGETS umicom-learning-pack-capacity-test)
        endif()
    endforeach()
    foreach(_case clone clone-lifetime clone-arguments open reader-lifetime two-readers search-open changed-revision changed-digest removed missing-resource changed-content missing-file bounded empty binary short-hash long-hash upper-hash invalid-hash arguments reader-selection reader-selection-pack reader-selection-revision reader-selection-digest reader-selection-missing reader-selection-bytes reader-selection-arguments)
        set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-pack-selection/${_case}")
        file(MAKE_DIRECTORY "${_directory}")
        add_test(NAME "framework.learning_pack.selection.${_case}"
            COMMAND umicom-learning-pack-shelf-reader-test "${_case}" "${_directory}")
        set_tests_properties("framework.learning_pack.selection.${_case}" PROPERTIES
            TIMEOUT 30 WORKING_DIRECTORY "${_directory}" LABELS "framework;learning;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.selection.${_case}"
                TARGETS umicom-learning-pack-shelf-reader-test)
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
    "${_umicom_pack_root}/examples/learning_pack_inspector/search.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/body_search.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/help.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/progress.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/archive.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/check_help.cmake"
    "${_umicom_pack_root}/examples/learning_pack_inspector/transaction.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/libraries.c"
    "${_umicom_pack_root}/examples/learning_pack_inspector/check_transaction.cmake"
    "${_umicom_pack_root}/examples/learning_pack_inspector/check_practice.cmake"
    "${_umicom_pack_root}/examples/learning_pack_inspector/check_search.cmake"
    "${_umicom_pack_root}/examples/learning_pack_inspector/guide.html"
    "${_umicom_pack_root}/examples/learning_pack_inspector/learning-pack.json.in"
    DESTINATION "share/umicom/framework/examples/learning_pack_inspector"
    COMPONENT FrameworkSDK)

# Native hosts share the same catalogue and verified reader owners.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningCatalogueGtk4.cmake")

# Explicit help identities use current admitted metadata; no file access is needed.
if(BUILD_TESTING)
    add_executable(umicom-learning-pack-help-test
        "${_umicom_pack_root}/tests/learning_pack/test_help.c")
    set_target_properties(umicom-learning-pack-help-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-pack-help-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-learning-pack-help-test)
    umicom_apply_sanitizers(umicom-learning-pack-help-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-pack-help-test)
    endif()
    foreach(_case command diagnostic topic kind-separation product-case key-case locale-case
            locale-no-fallback target-locale missing-route missing-pack missing-resource wrong-kind
            request-invalid outputs snapshot removed-pack replacement arguments duplicate
            duplicate-locale unterminated invalid-identities capacity empty)
        add_test(NAME "framework.learning_pack.help.${_case}"
            COMMAND umicom-learning-pack-help-test "${_case}" "${_umicom_pack_root}")
        set_tests_properties("framework.learning_pack.help.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;help;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.help.${_case}"
                TARGETS umicom-learning-pack-help-test)
        endif()
    endforeach()
endif()

# Progress borrows an explicit Data Server; readers remain usable without storage.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningProgress.cmake")

# Reviewed replacement retains previous readers and rejects stale shelf observations.
if(BUILD_TESTING)
    foreach(_kind replacement faults)
        add_executable("umicom-learning-shelf-${_kind}-test"
            "${_umicom_pack_root}/tests/learning_pack/test_shelf_${_kind}.c")
        set_target_properties("umicom-learning-shelf-${_kind}-test" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries("umicom-learning-shelf-${_kind}-test" PRIVATE Umicom::developer)
        umicom_apply_warnings("umicom-learning-shelf-${_kind}-test")
        umicom_apply_sanitizers("umicom-learning-shelf-${_kind}-test")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-learning-shelf-${_kind}-test")
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("umicom-learning-shelf-${_kind}-test" FILES
                "${_umicom_pack_root}/tests/learning_pack/shelf_fixture.h")
        endif()
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-shelf-faults-test FILES
            "${_umicom_pack_root}/src/teacher/learning_shelf.c"
            "${_umicom_pack_root}/src/teacher/learning_pack_copy.c"
            "${_umicom_pack_root}/src/teacher/learning_pack_internal.h")
    endif()
    set(_shelf_replacement_cases empty-clone arguments duplicate-adopt missing-remove stale future
        same-revision older-revision product-change locale-change missing-id clone-lifetime
        clone-independent full-shelf middle-order replace locale-case reader-kept search-stale)
    set(_shelf_faults_cases clone-allocation replace-no-allocation exhausted-adopt exhausted-remove
        exhausted-replace exhausted-clone)
    foreach(_kind replacement faults)
        foreach(_case IN LISTS _shelf_${_kind}_cases)
            set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-shelf-${_kind}/${_case}")
            file(MAKE_DIRECTORY "${_directory}")
            add_test(NAME "framework.learning_pack.shelf_${_kind}.${_case}"
                COMMAND "umicom-learning-shelf-${_kind}-test" "${_case}" "${_directory}")
            set_tests_properties("framework.learning_pack.shelf_${_kind}.${_case}" PROPERTIES
                TIMEOUT 30 WORKING_DIRECTORY "${_directory}" LABELS "framework;learning;ownership;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.learning_pack.shelf_${_kind}.${_case}"
                    TARGETS "umicom-learning-shelf-${_kind}-test")
            endif()
        endforeach()
    endforeach()
endif()

if(BUILD_TESTING)
    add_executable(umicom-learning-pack-changes-test
        "${_umicom_pack_root}/tests/learning_pack/test_pack_changes.c")
    set_target_properties(umicom-learning-pack-changes-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom-learning-pack-changes-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-learning-pack-changes-test)
    umicom_apply_sanitizers(umicom-learning-pack-changes-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-learning-pack-changes-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-learning-pack-changes-test FILES
            "${_umicom_pack_root}/src/teacher/learning_pack_changes.c"
            "${_umicom_pack_root}/src/teacher/learning_pack_internal.h")
    endif()
    foreach(_case identical empty reordered added removed renamed mixed title topic kind
            path digest example all-fields maximum independent arguments allocation)
        add_test(NAME "framework.learning_pack.changes.${_case}"
            COMMAND umicom-learning-pack-changes-test "${_case}" "${_umicom_pack_root}")
        set_tests_properties("framework.learning_pack.changes.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;learning;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.learning_pack.changes.${_case}"
                TARGETS umicom-learning-pack-changes-test)
        endif()
    endforeach()
endif()

# Remembered roots remain separate from pack admission and execution trust.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningLibraryChoices.cmake")

# Body search extends metadata browsing with explicit verified-resource snapshots.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomLearningText.cmake")
