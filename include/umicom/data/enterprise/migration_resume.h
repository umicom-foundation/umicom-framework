/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_resume.h
 *
 * PURPOSE:
 *   Decide whether a migration checkpoint can safely resume against a current schema fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_RESUME_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_RESUME_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/migration_plan.h"
#include "umicom/data/enterprise/migration_checkpoint.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Check checkpoint continuity and return the first step index to resume. */ UmiStatus umi_data_migration_resume_decide(const UmiDataMigrationPlan *plan,const UmiDataMigrationCheckpoint *checkpoint,uint64_t observed_fingerprint,size_t *out_next_step);

#ifdef __cplusplus
}
#endif
#endif
