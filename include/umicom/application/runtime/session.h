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

/**
 * Represent the application session data shared with callers of this public contract.
 */
typedef struct UmiApplicationSession {
    uint32_t structure_size;
    const UmiApplicationExperienceDefinition *experience;
    const UmiExperienceLayoutDefinition *layout;
    const char *active_panel_ids[UMI_APPLICATION_RUNTIME_MAX_PANELS];
    size_t active_panel_count;
    bool layout_locked;
    uint64_t revision;
} UmiApplicationSession;

/**
 * Initialise application session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_application_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationSession *out_session);
/**
 * Provide the application session select layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_select_layout(
    UmiApplicationSession *session,
    const char *layout_id);
/**
 * Provide the application session activate panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_activate_panel(
    UmiApplicationSession *session,
    const char *panel_id);
/**
 * Provide the application session deactivate panel operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_deactivate_panel(
    UmiApplicationSession *session,
    const char *panel_id);
/**
 * Provide the application session panel active operation used by this module and its
 * client applications.
 */
int umi_application_session_panel_active(
    const UmiApplicationSession *session,
    const char *panel_id);
/**
 * Provide the application session set layout locked operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_session_set_layout_locked(
    UmiApplicationSession *session,
    bool locked);
/**
 * Check that application session satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_session_validate(
    const UmiApplicationSession *session);

#ifdef __cplusplus
}
#endif

#endif
