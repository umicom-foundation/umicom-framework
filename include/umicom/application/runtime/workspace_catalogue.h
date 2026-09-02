/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/workspace_catalogue.h
 *
 * PURPOSE:
 *   Expose canonical application layouts through one cross-product workspace catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_WORKSPACE_CATALOGUE_H
#define UMICOM_APPLICATION_RUNTIME_WORKSPACE_CATALOGUE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application workspace entry data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationWorkspaceEntry {
    const char *application_id;
    const char *application_name;
    const UmiExperienceLayoutDefinition *layout;
} UmiApplicationWorkspaceEntry;

/**
 * Return the number of records represented by application workspace catalogue without
 * changing their state.
 */
size_t umi_application_workspace_catalogue_count(void);
/**
 * Find application workspace catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_application_workspace_catalogue_at(
    size_t index,
    UmiApplicationWorkspaceEntry *out_entry);
/**
 * Find application workspace catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_application_workspace_catalogue_find(
    const char *application_id,
    const char *layout_id,
    UmiApplicationWorkspaceEntry *out_entry);

#ifdef __cplusplus
}
#endif

#endif
