/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/session.h
 *
 * PURPOSE:
 *   Own deterministic in-memory application session state over immutable experience definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_SESSION_H
#define UMICOM_APPLICATION_RUNTIME_SESSION_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSession {
    uint32_t structure_size;
    const UmiApplicationExperienceDefinition *experience;
    const UmiExperienceLayoutDefinition *layout;
    const char *active_panel_ids[UMI_APPLICATION_RUNTIME_MAX_PANELS];
    size_t active_panel_count;
    bool layout_locked;
    uint64_t revision;
} UmiApplicationSession;

UmiStatus umi_application_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSession *out_session);
UmiStatus umi_application_session_select_layout(
    UmiApplicationSession *session,
    const char *layout_id);
UmiStatus umi_application_session_activate_panel(
    UmiApplicationSession *session,
    const char *panel_id);
UmiStatus umi_application_session_deactivate_panel(
    UmiApplicationSession *session,
    const char *panel_id);
int umi_application_session_panel_active(
    const UmiApplicationSession *session,
    const char *panel_id);
UmiStatus umi_application_session_set_layout_locked(
    UmiApplicationSession *session,
    bool locked);
UmiStatus umi_application_session_validate(
    const UmiApplicationSession *session);

#ifdef __cplusplus
}
#endif

#endif
