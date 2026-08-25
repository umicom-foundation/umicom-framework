#include <stdio.h>
#include "umicom/ui/workstation/transport_model.h"
int main(void) {
    UmiWsTransportModel m;
    if (umi_ws_transport_model_init(&m, 10000) != UMI_STATUS_OK) return 1;
    if (umi_ws_transport_model_play(&m) != UMI_STATUS_OK || m.state != UMI_WS_TRANSPORT_PLAYING) return 2;
    if (umi_ws_transport_model_seek(&m, 12000) != UMI_STATUS_OK || m.position_ms != 10000) return 3;
    if (umi_ws_transport_model_set_rate(&m, 2.0) != UMI_STATUS_OK || m.rate != 2.0) return 4;
    puts("transport model: ok");
    return 0;
}
