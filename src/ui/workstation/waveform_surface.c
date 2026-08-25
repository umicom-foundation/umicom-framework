/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/waveform_surface.c
 *
 * PURPOSE:
 *   Implement waveform display selection and amplitude state reusable by music, video and voice applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/waveform_surface.h"

UmiStatus umi_ws_waveform_surface_init(UmiWsWaveformSurface *surface,
                                       const char *waveform_id,
                                       int32_t sample_rate,
                                       int32_t channel_count,
                                       int64_t duration_samples) {
    if (surface == NULL || !umi_ws_id_valid(waveform_id) || sample_rate <= 0 || channel_count <= 0 || duration_samples < 0) return UMI_STATUS_INVALID_ARGUMENT;
    *surface = (UmiWsWaveformSurface){0};
    if (umi_ws_copy_text(surface->waveform_id, sizeof(surface->waveform_id), waveform_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->sample_rate = sample_rate;
    surface->channel_count = channel_count;
    surface->duration_samples = duration_samples;
    surface->vertical_scale = 1.0;
    surface->show_zero_line = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_waveform_surface_select(UmiWsWaveformSurface *surface, int64_t start, int64_t end) {
    if (surface == NULL || start < 0 || end < start || end > surface->duration_samples) return UMI_STATUS_INVALID_ARGUMENT;
    surface->selection_start = start;
    surface->selection_end = end;
    return UMI_STATUS_OK;
}

double umi_ws_waveform_surface_set_vertical_scale(UmiWsWaveformSurface *surface, double scale) {
    if (surface == NULL) return 0.0;
    if (scale < 0.1) scale = 0.1;
    if (scale > 16.0) scale = 16.0;
    surface->vertical_scale = scale;
    return scale;
}
