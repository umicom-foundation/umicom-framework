/* Umicom Foundation | Sammy Hegab | MIT */
#include "umicom/broker_connectivity/connection.h"
#include <stdio.h>
int main(void)
{
    UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
    for (unsigned i = 0; i < 2U; ++i) {
        if (i == 1U) {
            options.environment = UMI_TRADING_LIVE;
            options.adapter.paperOnly = 0;
            options.adapter.port = UmiIbkrDefaultPort(UMI_IBKR_TWS, UMI_TRADING_LIVE);
            options.acknowledgeLive = true;
        }
        UmiIbkrConnection *connection = NULL;
        if (UmiIbkrConnectionCreate(&options, &connection) != UMI_STATUS_OK) return 1;
        /* Construction allocates state; Open would be a separate, explicit action. */
        UmiIbkrConnectionDestroy(connection);
    }
    puts("SDK consumer: Paper and Live profiles constructed without opening sockets.");
    return 0;
}
