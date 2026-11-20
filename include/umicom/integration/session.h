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

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_SESSION_H
#define UMICOM_INTEGRATION_SESSION_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the integration session data shared with callers of this public contract.
 */
typedef struct UmiIntegrationSession {
    char session_id[UMI_INTEGRATION_ID_CAPACITY];
    char identity_id[UMI_INTEGRATION_ID_CAPACITY];
    char tenant_id[UMI_INTEGRATION_ID_CAPACITY];
    uint64_t next_correlation;
} UmiIntegrationSession;

/**
 * Initialise integration session from caller-provided values so later operations receive a
 * known state.
 * The inputs may use existing session text. Failure leaves the session unchanged.
 */
UmiStatus umi_integration_session_init(
    UmiIntegrationSession *session,
    const char *session_id,
    const char *identity_id);
/**
 * Provide the integration session next correlation operation used by this module and its
 * client applications.
 * The caller serialises use and provides output storage disjoint from the session.
 * The counter issues positive decimal values through UINT64_MAX, then returns
 * CAPACITY_EXCEEDED until a new session is established. Errors do not change
 * either the output buffer or the session. A session ID can be too long to
 * fit, together with its suffix, into an envelope's fixed correlation field.
 */
UmiStatus umi_integration_session_next_correlation(
    UmiIntegrationSession *session,
    char *output,
    size_t capacity);


#ifdef __cplusplus
}
#endif
#endif
