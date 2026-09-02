/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/transport_model.h
 *
 * PURPOSE:
 *   Define reusable transport controls for video, audio, replay and animation timelines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TRANSPORT_MODEL_H
#define UMICOM_UI_WORKSTATION_TRANSPORT_MODEL_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named ws transport state values accepted by this public contract.
 */
typedef enum UmiWsTransportState {
    UMI_WS_TRANSPORT_STOPPED = 0,
    UMI_WS_TRANSPORT_PLAYING = 1,
    UMI_WS_TRANSPORT_PAUSED = 2,
    UMI_WS_TRANSPORT_SCRUBBING = 3
} UmiWsTransportState;

/**
 * Represent the ws transport model data shared with callers of this public contract.
 */
typedef struct UmiWsTransportModel {
    UmiWsTransportState state;
    int64_t position_ms;
    int64_t duration_ms;
    double rate;
    bool loop;
} UmiWsTransportModel;

/**
 * Initialise ws transport model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_transport_model_init(UmiWsTransportModel *model, int64_t duration_ms);
/**
 * Provide the ws transport model play operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_transport_model_play(UmiWsTransportModel *model);
/**
 * Provide the ws transport model pause operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_transport_model_pause(UmiWsTransportModel *model);
/**
 * Provide the ws transport model seek operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_transport_model_seek(UmiWsTransportModel *model, int64_t position_ms);
/**
 * Provide the ws transport model set rate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_transport_model_set_rate(UmiWsTransportModel *model, double rate);

#ifdef __cplusplus
}
#endif

#endif
