/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/uat/database_workflow.c
 *
 * PURPOSE:
 *   Demonstrate UAT setup, insert/update/delete assertions and table cleanup
 *   against a disposable in-memory SQLite database, with structured findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/uat_workflow.h"
#include "umicom/testing/uat_target.h"
#include "umicom/testing/uat_database_rows.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DatabaseFixture {
    sqlite3 *database;
    int failAssertion;
    int failCleanup;
    bool typedRows;
    bool changedTypedValue;
} DatabaseFixture;

/* Exercise a failed transaction deliberately. Require a successful staged
 * edit and the exact SQLite constraint before collecting the rollback.
 * The separate read assertion then checks the resulting table. */
static int RollbackDuplicate(sqlite3 *database)
{
    if (sqlite3_get_autocommit(database) == 0) return SQLITE_MISUSE;
    int code = sqlite3_exec(database, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
    if (code != SQLITE_OK) return code;
    code = sqlite3_exec(database, "UPDATE notes SET text='temporary' WHERE id=1;", NULL, NULL, NULL);
    int expectedRefusal = 0;
    if (code == SQLITE_OK && sqlite3_changes(database) == 1) {
        code = sqlite3_exec(database, "INSERT INTO notes VALUES(1,'duplicate');", NULL, NULL, NULL);
        expectedRefusal = code != SQLITE_OK &&
            sqlite3_extended_errcode(database) == SQLITE_CONSTRAINT_PRIMARYKEY;
    }
    /* Always release the transaction owned by this step, even if the staged
     * mutation or error was unexpected. Never report an implicit close as a pass. */
    const int rollback = sqlite3_exec(database, "ROLLBACK;", NULL, NULL, NULL);
    return expectedRefusal && rollback == SQLITE_OK && sqlite3_get_autocommit(database) != 0
        ? SQLITE_OK : SQLITE_ERROR;
}

/* Capture SQLite's storage kind before requesting a pointer. Its view is
 * borrowed only until the next sqlite3_step or statement finalization. */
static int ObserveCell(sqlite3_stmt *statement, int column, UmiUatDatabaseCell *cell)
{
    const int kind = sqlite3_column_type(statement, column);
    switch (kind) {
    case SQLITE_NULL:
        cell->kind = UMI_DATA_VALUE_NULL;
        return SQLITE_OK;
    case SQLITE_INTEGER:
        cell->kind = UMI_DATA_VALUE_INTEGER;
        cell->value.integer = sqlite3_column_int64(statement, column);
        return SQLITE_OK;
    case SQLITE_FLOAT:
        cell->kind = UMI_DATA_VALUE_REAL;
        cell->value.real = sqlite3_column_double(statement, column);
        return SQLITE_OK;
    case SQLITE_TEXT:
    case SQLITE_BLOB: {
        const unsigned char *bytes = kind == SQLITE_TEXT
            ? sqlite3_column_text(statement, column) : sqlite3_column_blob(statement, column);
        const int length = sqlite3_column_bytes(statement, column);
        if (length < 0 || (bytes == NULL && (kind == SQLITE_TEXT || length != 0 ||
            sqlite3_errcode(sqlite3_db_handle(statement)) == SQLITE_NOMEM)))
            return SQLITE_ERROR;
        cell->kind = kind == SQLITE_TEXT ? UMI_DATA_VALUE_TEXT : UMI_DATA_VALUE_BLOB;
        cell->value.bytes = (UmiUatDatabaseBytes){bytes, (size_t)length};
        return SQLITE_OK;
    }
    default:
        return SQLITE_ERROR;
    }
}

/* Compare while SQLite still owns the first row. Stepping again proves
 * that no additional row remains. The adapter retains no SQLite pointer. */
static int VerifyDatabaseRow(const DatabaseFixture *fixture, UmiUatDatabaseComparison *comparison)
{
    static const unsigned char saved[] = "saved";
    static const unsigned char expected_blob[] = {0U, 1U, 0xFFU};
    const UmiUatDatabaseCell values[] = {
        {.kind = UMI_DATA_VALUE_INTEGER, .value.integer = 1},
        {.kind = UMI_DATA_VALUE_TEXT, .value.bytes = {saved, sizeof(saved) - 1U}},
        {.kind = UMI_DATA_VALUE_BLOB, .value.bytes = {expected_blob, sizeof(expected_blob)}},
        {.kind = UMI_DATA_VALUE_TEXT, .value.bytes = {NULL, 0U}},
        {.kind = UMI_DATA_VALUE_NULL},
        {.kind = UMI_DATA_VALUE_REAL, .value.real = 2.5},
    };
    const char *query = "SELECT id,text FROM notes ORDER BY id;";
    int columns = 2;
    if (fixture->typedRows) {
        columns = 6;
        /* The negative case shares the visible prefix of the expected text
         * but changes its exact length and suffix after a NUL byte. */
        query = fixture->changedTypedValue
            ? "SELECT id,text || char(0) || 'x',X'0001FF',CAST(X'' AS TEXT),NULL,2.5 FROM notes ORDER BY id;"
            : "SELECT id,text,X'0001FF',CAST(X'' AS TEXT),NULL,2.5 FROM notes ORDER BY id;";
    }
    sqlite3_stmt *statement = NULL;
    int code = sqlite3_prepare_v2(fixture->database, query, -1, &statement, NULL);
    if (code == SQLITE_OK && (sqlite3_column_count(statement) != columns ||
        sqlite3_step(statement) != SQLITE_ROW))
        code = SQLITE_ERROR;
    UmiUatDatabaseCell observed[sizeof values / sizeof values[0]] = {0};
    for (int column = 0; code == SQLITE_OK && column < columns; ++column) {
        code = ObserveCell(statement, column, &observed[column]);
    }
    if (code == SQLITE_OK) {
        const size_t count = (size_t)columns;
        const UmiUatDatabaseRows expected = {1U, count, count, values};
        const UmiUatDatabaseRows actual = {1U, count, count, observed};
        if (UmiUatCompareDatabaseRows(&expected, &actual, comparison) != UMI_STATUS_OK ||
            comparison->match != UMI_UAT_DATABASE_EQUAL || fixture->failAssertion ||
            sqlite3_step(statement) != SQLITE_DONE)
            code = SQLITE_ERROR;
    }
    if (sqlite3_finalize(statement) != SQLITE_OK) code = SQLITE_ERROR;
    return code;
}

/* This fixture owns a private :memory: database. No connection string, SQL
 * file or table name is accepted from the command line. A production adapter
 * needs a separately enforced environment and resource-ownership contract. */
static UmiStatus ExecuteDatabase(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result)
{
    DatabaseFixture *fixture = context;
    const char *sql = NULL;
    int code = SQLITE_OK;
    UmiUatDatabaseComparison comparison = {UMI_UAT_DATABASE_EQUAL, SIZE_MAX, SIZE_MAX};
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    if (strcmp(test->test_id, "create") == 0)
        sql = "CREATE TABLE notes(id INTEGER PRIMARY KEY, text TEXT NOT NULL);";
    else if (strcmp(test->test_id, "edit") == 0)
        sql = "INSERT INTO notes VALUES(1,'draft'),(2,'remove');"
              "UPDATE notes SET text='saved' WHERE id=1; DELETE FROM notes WHERE id=2;";
    else if (strcmp(test->test_id, "rollback") == 0)
        code = RollbackDuplicate(fixture->database);
    /* The shared assertion keeps storage kinds and lengths, including data
     * after a NUL. Retain the earlier coercing projection for review. */
#if 0
    else if (strcmp(test->test_id, "verify") == 0) {
        sqlite3_stmt *statement = NULL;
        code = sqlite3_prepare_v2(fixture->database,
            "SELECT id,text FROM notes ORDER BY id;", -1, &statement, NULL);
        if (code == SQLITE_OK) {
            const int first = sqlite3_step(statement);
            const unsigned char *text = first == SQLITE_ROW ? sqlite3_column_text(statement, 1) : NULL;
            const int matches = first == SQLITE_ROW && sqlite3_column_int(statement, 0) == 1 &&
                text != NULL && strcmp((const char *)text, "saved") == 0 && !fixture->failAssertion;
            const int second = sqlite3_step(statement);
            if (!matches || second != SQLITE_DONE) code = SQLITE_ERROR;
        }
        if (sqlite3_finalize(statement) != SQLITE_OK) code = SQLITE_ERROR;
    }
#else
    else if (strcmp(test->test_id, "verify") == 0)
        code = VerifyDatabaseRow(fixture, &comparison);
#endif
    else if (strcmp(test->test_id, "drop") == 0)
        sql = fixture->failCleanup ? "DROP TABLE deliberately_missing;" : "DROP TABLE IF EXISTS notes;";
    else return UMI_STATUS_INVALID_ARGUMENT;
    if (sql != NULL) code = sqlite3_exec(fixture->database, sql, NULL, NULL, NULL);
    /* The row's duration is left at zero: this example does not measure time. */
    umi_test_result_finish(result, code == SQLITE_OK ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE,
        code == SQLITE_OK ? 0 : 1, 0U);
    (void)snprintf(result->output, sizeof(result->output), "%s",
        code == SQLITE_OK ? "Fixture check completed." : "Fixture operation or assertion failed.");
    /* Include the location in typed example evidence without storing either
     * value. Human-readable locations start at one in this report. */
    if (comparison.match == UMI_UAT_DATABASE_VALUES_DIFFER ||
        comparison.match == UMI_UAT_DATABASE_KINDS_DIFFER) {
        (void)snprintf(result->output, sizeof(result->output), "Unexpected database cell at row %zu, column %zu.",
            comparison.row + 1U, comparison.column + 1U);
    }
    return UMI_STATUS_OK;
}

/* The database driver accepts only the operations used by this disposable
 * fixture. Existing SQL and retained findings still belong to the workflow. */
static UmiStatus ExecuteScopedDatabase(void *context, const UmiUatTarget *target,
    UmiUatOperation operation, const UmiTestCase *test, UmiCancellationToken *cancellation,
    UmiTestResult *result)
{
    const UmiUatOperation needed = strcmp(test->test_id, "verify") == 0
        ? UMI_UAT_DATABASE_READ : UMI_UAT_DATABASE_WRITE;
    if (target->platform != UMI_UAT_DESKTOP || strcmp(target->sessionId, "private-memory") != 0 ||
        operation != needed)
        return UMI_STATUS_PERMISSION_DENIED;
    return ExecuteDatabase(context, test, cancellation, result);
}

int main(int argc, char **argv)
{
    /* Retain the previous fixed four-step declaration for review. The current
     * descriptors keep each step's phase and operation together when rollback
     * is selected. Existing invocations still emit the same four findings. */
#if 0
    const char *ids[] = {"create", "edit", "verify", "drop"};
    const UmiUatOperation operations[] = {UMI_UAT_DATABASE_WRITE, UMI_UAT_DATABASE_WRITE,
        UMI_UAT_DATABASE_READ, UMI_UAT_DATABASE_WRITE};
    UmiUatStep steps[4] = {0};
#endif
    const struct {
        const char *id;
        UmiUatPhase phase;
        UmiUatOperation operation;
    } plan[] = {
        {"create", UMI_UAT_SETUP, UMI_UAT_DATABASE_WRITE},
        {"edit", UMI_UAT_ACTION, UMI_UAT_DATABASE_WRITE},
        {"rollback", UMI_UAT_ACTION, UMI_UAT_DATABASE_WRITE},
        {"verify", UMI_UAT_ASSERT, UMI_UAT_DATABASE_READ},
        {"drop", UMI_UAT_CLEANUP, UMI_UAT_DATABASE_WRITE},
    };
    UmiUatStep steps[sizeof plan / sizeof plan[0]] = {0};
    UmiUatOperation operations[sizeof plan / sizeof plan[0]];
    size_t count = 0U;
    UmiUatReport *report = NULL;
    UmiUatSummary summary = {0};
    DatabaseFixture fixture = {0};
    char *json = malloc(16384U);
    int exitCode = 1;
    /* Keep the earlier argument contract for review. The two new modes
     * choose only fixed synthetic projections over the same private table. */
#if 0
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--fail-assertion") != 0 &&
        strcmp(argv[1], "--fail-cleanup") != 0 && strcmp(argv[1], "--verify-rollback") != 0)) {
        fprintf(stderr, "Usage: umicom-uat-database-example [--fail-assertion|--fail-cleanup|--verify-rollback]\n");
        free(json); return 2;
    }
