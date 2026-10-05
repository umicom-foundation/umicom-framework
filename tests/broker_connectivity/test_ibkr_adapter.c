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
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/ibkr_adapter.h"

#include "../value_archive/transfer_cases.h"

#include "umicom/broker_connectivity/ibkr_adapter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiIbkrAdapterConfigTransferEqual(const UmiIbkrAdapterConfig *a, const UmiIbkrAdapterConfig *b)
{
    return strcmp(a->host, b->host) == 0 &&
        a->port == b->port &&
        a->clientId == b->clientId &&
        strcmp(a->account, b->account) == 0 &&
        a->paperOnly == b->paperOnly &&
        a->readOnly == b->readOnly;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiIbkrAdapterConfigTransferTails(UmiIbkrAdapterConfig *value)
{
    (void)value;
    {
        size_t used = strlen(value->host) + 1U;
        memset(value->host + used, 0xa5, sizeof(value->host) - used);
    }
    {
        size_t used = strlen(value->account) + 1U;
        memset(value->account + used, 0xa5, sizeof(value->account) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiIbkrAdapterConfigTransferMalformed(const UmiIbkrAdapterConfig *sample)
{
    (void)sample;
    {
        UmiIbkrAdapterConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.host, 'x', sizeof(invalid.host));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ibkr_adapter_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ibkr_adapter_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated host was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiIbkrAdapterConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account, 'x', sizeof(invalid.account));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ibkr_adapter_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ibkr_adapter_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiIbkrAdapterConfigTransferCases, UmiIbkrAdapterConfig,
    umi_ibkr_adapter_config_archive_encode, umi_ibkr_adapter_config_archive_decode,
    UmiIbkrAdapterConfigTransferEqual, UmiIbkrAdapterConfigTransferTails, UmiIbkrAdapterConfigTransferMalformed)

int main(void)
{
    UmiIbkrAdapterConfig config;
    UmiOrderRequest request = {0};
    UmiIbkrOrderMessage message;
    UmiOrderStatus status;

    umi_ibkr_adapter_config_init(&config);
    assert(umi_ibkr_adapter_config_validate(&config) == UMI_STATUS_OK);
    if (UmiIbkrAdapterConfigTransferCases(&config) != 0) return 1;


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
