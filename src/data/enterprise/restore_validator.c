/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/restore_validator.c
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
#include "umicom/data/enterprise/restore_validator.h"
#include <string.h>

/* Restore validation turns destructive preconditions into inspectable blockers. */ UmiStatus umi_data_restore_validator_check(const UmiDataRestorePlan *plan,const UmiDataBackupManifest *manifest,UmiDataRestoreValidation *out_validation){if(plan==NULL||manifest==NULL||out_validation==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_validation,0,sizeof(*out_validation));out_validation->backup_complete=manifest->complete;out_validation->schema_match=plan->expected_schema_fingerprint==manifest->schema_fingerprint;out_validation->approval_required=!plan->verify_only;if(!out_validation->backup_complete)out_validation->blockers++;if(!out_validation->schema_match)out_validation->blockers++;if(out_validation->approval_required&&!plan->approved)out_validation->blockers++;out_validation->valid=out_validation->blockers==0U;return UMI_STATUS_OK;}