#endif
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--fail-assertion") != 0 &&
        strcmp(argv[1], "--fail-cleanup") != 0 && strcmp(argv[1], "--verify-rollback") != 0 &&
        strcmp(argv[1], "--verify-types") != 0 && strcmp(argv[1], "--reject-typed-change") != 0)) {
        fprintf(stderr, "Usage: umicom-uat-database-example [--fail-assertion|--fail-cleanup|--verify-rollback|--verify-types|--reject-typed-change]\n");
        free(json); return 2;
    }
    fixture.failAssertion = argc == 2 && strcmp(argv[1], "--fail-assertion") == 0;
    fixture.failCleanup = argc == 2 && strcmp(argv[1], "--fail-cleanup") == 0;
    const int verifyRollback = argc == 2 && strcmp(argv[1], "--verify-rollback") == 0;
    /* Both typed outcomes use the same assertion and independent cleanup. */
    fixture.changedTypedValue = argc == 2 && strcmp(argv[1], "--reject-typed-change") == 0;
    fixture.typedRows = fixture.changedTypedValue ||
        (argc == 2 && strcmp(argv[1], "--verify-types") == 0);
    if (json == NULL) return 2;
    if (sqlite3_open(":memory:", &fixture.database) != SQLITE_OK) {
        sqlite3_close(fixture.database); free(json); return 2;
    }
    /* The old positional projection is preserved for review. The optional
     * action now carries its explicit phase and write permission. */
