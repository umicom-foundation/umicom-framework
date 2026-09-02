/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_audit.c
 *
 * PURPOSE:
 *   Verify aggregate appearance accessibility, scaling, typography and renderer-parity findings into one audit result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_audit.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceAppearanceAudit item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_audit_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_audit_is_valid(&item)) return 2;
    item.errors=1U; umi_appearance_audit_evaluate(&item); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(item.passed) return 3;
    return 0;
}
