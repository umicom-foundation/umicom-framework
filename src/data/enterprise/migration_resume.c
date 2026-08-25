/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/migration_resume.c
 *
 * PURPOSE:
 *   Decide whether a migration checkpoint can safely resume against a current schema fingerprint.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_resume.h"
#include <string.h>

/* Resume is denied if observed schema no longer matches checkpoint evidence. */ UmiStatus umi_data_migration_resume_decide(const UmiDataMigrationPlan *plan,const UmiDataMigrationCheckpoint *checkpoint,uint64_t observed_fingerprint,size_t *out_next_step){if(plan==NULL||checkpoint==NULL||out_next_step==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(checkpoint->committed)return UMI_STATUS_INVALID_STATE;if(checkpoint->current_fingerprint!=observed_fingerprint)return UMI_STATUS_INVALID_STATE;if(checkpoint->completed_steps>plan->count)return UMI_STATUS_INVALID_STATE;*out_next_step=checkpoint->completed_steps;return UMI_STATUS_OK;}
