#include <stdio.h>
#include "umicom/ui/workstation/waveform_surface.h"
int main(void) {
    UmiWsWaveformSurface w;
    if (umi_ws_waveform_surface_init(&w, "audio.main", 48000, 2, 480000) != UMI_STATUS_OK) return 1;
    if (umi_ws_waveform_surface_select(&w, 100, 1000) != UMI_STATUS_OK) return 2;
    if (umi_ws_waveform_surface_set_vertical_scale(&w, 50.0) != 16.0) return 3;
    puts("waveform surface: ok");
    return 0;
}
