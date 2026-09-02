#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDataWorkbenchPlatform.cmake
#
# PURPOSE:
#   Compose canonical Database Explorer and enterprise data capabilities into
#   the professional Database, SQL and data-modelling workbench.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_DATA_WORKBENCH_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_data)
    message(FATAL_ERROR
        "UmicomDataWorkbenchPlatform.cmake requires the canonical umicom_data target")
endif()

target_sources(umicom_data PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/connection_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/navigator_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/sql_document.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/result_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/query_builder.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/schema_compare.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/er_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/data_editor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/transfer_workbench.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/admin_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/command_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/data/workbench/runtime.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add data workbench test build helper so parent and application projects apply
    # one consistent rule.
    function(umicom_add_data_workbench_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_DATA_WORKBENCH_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::data)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties(
            "${test_name}"
            PROPERTIES LABELS "framework;data-workbench;database;sql;modelling"
        )
    endfunction()

    umicom_add_data_workbench_test(
        umicom-data-workbench-types-test
        framework.data_workbench.types
        tests/data_workbench/test_types.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-connection-model-test
        framework.data_workbench.connection.model
        tests/data_workbench/test_connection_model.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-navigator-model-test
        framework.data_workbench.navigator.model
        tests/data_workbench/test_navigator_model.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-sql-document-test
        framework.data_workbench.sql.document
        tests/data_workbench/test_sql_document.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-result-model-test
        framework.data_workbench.result.model
        tests/data_workbench/test_result_model.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-query-builder-test
        framework.data_workbench.query.builder
        tests/data_workbench/test_query_builder.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-schema-compare-test
        framework.data_workbench.schema.compare
        tests/data_workbench/test_schema_compare.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-er-model-test
        framework.data_workbench.er.model
        tests/data_workbench/test_er_model.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-data-editor-test
        framework.data_workbench.data.editor
        tests/data_workbench/test_data_editor.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-transfer-test
        framework.data_workbench.transfer
        tests/data_workbench/test_transfer_workbench.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-admin-model-test
        framework.data_workbench.admin.model
        tests/data_workbench/test_admin_model.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-command-catalogue-test
        framework.data_workbench.command.catalogue
        tests/data_workbench/test_command_catalogue.c)
    umicom_add_data_workbench_test(
        umicom-data-workbench-runtime-test
        framework.data_workbench.runtime
        tests/data_workbench/test_runtime.c)
endif()
