/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_session.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime session contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/ai_coding_runtime/session.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted session value. */
static int UmiAiCodingSessionConstructionCases(void)
{
    UmiAiCodingSession *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_session_init(value, "session", "C:/work") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->session_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_session_init(value, oversized, "C:/work") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_session_init(value, value->session_id, "C:/work") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->session_id, "session") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->workspace_root) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_session_init(value, "session", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ai_coding_session_init(value, "session", value->workspace_root) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->workspace_root, "C:/work") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiAiCodingSessionConstructionCases() != 0) return 1;


    UmiAiCodingSession session;
    assert(umi_ai_coding_session_init(
        &session, "session", "C:/work") == UMI_STATUS_OK);
    assert(umi_ai_coding_session_begin_task(
        &session, "task") == UMI_STATUS_OK);
    assert(session.task_count == 1U);
    assert(umi_ai_coding_session_complete_task(
        &session, UMI_AI_CODING_RUNTIME_COMPLETED) == UMI_STATUS_OK);
    assert(session.completed_count == 1U);
    assert(session.active_task_id[0] == '\0');

    return 0;
}
