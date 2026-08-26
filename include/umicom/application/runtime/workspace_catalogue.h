/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/workspace_catalogue.h
 *
 * PURPOSE:
 *   Expose canonical application layouts through one cross-product workspace catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_WORKSPACE_CATALOGUE_H
#define UMICOM_APPLICATION_RUNTIME_WORKSPACE_CATALOGUE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationWorkspaceEntry {
    const char *application_id;
    const char *application_name;
    const UmiExperienceLayoutDefinition *layout;
} UmiApplicationWorkspaceEntry;

size_t umi_application_workspace_catalogue_count(void);
UmiStatus umi_application_workspace_catalogue_at(
    size_t index,
    UmiApplicationWorkspaceEntry *out_entry);
UmiStatus umi_application_workspace_catalogue_find(
    const char *application_id,
    const char *layout_id,
    UmiApplicationWorkspaceEntry *out_entry);

#ifdef __cplusplus
}
#endif

#endif
