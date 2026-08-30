/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/unit_of_work.h
 *
 * PURPOSE:
 *   Coordinate identity, change tracking and commit readiness without hiding transaction execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_UNIT_OF_WORK_H
#define UMICOM_DATA_ENTERPRISE_UNIT_OF_WORK_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/identity_map.h"
#include "umicom/data/enterprise/change_tracker.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataUnitOfWork { char unit_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; UmiDataIdentityMap identities; UmiDataChangeTracker changes; bool transaction_open; bool validation_passed; bool committed; } UmiDataUnitOfWork;
/* Open a new logical unit of work. */ UmiStatus umi_data_unit_of_work_begin(UmiDataUnitOfWork *unit,const char *unit_id);
/* Mark validation evidence after repository/query preparation. */ UmiStatus umi_data_unit_of_work_validate(UmiDataUnitOfWork *unit,bool passed);
/* Commit only after validation and while a transaction is open. */ UmiStatus umi_data_unit_of_work_commit(UmiDataUnitOfWork *unit);
/* Roll back local orchestration state without performing backend I/O. */ void umi_data_unit_of_work_rollback(UmiDataUnitOfWork *unit);

#ifdef __cplusplus
}
#endif
#endif
