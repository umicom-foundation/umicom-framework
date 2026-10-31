/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/connection_guidance.h
 *
 * PURPOSE:
 *   Explain the next connection step from observed broker state without performing an action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTION_GUIDANCE_H
#define UMICOM_BROKER_CONNECTION_GUIDANCE_H
#include "umicom/broker_connectivity/connection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiIbkrConnectionStep {
    UMI_IBKR_STEP_OPEN = 0,
    UMI_IBKR_STEP_WAIT,
    UMI_IBKR_STEP_CHECK_ENDPOINT,
    UMI_IBKR_STEP_CHANGE_CLIENT,
    UMI_IBKR_STEP_REVIEW_PROTOCOL,
    UMI_IBKR_STEP_REVIEW_PROVIDER,
    UMI_IBKR_STEP_RECONNECT,
    UMI_IBKR_STEP_READ_ACCOUNT,
    UMI_IBKR_STEP_WAIT_REPORT,
    UMI_IBKR_STEP_REVIEW_REPORT
} UmiIbkrConnectionStep;
typedef struct UmiIbkrConnectionGuidance {
    UmiIbkrConnectionStep step;
    const char *title;
    const char *detail;
} UmiIbkrConnectionGuidance;

/* Strings have static lifetime. This pure function reads a copied observation,
 * opens no socket and changes no settings. Advice describes a next check, not
 * a diagnosis proven by a numeric status or an attestation of Paper/Live.
 * Invalid arguments leave outGuidance unchanged. */
UmiStatus UmiIbkrConnectionExplain(const UmiIbkrConnectionSnapshot *snapshot,
    UmiIbkrConnectionGuidance *outGuidance);
#ifdef __cplusplus
}
#endif
#endif
