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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_QUALITY_WORKBENCH_SERVICE_H
#define UMICOM_TEST_WORKBENCH_TEST_QUALITY_WORKBENCH_SERVICE_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestQualityWorkbenchService {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestQualityWorkbenchService;
UmiStatus umi_test_quality_workbench_service_init(UmiTestQualityWorkbenchService *model,const char *id,const char *label);
UmiStatus umi_test_quality_workbench_service_set_active(UmiTestQualityWorkbenchService *model,bool active);
UmiStatus umi_test_quality_workbench_service_set_count(UmiTestQualityWorkbenchService *model,uint32_t item_count);
UmiStatus umi_test_quality_workbench_service_set_state(UmiTestQualityWorkbenchService *model,UmiTestWorkbenchState state);
int umi_test_quality_workbench_service_valid(const UmiTestQualityWorkbenchService *model);
#ifdef __cplusplus
}
#endif
#endif
