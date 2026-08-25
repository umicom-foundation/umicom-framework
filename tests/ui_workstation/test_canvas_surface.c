#include <stdio.h>
#include "umicom/ui/workstation/canvas_surface.h"
int main(void) {
    UmiWsCanvasSurface c;
    if (umi_ws_canvas_surface_init(&c, "kitchen.plan") != UMI_STATUS_OK) return 1;
    if (umi_ws_canvas_surface_set_zoom(&c, 2.0) != 2.0) return 2;
    umi_ws_canvas_surface_pan(&c, 10.0, -5.0);
    umi_ws_canvas_surface_set_grid(&c, 20, true);
    if (c.pan_x != 10.0 || c.grid_size != 20) return 3;
    puts("canvas surface: ok");
    return 0;
}
