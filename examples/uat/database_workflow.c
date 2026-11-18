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
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DatabaseFixture {
    sqlite3 *database;
    int failAssertion;
    int failCleanup;
} DatabaseFixture;

/* This fixture owns a private :memory: database. No connection string, SQL
 * file or table name is accepted from the command line. A production adapter
 * needs a separately enforced environment and resource-ownership contract. */
static UmiStatus ExecuteDatabase(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result)
{
    DatabaseFixture *fixture = context;
    const char *sql = NULL;
    int code = SQLITE_OK;
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    if (strcmp(test->test_id, "create") == 0)
        sql = "CREATE TABLE notes(id INTEGER PRIMARY KEY, text TEXT NOT NULL);";
    else if (strcmp(test->test_id, "edit") == 0)
        sql = "INSERT INTO notes VALUES(1,'draft'),(2,'remove');"
              "UPDATE notes SET text='saved' WHERE id=1; DELETE FROM notes WHERE id=2;";
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
    } else if (strcmp(test->test_id, "drop") == 0)
        sql = fixture->failCleanup ? "DROP TABLE deliberately_missing;" : "DROP TABLE IF EXISTS notes;";
    else return UMI_STATUS_INVALID_ARGUMENT;
    if (sql != NULL) code = sqlite3_exec(fixture->database, sql, NULL, NULL, NULL);
    /* The row's duration is left at zero: this example does not measure time. */
    umi_test_result_finish(result, code == SQLITE_OK ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE,
        code == SQLITE_OK ? 0 : 1, 0U);
    (void)snprintf(result->output, sizeof(result->output), "%s",
        code == SQLITE_OK ? "Fixture check completed." : "Fixture operation or assertion failed.");
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    const char *ids[] = {"create", "edit", "verify", "drop"};
    UmiUatStep steps[4] = {0};
    UmiUatReport *report = NULL;
    UmiUatSummary summary = {0};
    DatabaseFixture fixture = {0};
    char *json = malloc(16384U);
    int exitCode = 1;
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--fail-assertion") != 0 &&
        strcmp(argv[1], "--fail-cleanup") != 0)) {
        fprintf(stderr, "Usage: umicom-uat-database-example [--fail-assertion|--fail-cleanup]\n");
        free(json); return 2;
    }
    fixture.failAssertion = argc == 2 && strcmp(argv[1], "--fail-assertion") == 0;
    fixture.failCleanup = argc == 2 && strcmp(argv[1], "--fail-cleanup") == 0;
    if (json == NULL) return 2;
    if (sqlite3_open(":memory:", &fixture.database) != SQLITE_OK) {
        sqlite3_close(fixture.database); free(json); return 2;
    }
    for (size_t i = 0U; i < 4U; ++i) {
        steps[i].phase = i == 0U ? UMI_UAT_SETUP : i == 1U ? UMI_UAT_ACTION :
            i == 2U ? UMI_UAT_ASSERT : UMI_UAT_CLEANUP;
        umi_test_case_init(&steps[i].test, ids[i], ids[i]);
        if (umi_test_case_set_command(&steps[i].test, "sqlite-memory-fixture", "", "") != UMI_STATUS_OK)
            goto finish;
        steps[i].test.kind = UMI_TEST_KIND_INTEGRATION;
        steps[i].test.timeout_ms = 1000U;
    }
    if (UmiUatRun(steps, 4U, ExecuteDatabase, &fixture, NULL, NULL, &report) != UMI_STATUS_OK)
        goto finish;
    if (UmiUatReportSummary(report, &summary) != UMI_STATUS_OK ||
        UmiUatReportJson(report, 0, json, 16384U) != UMI_STATUS_OK) goto finish;
    if (fputs(json, stdout) == EOF || fflush(stdout) != 0) goto finish;
    exitCode = summary.accepted ? 0 : 1;
finish:
    UmiUatReportDestroy(report);
    if (sqlite3_close(fixture.database) != SQLITE_OK) exitCode = 2;
    free(json);
    return exitCode;
}
