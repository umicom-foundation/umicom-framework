/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/session.h
 *
 * PURPOSE:
 *   Carry shared session identity and correlation state across application boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_SESSION_H
#define UMICOM_INTEGRATION_SESSION_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

typedef struct UmiIntegrationSession {
    char session_id[UMI_INTEGRATION_ID_CAPACITY];
    char identity_id[UMI_INTEGRATION_ID_CAPACITY];
    char tenant_id[UMI_INTEGRATION_ID_CAPACITY];
    uint64_t next_correlation;
} UmiIntegrationSession;

UmiStatus umi_integration_session_init(
    UmiIntegrationSession *session,
    const char *session_id,
    const char *identity_id);
UmiStatus umi_integration_session_next_correlation(
    UmiIntegrationSession *session,
    char *output,
    size_t capacity);

#endif
