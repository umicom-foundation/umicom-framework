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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/digital_asset_service.h"

#include <string.h>

/* Initialise every registry so no application owns hidden custody state. */
void umi_digital_asset_digital_asset_service_init(UmiDigitalAssetService *service)
{
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
UmiStatus umi_digital_asset_digital_asset_service_start(UmiDigitalAssetService *service)
{
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
