/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/transport_model.h
 *
 * PURPOSE:
 *   Define reusable transport controls for video, audio, replay and animation timelines.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TRANSPORT_MODEL_H
#define UMICOM_UI_WORKSTATION_TRANSPORT_MODEL_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsTransportState {
    UMI_WS_TRANSPORT_STOPPED = 0,
    UMI_WS_TRANSPORT_PLAYING = 1,
    UMI_WS_TRANSPORT_PAUSED = 2,
    UMI_WS_TRANSPORT_SCRUBBING = 3
} UmiWsTransportState;

typedef struct UmiWsTransportModel {
    UmiWsTransportState state;
    int64_t position_ms;
    int64_t duration_ms;
    double rate;
    bool loop;
} UmiWsTransportModel;

UmiStatus umi_ws_transport_model_init(UmiWsTransportModel *model, int64_t duration_ms);
UmiStatus umi_ws_transport_model_play(UmiWsTransportModel *model);
UmiStatus umi_ws_transport_model_pause(UmiWsTransportModel *model);
UmiStatus umi_ws_transport_model_seek(UmiWsTransportModel *model, int64_t position_ms);
UmiStatus umi_ws_transport_model_set_rate(UmiWsTransportModel *model, double rate);

#ifdef __cplusplus
}
#endif

#endif
