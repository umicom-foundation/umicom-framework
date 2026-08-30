/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/environment.h
 *
 * PURPOSE:
 *   Store public API-workbench variables and resolve {{name}} placeholders.
 *   Secret values are deliberately excluded and remain security-provider owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_ENVIRONMENT_H
#define UMICOM_WEB_WORKBENCH_ENVIRONMENT_H

#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebWorkbenchEnvironmentValue {
    char name[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char value[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    bool enabled;
} UmiWebWorkbenchEnvironmentValue;

typedef struct UmiWebWorkbenchEnvironment {
    char environment_id[UMI_WEB_WORKBENCH_ID_CAPACITY];
    char name[UMI_WEB_WORKBENCH_NAME_CAPACITY];
    UmiWebWorkbenchEnvironmentValue values[
        UMI_WEB_WORKBENCH_MAX_ENVIRONMENT_VALUES];
    size_t value_count;
    uint64_t revision;
} UmiWebWorkbenchEnvironment;

void umi_web_workbench_environment_init(
    UmiWebWorkbenchEnvironment *environment,
    const char *environment_id,
    const char *name);
UmiStatus umi_web_workbench_environment_set(
    UmiWebWorkbenchEnvironment *environment,
    const char *name,
    const char *value,
    bool enabled);
UmiStatus umi_web_workbench_environment_remove(
    UmiWebWorkbenchEnvironment *environment,
    const char *name);
const UmiWebWorkbenchEnvironmentValue *umi_web_workbench_environment_find(
    const UmiWebWorkbenchEnvironment *environment,
    const char *name);
UmiStatus umi_web_workbench_environment_resolve(
    const UmiWebWorkbenchEnvironment *environment,
    const char *source,
    char *out_text,
    size_t capacity,
    size_t *out_unresolved_count);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_ENVIRONMENT_H */
