/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/runtime.h
 *
 * PURPOSE:
 *   Coordinate layout selection and geometric projection for one canonical suite application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_RUNTIME_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_RUNTIME_H

#include "umicom/application/suite_layout/catalogue.h"
#include "umicom/application/suite_layout/projection.h"
#include "umicom/application/suite_layout/snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSuiteLayoutRuntime {
    const UmiApplicationExperienceDefinition *experience;
    UmiUiWorkspaceLayout active_layout;
    uint64_t revision;
    int loaded;
} UmiApplicationSuiteLayoutRuntime;

void umi_application_suite_layout_runtime_init(
    UmiApplicationSuiteLayoutRuntime *runtime);
UmiStatus umi_application_suite_layout_runtime_load(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *application_id);
UmiStatus umi_application_suite_layout_runtime_select(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *layout_id);
const UmiUiWorkspaceLayout *umi_application_suite_layout_runtime_active(
    const UmiApplicationSuiteLayoutRuntime *runtime);
UmiApplicationSuiteLayoutSnapshot umi_application_suite_layout_runtime_snapshot(
    const UmiApplicationSuiteLayoutRuntime *runtime);

#ifdef __cplusplus
}
#endif
#endif
