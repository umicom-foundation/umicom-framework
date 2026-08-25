#include <stdio.h>
#include "umicom/ui/workstation/toolbar_model.h"
int main(void) {
    UmiWsToolbarModel t;
    umi_ws_toolbar_model_init(&t);
    if (umi_ws_toolbar_model_add(&t, "run", "Run", "run.start", UMI_WS_TOOLBAR_BUTTON) != UMI_STATUS_OK) return 1;
    if (umi_ws_toolbar_model_set_active(&t, "run", true) != UMI_STATUS_OK) return 2;
    if (!t.items[0].active) return 3;
    puts("toolbar model: ok");
    return 0;
}
