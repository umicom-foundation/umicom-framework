/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_validator.c
 *
 * PURPOSE:
 *   Apply destructive/reversibility/schema-compatibility policy before a migration plan can execute.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_validator.h"
#include <string.h>

/* Validation converts risky migration characteristics into explicit gates. */ UmiStatus umi_data_migration_validator_check(const UmiDataMigrationPlan *plan,UmiDataCompatibility compatibility,bool backup_available,bool destructive_approved,UmiDataMigrationValidation *out_validation){if(plan==NULL||out_validation==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out_validation,0,sizeof(*out_validation));out_validation->requires_backup=plan->destructive||compatibility==UMI_DATA_BREAKING;out_validation->requires_approval=plan->destructive||compatibility==UMI_DATA_BREAKING;if(out_validation->requires_backup&&!backup_available)out_validation->blockers++;if(out_validation->requires_approval&&!destructive_approved)out_validation->blockers++;out_validation->valid=out_validation->blockers==0U;return UMI_STATUS_OK;}
