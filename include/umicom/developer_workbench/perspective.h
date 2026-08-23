/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspective.h
 *
 * PURPOSE:
 *   Define task-oriented workbench perspectives independently from physical
 *   layout. Workspace answers "what"; perspective answers "which tools"; layout
 *   answers "where"; session answers "what is currently open/running".
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchPerspectiveDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *perspective_id;
    const char *title;
    const char *description;
    const char *const *surface_ids;
    size_t surface_count;
    const char *default_focus_surface_id;
} UmiDeveloperWorkbenchPerspectiveDefinition;

UmiStatus umi_developer_workbench_perspective_validate(
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective);

#ifdef __cplusplus
}
#endif

#endif
