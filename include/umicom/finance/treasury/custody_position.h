/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/custody_position.h
 *
 * PURPOSE:
 *   Represent settled, pending-in and pending-out custody quantities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CUSTODY_POSITION_H
#define UMICOM_FINANCE_TREASURY_CUSTODY_POSITION_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCustodyPosition {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t settled_quantity;
    int64_t pending_in_quantity;
    int64_t pending_out_quantity;
} UmiTreasuryCustodyPosition;
UmiStatus umi_treasury_custody_position_init(UmiTreasuryCustodyPosition *value,
    const char *id,
    int64_t settled_quantity,
    int64_t pending_in_quantity,
    int64_t pending_out_quantity);
bool umi_treasury_custody_position_valid(const UmiTreasuryCustodyPosition *value);
int64_t umi_treasury_custody_position_projected_quantity(const UmiTreasuryCustodyPosition *value);
#ifdef __cplusplus
}
#endif
#endif
