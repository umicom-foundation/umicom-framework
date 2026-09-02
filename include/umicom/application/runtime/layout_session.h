/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/layout_session.h
 *
 * PURPOSE:
 *   Represent layout selection and lock state independently of any frontend toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_LAYOUT_SESSION_H
#define UMICOM_APPLICATION_RUNTIME_LAYOUT_SESSION_H

#include "umicom/application/runtime/types.h"
#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application layout session data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLayoutSession {
    uint32_t structure_size;
    const UmiApplicationExperienceDefinition *experience;
    const UmiExperienceLayoutDefinition *selected_layout;
    bool locked;
    uint64_t revision;
} UmiApplicationLayoutSession;

/**
 * Initialise application layout session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_layout_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationLayoutSession *out_session);
/**
 * Provide the application layout session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_layout_session_select(
    UmiApplicationLayoutSession *session,
    const char *layout_id);
/**
 * Provide the application layout session set locked operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_layout_session_set_locked(
    UmiApplicationLayoutSession *session,
    bool locked);
/**
 * Provide the application layout session current operation used by this module and its
 * client applications.
 */
const UmiExperienceLayoutDefinition *umi_application_layout_session_current(
    const UmiApplicationLayoutSession *session);
/* Materialise experience layouts as canonical workspace profiles.
 * Frontends then use the existing profile model for layout selection and persistence. */
UmiStatus umi_application_layout_session_apply_to_profiles(
    const UmiApplicationLayoutSession *session,
    UmiUiWorkspaceProfileModel *profiles);

#ifdef __cplusplus
}
#endif

#endif
