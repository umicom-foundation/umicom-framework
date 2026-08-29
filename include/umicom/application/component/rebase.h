/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/rebase.h
 *
 * PURPOSE:
 *   Move a user-customised workspace onto a newer Framework recipe while
 *   keeping matching user choices and safely introducing new components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_REBASE_H
#define UMICOM_APPLICATION_COMPONENT_REBASE_H

#include "umicom/application/component/draft.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationComponentRebaseReport {
  size_t preserved_count;
  size_t added_count;
  size_t removed_count;
  uint64_t previous_revision;
  uint64_t result_revision;
} UmiApplicationComponentRebaseReport;

UmiStatus umi_application_component_workspace_rebase(
    UmiApplicationComponentWorkspaceDraft *draft, const UmiApplicationComponentRecipe *new_recipe,
    int keep_removed_components, UmiApplicationComponentRebaseReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
