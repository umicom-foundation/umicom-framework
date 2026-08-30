/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/update_transaction.h
 *
 * PURPOSE:
 *   update transaction lifecycle and commit/rollback state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_UPDATE_TRANSACTION_H
#define UMICOM_DISTRIBUTION_RUNTIME_UPDATE_TRANSACTION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDrUpdateTransaction { char id[UMI_DR_ID_CAPACITY]; UmiDrState state; bool checkpoint_ready; bool files_staged; bool health_verified; } UmiDrUpdateTransaction;
void umi_dr_update_transaction_init(UmiDrUpdateTransaction *transaction); UmiStatus umi_dr_update_transaction_stage(UmiDrUpdateTransaction *transaction); UmiStatus umi_dr_update_transaction_commit(UmiDrUpdateTransaction *transaction); UmiStatus umi_dr_update_transaction_rollback(UmiDrUpdateTransaction *transaction);

#ifdef __cplusplus
}
#endif
#endif
