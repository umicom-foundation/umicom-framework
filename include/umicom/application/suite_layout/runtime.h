/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/runtime.h
 *
 * PURPOSE:
 *   Coordinate layout selection and geometric projection for one canonical suite application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_RUNTIME_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_RUNTIME_H

#include "umicom/application/suite_layout/catalogue.h"
#include "umicom/application/suite_layout/projection.h"
#include "umicom/application/suite_layout/snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Active layout state for one application session.
 *
 * `experience` is borrowed from the immutable Framework catalogue. The active
 * layout is copied into the runtime and remains valid until reinitialisation.
 */
typedef struct UmiApplicationSuiteLayoutRuntime {
    const UmiApplicationExperienceDefinition *experience;
    UmiUiWorkspaceLayout active_layout;
    uint64_t revision;
    int loaded;
} UmiApplicationSuiteLayoutRuntime;

/**
 * Resets a runtime to a safe unloaded state without allocating memory.
 *
 * @param runtime Caller-owned runtime to initialise. A null pointer is ignored.
 */
void umi_application_suite_layout_runtime_init(
    UmiApplicationSuiteLayoutRuntime *runtime);
/**
 * Loads one application and projects its default layout.
 *
 * @param runtime Initialised caller-owned runtime.
 * @param application_id Stable identifier resolved through the catalogue.
 * @return `UMI_STATUS_OK` when the default workspace is active.
 */
UmiStatus umi_application_suite_layout_runtime_load(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *application_id);
/**
 * Replaces the active workspace with another canonical layout.
 *
 * @param runtime Loaded caller-owned runtime.
 * @param layout_id Stable layout identifier owned by the active experience.
 * @return `UMI_STATUS_OK` when projection succeeds and revision advances.
 */
UmiStatus umi_application_suite_layout_runtime_select(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *layout_id);
/**
 * Returns the currently active workspace layout.
 *
 * @param runtime Loaded layout runtime.
 * @return A pointer owned by `runtime`, or `NULL` while unloaded.
 */
const UmiUiWorkspaceLayout *umi_application_suite_layout_runtime_active(
    const UmiApplicationSuiteLayoutRuntime *runtime);
/**
 * Copies small observable runtime facts for a frontend or diagnostic tool.
 *
 * @param runtime Runtime whose state will be observed.
 * @return An owned value snapshot. Missing or unloaded input produces a safe
 *         snapshot whose `loaded` member is false.
 */
UmiApplicationSuiteLayoutSnapshot umi_application_suite_layout_runtime_snapshot(
    const UmiApplicationSuiteLayoutRuntime *runtime);

#ifdef __cplusplus
}
#endif
#endif
