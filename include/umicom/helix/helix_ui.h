/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/helix_ui.h
 * PURPOSE: Project Helix v2 runtime state into toolkit-neutral pane models.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: GTK4, Qt6, Wt and web frontends can render these same view
 * models without embedding autonomy or approval logic in their widgets. */
#ifndef INCLUDE_UMICOM_HELIX_HELIX_UI_H
#define INCLUDE_UMICOM_HELIX_HELIX_UI_H

#include "umicom/helix/orchestrator.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_helix_ui_overview(const char *view_id,
                                   const UmiHelixOrchestrator *runtime,
                                   UmiUiViewModel **out_view);
UmiStatus umi_helix_ui_agents(const char *view_id,
                                 const UmiHelixOrchestrator *runtime,
                                 UmiUiViewModel **out_view);
UmiStatus umi_helix_ui_approvals(const char *view_id,
                                    const UmiHelixOrchestrator *runtime,
                                    UmiUiViewModel **out_view);
UmiStatus umi_helix_ui_journal(const char *view_id,
                                  const UmiHelixOrchestrator *runtime,
                                  UmiUiViewModel **out_view);
UmiStatus umi_helix_ui_candidates(const char *view_id,
                                     const UmiHelixOrchestrator *runtime,
                                     UmiUiViewModel **out_view);
UmiStatus umi_helix_ui_rollback(const char *view_id,
                                   const UmiHelixOrchestrator *runtime,
                                   UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
