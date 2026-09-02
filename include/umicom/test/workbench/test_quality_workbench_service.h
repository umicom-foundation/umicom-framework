/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_quality_workbench_service.h
 *
 * PURPOSE:
 *   Model test quality workbench service state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_QUALITY_WORKBENCH_SERVICE_H
#define UMICOM_TEST_WORKBENCH_TEST_QUALITY_WORKBENCH_SERVICE_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test quality workbench service data shared with callers of this public
 * contract.
 */
typedef struct UmiTestQualityWorkbenchService {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestQualityWorkbenchService;
/**
 * Initialise test quality workbench service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_quality_workbench_service_init(UmiTestQualityWorkbenchService *model,const char *id,const char *label);
/**
 * Exercise test quality workbench service set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_test_quality_workbench_service_set_active(UmiTestQualityWorkbenchService *model,bool active);
/**
 * Return the number of records represented by test quality workbench service set without
 * changing their state.
 */
UmiStatus umi_test_quality_workbench_service_set_count(UmiTestQualityWorkbenchService *model,uint32_t item_count);
/**
 * Exercise test quality workbench service set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_test_quality_workbench_service_set_state(UmiTestQualityWorkbenchService *model,UmiTestWorkbenchState state);
/**
 * Check that test quality workbench service satisfies its contract before another service
 * relies on it.
 */
int umi_test_quality_workbench_service_valid(const UmiTestQualityWorkbenchService *model);
#ifdef __cplusplus
}
#endif
#endif
