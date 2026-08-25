#include <stdio.h>
#include "umicom/ui/workstation/data_grid_surface.h"
int main(void) {
    UmiWsDataGridSurface g;
    if (umi_ws_data_grid_surface_init(&g, "orders") != UMI_STATUS_OK) return 1;
    if (umi_ws_data_grid_surface_add_column(&g, "symbol", "Symbol", 80) != UMI_STATUS_OK) return 2;
    umi_ws_data_grid_surface_set_rows(&g, 10U);
    if (g.column_count != 1U || g.row_count != 10U) return 3;
    puts("data grid surface: ok");
    return 0;
}
