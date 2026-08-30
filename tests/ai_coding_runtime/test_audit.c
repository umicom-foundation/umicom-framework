/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_audit.c
 *
 * PURPOSE:
 *   Verify AI coding runtime audit behavior.
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
#include "umicom/ai_coding_runtime/audit.h"

int main(void)
{

    UmiAiCodingAuditLog *log = NULL;
    UmiAiCodingAuditRecord input = {0};
    UmiAiCodingAuditRecord output;
    (void)strcpy(input.task_id, "task");
    input.kind = UMI_AI_CODING_EVENT_VALIDATION;
    input.state = UMI_AI_CODING_RUNTIME_VALIDATING;
    input.status = UMI_STATUS_OK;
    assert(umi_ai_coding_audit_log_create(&log) == UMI_STATUS_OK);
    assert(umi_ai_coding_audit_log_append(log, &input) == UMI_STATUS_OK);
    assert(umi_ai_coding_audit_log_at(log, 0U, &output) == UMI_STATUS_OK);
    assert(output.sequence == 1U);
    assert(strcmp(output.task_id, "task") == 0);
    umi_ai_coding_audit_log_destroy(log);

    return 0;
}
