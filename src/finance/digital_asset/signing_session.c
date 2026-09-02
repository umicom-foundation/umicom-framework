/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/signing_session.c
 *
 * PURPOSE:
 *   Track threshold approvals for a signing request while avoiding secret-key handling.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/signing_session.h"

#include <string.h>

/* Copy the request so approval state remains isolated from caller mutation. */
UmiStatus umi_digital_asset_signing_session_init(UmiDigitalSigningSession *session, const UmiDigitalSigningRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || request == NULL || !umi_digital_asset_signing_request_valid(request)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(session, 0, sizeof *session);
    session->request = *request;
    session->revision = 1U;
    return UMI_STATUS_OK;
}

/* Approval accounting is deliberately separate from actual cryptographic signing. */
UmiStatus umi_digital_asset_signing_session_approve(UmiDigitalSigningSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->request.closed) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->request.received_approvals >= session->request.required_approvals) return UMI_STATUS_ALREADY_EXISTS;
    session->request.received_approvals++;
    session->revision++;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->request.received_approvals == session->request.required_approvals) session->request.closed = true;
    return UMI_STATUS_OK;
}

/* The session becomes ready only when the configured threshold is met. */
bool umi_digital_asset_signing_session_ready(const UmiDigitalSigningSession *session)
{
    return session != NULL && session->request.received_approvals >= session->request.required_approvals;
}
