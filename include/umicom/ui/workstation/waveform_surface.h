/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/waveform_surface.h
 *
 * PURPOSE:
 *   Define waveform display selection and amplitude state reusable by music, video and voice applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_WAVEFORM_SURFACE_H
#define UMICOM_UI_WORKSTATION_WAVEFORM_SURFACE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsWaveformSurface {
    char waveform_id[UMI_UI_ID_CAPACITY];
    int32_t sample_rate;
    int32_t channel_count;
    int64_t duration_samples;
    int64_t selection_start;
    int64_t selection_end;
    double vertical_scale;
    bool show_zero_line;
} UmiWsWaveformSurface;

UmiStatus umi_ws_waveform_surface_init(UmiWsWaveformSurface *surface,
                                       const char *waveform_id,
                                       int32_t sample_rate,
                                       int32_t channel_count,
                                       int64_t duration_samples);
UmiStatus umi_ws_waveform_surface_select(UmiWsWaveformSurface *surface, int64_t start, int64_t end);
double umi_ws_waveform_surface_set_vertical_scale(UmiWsWaveformSurface *surface, double scale);

#ifdef __cplusplus
}
#endif

#endif
