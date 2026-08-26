/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_types.c
 *
 * PURPOSE:
 *   Verify common prudential numeric, text and fingerprint primitives.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/types.h"
#include <string.h>
int main(void)
{
    char text[8]; double ratio=0.0, weighted=0.0;
    if (umi_pru_copy_text(text, sizeof text, "CET1") != UMI_STATUS_OK) return 1;
    if (strcmp(text, "CET1") != 0) return 2;
    if (umi_pru_copy_text(text, 4U, "TOOLONG") != UMI_STATUS_CAPACITY_EXCEEDED) return 3;
    if (umi_pru_hash_text("stress") == 0U) return 4;
    if (umi_pru_ratio(12.0, 4.0, &ratio) != UMI_STATUS_OK || ratio != 3.0) return 5;
    if (umi_pru_weighted_amount(100.0, 0.75, &weighted) != UMI_STATUS_OK || weighted != 75.0) return 6;
    return 0;
}
