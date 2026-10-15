/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/digital_asset_service.c
 *
 * PURPOSE:
 *   Aggregate digital networks, assets, custody, balances, transfers and markets into one Framework service.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include "umicom/finance/digital_asset/digital_asset_service.h"

#include <string.h>

/* Initialise every registry so no application owns hidden custody state. */
void umi_digital_asset_digital_asset_service_init(UmiDigitalAssetService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service != NULL) {
        memset(service, 0, sizeof *service);
        umi_digital_asset_network_catalogue_init(&service->networks);
        umi_digital_asset_asset_catalogue_init(&service->assets);
        umi_digital_asset_wallet_book_init(&service->wallets);
        umi_digital_asset_custody_book_init(&service->custody_accounts);
        umi_digital_asset_balance_book_init(&service->balances);
        umi_digital_asset_transaction_book_init(&service->transactions);
        umi_digital_asset_transfer_book_init(&service->transfers);
        umi_digital_asset_digital_market_catalogue_init(&service->markets);
        service->revision = 1U;
    }
}

/* Activation is explicit for runtime bootstrap, observability and policy control. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_digital_asset_digital_asset_service_start(UmiDigitalAssetService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->started = true;
    service->revision++;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_digital_asset_digital_asset_service_start(UmiDigitalAssetService *service)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (service != NULL && service->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    service->started = true;
    service->revision++;
    return UMI_STATUS_OK;
}

/* Service readiness is intentionally independent from having live network connections. */
bool umi_digital_asset_digital_asset_service_ready(const UmiDigitalAssetService *service)
{
    return service != NULL && service->started && service->revision > 0U;
}
