/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_change_guard.c
 *
 * PURPOSE:
 *   Verify AI coding runtime change guard behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/change_guard.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiAiCodingPatch patch;
    UmiAiCodingChangeGuardPolicy policy;
    UmiAiCodingChangeGuardResult result;
    assert(umi_ai_coding_patch_init(
        &patch, "patch", "request", "Title", "Reason") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "src/a.c", UMI_AI_CODING_PATCH_DELETE,
        "int a;\n", "") == UMI_STATUS_OK);
    umi_ai_coding_change_guard_policy_init(&policy);
    assert(umi_ai_coding_change_guard_check(
        &patch, &policy, &result) == UMI_STATUS_OK);
    assert(!result.accepted);
    assert(strstr(result.reason, "Deletion") != NULL);

    return 0;
}
