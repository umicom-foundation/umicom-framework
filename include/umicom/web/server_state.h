/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/server_state.h
 *
 * PURPOSE:
 *   Expose observable web-server lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SERVER_STATE_H
#define UMICOM_WEB_SERVER_STATE_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web server state data shared with callers of this public contract.
 */
typedef struct UmiWebServerState { UmiWebServerPhase phase; uint16_t port; uint64_t requests; UmiStatus last_status; } UmiWebServerState;
/**
 * Initialise web server state from caller-provided values so later operations receive a
 * known state.
 */
void umi_web_server_state_init(UmiWebServerState *state);
/**
 * Provide the web server phase text operation used by this module and its client
 * applications.
 */
const char *umi_web_server_phase_text(UmiWebServerPhase phase);
#ifdef __cplusplus
}
#endif
#endif
