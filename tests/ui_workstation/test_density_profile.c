#include <stdio.h>
#include "umicom/ui/workstation/density_profile.h"
int main(void) {
    UmiWsDensityProfile p = umi_ws_density_profile_make(UMI_WS_DENSITY_COMPACT);
    if (p.row_height != 23 || p.hit_target != 28) return 1;
    p = umi_ws_density_profile_make(UMI_WS_DENSITY_TOUCH);
    if (p.hit_target != 48) return 2;
    puts("density profile: ok");
    return 0;
}
