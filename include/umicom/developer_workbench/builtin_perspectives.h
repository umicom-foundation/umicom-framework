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

/**
 * Return the number of records represented by developer workbench builtin perspective
 * without changing their state.
 */
size_t umi_developer_workbench_builtin_perspective_count(void);

/**
 * Find developer workbench builtin perspective while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_builtin_perspective_at(size_t index);

/**
 * Add developer workbench builtin perspectives only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_developer_workbench_builtin_perspectives_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
