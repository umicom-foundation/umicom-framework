/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/waveform_surface.h
 *
 * PURPOSE:
 *   Define waveform display selection and amplitude state reusable by music, video and voice applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_WAVEFORM_SURFACE_H
#define UMICOM_UI_WORKSTATION_WAVEFORM_SURFACE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws waveform surface data shared with callers of this public contract.
 */
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

/**
 * Initialise ws waveform surface from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_waveform_surface_init(UmiWsWaveformSurface *surface,
                                       const char *waveform_id,
                                       int32_t sample_rate,
                                       int32_t channel_count,
                                       int64_t duration_samples);
/**
 * Provide the ws waveform surface select operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_waveform_surface_select(UmiWsWaveformSurface *surface, int64_t start, int64_t end);
/**
 * Provide the ws waveform surface set vertical scale operation used by this module and its
 * client applications.
 */
double umi_ws_waveform_surface_set_vertical_scale(UmiWsWaveformSurface *surface, double scale);

#ifdef __cplusplus
}
#endif

#endif
