/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/builtin_perspectives.h
 *
 * PURPOSE:
 *   Enumerate and bulk-register Framework developer perspectives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_BUILTIN_PERSPECTIVES_H
#define UMICOM_DEVELOPER_WORKBENCH_BUILTIN_PERSPECTIVES_H

#include "umicom/developer_workbench/perspective_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_developer_workbench_builtin_perspective_count(void);

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_builtin_perspective_at(size_t index);

UmiStatus umi_developer_workbench_builtin_perspectives_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
