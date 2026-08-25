#include <stdio.h>
#include "umicom/ui/workstation/workstation.h"
#include "umicom/ui/workstation/studio_layouts.h"
int main(void) {
    UmiWsWorkstation w;
    UmiWsSurfaceDescriptor d;
    UmiWsLayoutTemplate l;
    UmiWsPerspectiveTemplate p;
    if (umi_ws_workstation_init(&w, "umicom.desktop", UMI_WS_DENSITY_COMPACT) != UMI_STATUS_OK) return 1;
    if (umi_ws_surface_descriptor_init(&d, "studio.editor", "Editor", UMI_WS_DOMAIN_STUDIO, UMI_WS_SURFACE_EDITOR) != UMI_STATUS_OK) return 2;
    if (umi_ws_surface_catalogue_upsert(&w.surfaces, &d) != UMI_STATUS_OK) return 3;
    if (umi_ws_studio_layout_code(&l) != UMI_STATUS_OK) return 4;
    if (umi_ws_layout_catalogue_upsert(&w.layouts, &l) != UMI_STATUS_OK) return 5;
    if (umi_ws_perspective_template_init(&p, "studio.code", "Code", UMI_WS_DOMAIN_STUDIO) != UMI_STATUS_OK) return 6;
    if (umi_ws_perspective_template_set_default_layout(&p, "studio.code") != UMI_STATUS_OK) return 7;
    if (umi_ws_perspective_catalogue_upsert(&w.perspectives, &p) != UMI_STATUS_OK) return 8;
    if (umi_ws_workstation_activate_layout(&w, "studio.code") != UMI_STATUS_OK) return 9;
    if (umi_ws_workstation_activate_perspective(&w, "studio.code") != UMI_STATUS_OK) return 10;
    if (!umi_ws_workstation_ready(&w) || umi_ws_workstation_fingerprint(&w) == 0U) return 11;
    puts("workstation: ok");
    return 0;
}
