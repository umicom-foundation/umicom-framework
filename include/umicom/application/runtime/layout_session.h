/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/layout_session.h
 *
 * PURPOSE:
 *   Represent layout selection and lock state independently of any frontend toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_LAYOUT_SESSION_H
#define UMICOM_APPLICATION_RUNTIME_LAYOUT_SESSION_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationLayoutSession {
    uint32_t structure_size;
    const UmiApplicationExperienceDefinition *experience;
    const UmiExperienceLayoutDefinition *selected_layout;
    bool locked;
    uint64_t revision;
} UmiApplicationLayoutSession;

UmiStatus umi_application_layout_session_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationLayoutSession *out_session);
UmiStatus umi_application_layout_session_select(
    UmiApplicationLayoutSession *session,
    const char *layout_id);
UmiStatus umi_application_layout_session_set_locked(
    UmiApplicationLayoutSession *session,
    bool locked);
const UmiExperienceLayoutDefinition *umi_application_layout_session_current(
    const UmiApplicationLayoutSession *session);

#ifdef __cplusplus
}
#endif

#endif