#if 0
    for (size_t i = 0U; i < 4U; ++i) {
        steps[i].phase = i == 0U ? UMI_UAT_SETUP : i == 1U ? UMI_UAT_ACTION :
            i == 2U ? UMI_UAT_ASSERT : UMI_UAT_CLEANUP;
        umi_test_case_init(&steps[i].test, ids[i], ids[i]);
        if (umi_test_case_set_command(&steps[i].test, "sqlite-memory-fixture", "", "") != UMI_STATUS_OK)
            goto finish;
        steps[i].test.kind = UMI_TEST_KIND_INTEGRATION;
        steps[i].test.timeout_ms = 1000U;
    }
#endif
    for (size_t i = 0U; i < sizeof plan / sizeof plan[0]; ++i) {
        if (!verifyRollback && strcmp(plan[i].id, "rollback") == 0) continue;
        UmiUatStep *step = &steps[count];
        step->phase = plan[i].phase;
        operations[count] = plan[i].operation;
        umi_test_case_init(&step->test, plan[i].id, plan[i].id);
        if (umi_test_case_set_command(&step->test, "sqlite-memory-fixture", "", "") != UMI_STATUS_OK)
            goto finish;
        step->test.kind = UMI_TEST_KIND_INTEGRATION;
        step->test.timeout_ms = 1000U;
        ++count;
    }
    /* Retain the original unscoped call for review. The scoped driver reuses
     * that workflow and adds fixture identity and operation admission. */
