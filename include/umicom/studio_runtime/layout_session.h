/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_session.h
 *
 * PURPOSE:
 *   Persist semantic Studio Application Shell state/layout through the existing
 *   Framework UmiSessionStore.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_SESSION_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_SESSION_H

#include "umicom/application_shell/layout.h"
#include "umicom/application_shell/state.h"
#include "umicom/platform/session_store.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_LAYOUT_SESSION_MAX_PLACEMENTS 64U

UmiStatus umi_studio_layout_session_save(
    UmiSessionStore *store,
    const char *prefix,
    const char *active_preset_id,
    const UmiApplicationShellState *state,
    const UmiApplicationShellLayout *layout);

UmiStatus umi_studio_layout_session_restore(
    const UmiSessionStore *store,
    const char *prefix,
    char *out_preset_id,
    size_t preset_capacity,
    UmiApplicationShellState *state,
    UmiApplicationShellLayout *layout,
    int *out_restored);

#ifdef __cplusplus
}
#endif
#endif
