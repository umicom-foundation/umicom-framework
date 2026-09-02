/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/repository_plan.h
 *
 * PURPOSE:
 *   Build explicit ORM persistence operations from tracked changes before any SQL is executed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_REPOSITORY_PLAN_H
#define UMICOM_DATA_ENTERPRISE_REPOSITORY_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/change_tracker.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data repository operation data shared with callers of this public
 * contract.
 */
typedef struct UmiDataRepositoryOperation { char operation_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char key[UMI_DATA_ENTERPRISE_ID_CAPACITY]; UmiDataChangeKind kind; uint32_t order; } UmiDataRepositoryOperation;
/**
 * Represent the data repository plan data shared with callers of this public contract.
 */
typedef struct UmiDataRepositoryPlan { UmiDataRepositoryOperation items[UMI_DATA_ENTERPRISE_MAX_OPERATIONS]; size_t count; } UmiDataRepositoryPlan;
/* Reset a repository operation plan. */ void umi_data_repository_plan_init(UmiDataRepositoryPlan *plan);
/* Build an ordered plan from a change tracker. */ UmiStatus umi_data_repository_plan_build(UmiDataRepositoryPlan *plan,const UmiDataChangeTracker *tracker);
/* Validate deterministic operation ordering and identifiers. */ UmiStatus umi_data_repository_plan_validate(const UmiDataRepositoryPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
