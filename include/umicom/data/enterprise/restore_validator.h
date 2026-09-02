/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/restore_validator.h
 *
 * PURPOSE:
 *   Validate backup completeness, schema compatibility and explicit approval before restore execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_RESTORE_VALIDATOR_H
#define UMICOM_DATA_ENTERPRISE_RESTORE_VALIDATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/restore_plan.h"
#include "umicom/data/enterprise/backup_manifest.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data restore validation data shared with callers of this public contract.
 */
typedef struct UmiDataRestoreValidation { bool valid; bool schema_match; bool backup_complete; bool approval_required; size_t blockers; } UmiDataRestoreValidation;
/* Validate a restore plan against backup evidence without performing the restore. */ UmiStatus umi_data_restore_validator_check(const UmiDataRestorePlan *plan,const UmiDataBackupManifest *manifest,UmiDataRestoreValidation *out_validation);

#ifdef __cplusplus
}
#endif
#endif
