/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_ibkr_adapter.c
 *
 * PURPOSE:
 *   Verify IBKR configuration, paper-only order mapping and status mapping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/ibkr_adapter.h"

int main(void)
{
    UmiIbkrAdapterConfig config;
    UmiOrderRequest request = {0};
    UmiIbkrOrderMessage message;
    UmiOrderStatus status;

    umi_ibkr_adapter_config_init(&config);
    assert(umi_ibkr_adapter_config_validate(&config) == UMI_STATUS_OK);

    request.side = UMI_SIDE_BUY;
    request.type = UMI_ORDER_LIMIT;
    request.tif = UMI_TIF_DAY;
    request.quantity = 2.0;
    request.limit_price = 100.0;
    request.environment = UMI_TRADING_PAPER;
    assert(umi_ibkr_adapter_map_order(&request, &config, &message) ==
           UMI_STATUS_OK);
    assert(strcmp(message.action, "BUY") == 0);
    assert(strcmp(message.orderType, "LMT") == 0);

    request.environment = UMI_TRADING_LIVE;
    assert(umi_ibkr_adapter_map_order(&request, &config, &message) ==
           UMI_STATUS_PERMISSION_DENIED);

    assert(umi_ibkr_adapter_map_status("Submitted", &status) == UMI_STATUS_OK);
    assert(status == UMI_ORDER_ACCEPTED);
    return 0;
}