#if 0
    if (UmiUatRun(steps, 4U, ExecuteDatabase, &fixture, NULL, NULL, &report) != UMI_STATUS_OK)
        goto finish;
#endif
    const UmiUatTarget target = {UMI_UAT_TARGET_PROTOCOL, UMI_UAT_DESKTOP, "sqlite-fixture", "private-memory"};
    const uint32_t permissions = UMI_UAT_DATABASE_READ | UMI_UAT_DATABASE_WRITE;
    const UmiUatDriver driver = {target, permissions, ExecuteScopedDatabase, &fixture};
    if (UmiUatRunTarget(&target, permissions, &driver, steps, operations, count, NULL, NULL, &report) != UMI_STATUS_OK)
        goto finish;
    /* Original modes keep outputs suppressed. Typed examples intentionally
     * show the newly checked row location, which contains no row contents. */
#if 0
    if (UmiUatReportSummary(report, &summary) != UMI_STATUS_OK ||
        UmiUatReportJson(report, 0, json, 16384U) != UMI_STATUS_OK) goto finish;
#endif
    if (UmiUatReportSummary(report, &summary) != UMI_STATUS_OK ||
        UmiUatReportJson(report, fixture.typedRows, json, 16384U) != UMI_STATUS_OK) goto finish;
    if (fputs(json, stdout) == EOF || fflush(stdout) != 0) goto finish;
    exitCode = summary.accepted ? 0 : 1;
finish:
    UmiUatReportDestroy(report);
    if (sqlite3_close(fixture.database) != SQLITE_OK) exitCode = 2;
    free(json);
    return exitCode;
}
