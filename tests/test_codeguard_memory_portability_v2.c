/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_memory_portability_v2.c
 *
 * PURPOSE:
 *   Implement the test codeguard memory portability v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Memory and portability v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/codeguard/memory_audit.h"
#include "umicom/codeguard/portability_audit.h"
int main(void)
{
    UmiCodeGuardEvidenceStore evidence = {0};
    assert(umi_codeguard_memory_audit_line("src/model.c",10U,"items = realloc(items, capacity);",&evidence) == UMI_STATUS_OK);
    assert(umi_codeguard_portability_audit_line("src/model.c",11U,"Sleep(100);",&evidence) == UMI_STATUS_OK);
    assert(evidence.count == 2U);
    assert(umi_codeguard_portability_audit_line("src/platform/timer_win32.c",12U,"Sleep(100);",&evidence) == UMI_STATUS_OK);
    assert(evidence.count == 2U && umi_codeguard_portability_is_adapter_path("src/platform/timer_win32.c"));
    return 0;
}
