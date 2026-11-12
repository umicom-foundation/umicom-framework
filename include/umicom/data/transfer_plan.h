/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/transfer_plan.h
 *
 * PURPOSE:
 *   Publish the public transfer plan contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_TRANSFER_PLAN_H
#define UMICOM_DATA_TRANSFER_PLAN_H
#include "umicom/data/explorer_types.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the database transfer plan data shared with callers of this public contract.
 */
typedef struct UmiDatabaseTransferPlan { char id[UMI_DATABASE_ID_CAPACITY]; UmiDatabaseTransferDirection direction; UmiDatabaseTransferFormat format; char table_name[UMI_DATABASE_NAME_CAPACITY]; char path[UMI_DATABASE_PATH_CAPACITY]; char delimiter[8]; int include_header; int transactional; int dry_run; size_t processed_rows; size_t rejected_rows; UmiStatus status; } UmiDatabaseTransferPlan;
/**
 * Initialise database transfer plan from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_database_transfer_plan_init(UmiDatabaseTransferPlan *plan,const char *id,UmiDatabaseTransferDirection direction,UmiDatabaseTransferFormat format,const char *table_name,const char *path);
/**
 * Check that database transfer plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_database_transfer_plan_validate(const UmiDatabaseTransferPlan *plan,char *message,size_t message_capacity);

#ifdef __cplusplus
}
#endif
#endif
