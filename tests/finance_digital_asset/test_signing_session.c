/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_signing_session.c
 *
 * PURPOSE:
 *   Implement the test signing session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/signing_session.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalSigningRequest request;
    UmiDigitalSigningSession session;
    CHECK(umi_digital_asset_signing_request_init(&request, "SIGN-1", "TX-1", "POL-1", 2U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_session_init(&session, &request) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_session_approve(&session) == UMI_STATUS_OK);
    CHECK(!umi_digital_asset_signing_session_ready(&session));
    CHECK(umi_digital_asset_signing_session_approve(&session) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_session_ready(&session));
    return 0;
}
