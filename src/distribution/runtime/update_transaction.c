/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_transaction.c
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
#include "umicom/distribution/runtime/update_transaction.h"

void umi_dr_update_transaction_init(UmiDrUpdateTransaction*t){if(t!=NULL){*t=(UmiDrUpdateTransaction){0};t->state=UMI_DR_STATE_PLANNED;}}UmiStatus umi_dr_update_transaction_stage(UmiDrUpdateTransaction*t){if(t==NULL||t->state!=UMI_DR_STATE_PLANNED||!t->checkpoint_ready)return UMI_STATUS_INVALID_STATE;t->files_staged=true;t->state=UMI_DR_STATE_READY;return UMI_STATUS_OK;}UmiStatus umi_dr_update_transaction_commit(UmiDrUpdateTransaction*t){if(t==NULL||t->state!=UMI_DR_STATE_READY||!t->health_verified)return UMI_STATUS_INVALID_STATE;t->state=UMI_DR_STATE_COMMITTED;return UMI_STATUS_OK;}UmiStatus umi_dr_update_transaction_rollback(UmiDrUpdateTransaction*t){if(t==NULL||!t->checkpoint_ready)return UMI_STATUS_INVALID_STATE;t->state=UMI_DR_STATE_ROLLED_BACK;return UMI_STATUS_OK;}
