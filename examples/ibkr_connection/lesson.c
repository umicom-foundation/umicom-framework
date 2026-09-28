/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/broker_connectivity/connection.h"
#include <stdio.h>
int main(void)
{
    UmiIbkrConnectionOptions paper=UmiIbkrConnectionOptionsDefault();
    UmiIbkrConnectionOptions live=paper;
    live.environment=UMI_TRADING_LIVE;live.adapter.paperOnly=0;
    live.adapter.port=UmiIbkrDefaultPort(UMI_IBKR_TWS,UMI_TRADING_LIVE);
    if(UmiIbkrConnectionValidate(&paper)!=UMI_STATUS_OK ||
       UmiIbkrConnectionValidate(&live)!=UMI_STATUS_PERMISSION_DENIED)return 1;
    live.acknowledgeLive=true;
    if(UmiIbkrConnectionValidate(&live)!=UMI_STATUS_OK)return 1;
    printf("Paper TWS default: %u; Live TWS default: %u\n",(unsigned)paper.adapter.port,(unsigned)live.adapter.port);
    puts("Both profiles are read-only. A port number does not prove the account mode.");
    puts("Practice complete. No socket, file, order or network request was created.");
    return 0;
}
