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
int main(void) {
    UmiAppearanceAppearanceService item;
    if (umi_appearance_service_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_service_is_valid(&item)) return 2;
    if (!umi_appearance_service_ready(&item)) return 3;
    return 0;
}
