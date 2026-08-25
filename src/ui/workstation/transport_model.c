/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/transport_model.c
 *
 * PURPOSE:
 *   Implement reusable transport controls for video, audio, replay and animation timelines.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/transport_model.h"

UmiStatus umi_ws_transport_model_init(UmiWsTransportModel *model, int64_t duration_ms) {
    if (model == NULL || duration_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    *model = (UmiWsTransportModel){0};
    model->duration_ms = duration_ms;
    model->rate = 1.0;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_transport_model_play(UmiWsTransportModel *model) {
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->state = UMI_WS_TRANSPORT_PLAYING;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_transport_model_pause(UmiWsTransportModel *model) {
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->state = UMI_WS_TRANSPORT_PAUSED;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_transport_model_seek(UmiWsTransportModel *model, int64_t position_ms) {
    if (model == NULL || position_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->duration_ms > 0 && position_ms > model->duration_ms) position_ms = model->duration_ms;
    model->position_ms = position_ms;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_transport_model_set_rate(UmiWsTransportModel *model, double rate) {
    if (model == NULL || rate <= 0.0 || rate > 64.0) return UMI_STATUS_INVALID_ARGUMENT;
    model->rate = rate;
    return UMI_STATUS_OK;
}
