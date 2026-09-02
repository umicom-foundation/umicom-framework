/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_service.c
 *
 * PURPOSE:
 *   Verify expose aggregate readiness for Framework-owned production appearance services consumed by every thin application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceAppearanceService item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_service_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_service_is_valid(&item)) return 2;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_service_ready(&item)) return 3;
    return 0;
}
