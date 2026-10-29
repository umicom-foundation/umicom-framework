#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomLearningText.cmake
#
# PURPOSE:
#   Compose verified body search, explicit SDK dependencies and focused regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_learning_text_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
foreach(_dependency umicom_developer umicom_browser umicom_editor)
    if(NOT TARGET "${_dependency}")
        message(FATAL_ERROR "Learning text search requires ${_dependency}")
    endif()
endforeach()
target_sources(umicom_developer PRIVATE
    "${_learning_text_root}/src/teacher/learning_text_index.c"
    "${_learning_text_root}/src/teacher/learning_text_search.c")
# Static SDK consumers need both engines even though the public API exposes
# only learning records. Keep this dependency in the exported target graph.
target_link_libraries(umicom_developer PRIVATE Umicom::browser Umicom::editor)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_developer FILES
        "${_learning_text_root}/src/teacher/learning_text_internal.h"
        "${_learning_text_root}/src/base/utf8_text_private.h")
endif()
install(FILES "${_learning_text_root}/include/umicom/teacher/learning_text.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/teacher" COMPONENT FrameworkSDK)
if(BUILD_TESTING)
    set(_learning_text_index_cases default empty-shelf binary utf8 control malformed-html
        empty-documents html-case source-limit document-limit total-limit total-exact
        count-limit count-exact maximum-document oversize-document changed missing snapshot
        failed-replacement cancel-before cancel-middle cancel-after arguments outputs two-packs)
    set(_learning_text_search_cases default order projection-offset source-offset lifetime outputs
        arguments reopen-changed reopen-verified preview-utf8 preview-newline unicode unicode-case
        literal hidden-script hidden-head metadata-only case-sensitive whole-word substring product
        product-case locale locale-no-fallback example image truncated missing empty-query query-limit
        unterminated product-unterminated locale-unterminated query-control query-utf8 zero-results
        too-many-results bad-kind bad-case bad-word cancel-before cancel-middle cancel-after two-packs)
    set(_learning_text_faults_cases owner metadata editor reading first-document later-document)
    set(_learning_text_query_cases default null empty phrase-capacity product-capacity locale-capacity
        phrase-utf8 product-utf8 locale-utf8 control del zero-results many-results kind case word
        callback-not-called maximum locale unicode image)
    foreach(_kind index search faults query)
        set(_target "umicom-learning-text-${_kind}-test")
        add_executable("${_target}" "${_learning_text_root}/tests/learning_pack/test_text_${_kind}.c")
        set_target_properties("${_target}" PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        target_link_libraries("${_target}" PRIVATE Umicom::developer Umicom::browser)
        umicom_apply_warnings("${_target}")
        umicom_apply_sanitizers("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("${_target}" FILES
                "${_learning_text_root}/tests/learning_pack/text_fixture.h")
        endif()
        if(_kind STREQUAL "faults" AND COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs("${_target}" FILES
                "${_learning_text_root}/src/teacher/learning_text_index.c"
                "${_learning_text_root}/src/teacher/learning_text_internal.h"
                "${_learning_text_root}/src/base/utf8_text_private.h")
        endif()
        foreach(_case IN LISTS _learning_text_${_kind}_cases)
            set(_directory "${CMAKE_CURRENT_BINARY_DIR}/learning-text-${_kind}/${_case}")
            file(MAKE_DIRECTORY "${_directory}")
            add_test(NAME "framework.learning_pack.text_${_kind}.${_case}"
                COMMAND "${_target}" "${_case}" "${_directory}")
            set_tests_properties("framework.learning_pack.text_${_kind}.${_case}" PROPERTIES
                TIMEOUT 30 WORKING_DIRECTORY "${_directory}"
                LABELS "framework;learning;search;ownership;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners("framework.learning_pack.text_${_kind}.${_case}" TARGETS "${_target}")
            endif()
        endforeach()
    endforeach()
endif()
