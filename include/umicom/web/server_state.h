/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/server_state.h
 *
 * PURPOSE:
 *   Expose observable web-server lifecycle state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SERVER_STATE_H
#define UMICOM_WEB_SERVER_STATE_H
#include "umicom/web/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebServerState { UmiWebServerPhase phase; uint16_t port; uint64_t requests; UmiStatus last_status; } UmiWebServerState;
void umi_web_server_state_init(UmiWebServerState *state);
const char *umi_web_server_phase_text(UmiWebServerPhase phase);
#ifdef __cplusplus
}
#endif
#endif
