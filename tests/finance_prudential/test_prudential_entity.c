/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_prudential_entity.c
 *
 * PURPOSE:
 *   Verify represent a regulated legal entity and its prudential reporting perimeter.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/prudential_entity.h"

int main(void) { UmiPrudentialEntity r; if (umi_pru_prudential_entity_init(&r,"BANK-1","Umicom Bank","GB") != UMI_STATUS_OK) return 1; if (r.entity_id[0] != 'B' || r.jurisdiction[0] != 'G') return 2; return 0; }
