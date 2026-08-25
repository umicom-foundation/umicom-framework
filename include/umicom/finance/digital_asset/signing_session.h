/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/signing_session.h
 *
 * PURPOSE:
 *   Track threshold approvals for a signing request while avoiding secret-key handling.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_SESSION_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_SIGNING_SESSION_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/signing_request.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalSigningSession {
    UmiDigitalSigningRequest request;
    uint64_t revision;
} UmiDigitalSigningSession;

/* Start a signing session from a validated request. */
UmiStatus umi_digital_asset_signing_session_init(UmiDigitalSigningSession *session, const UmiDigitalSigningRequest *request);

/* Record one independent approval up to the configured threshold. */
UmiStatus umi_digital_asset_signing_session_approve(UmiDigitalSigningSession *session);

/* Return true when the approval threshold is satisfied. */
bool umi_digital_asset_signing_session_ready(const UmiDigitalSigningSession *session);

#ifdef __cplusplus
}
#endif

#endif
