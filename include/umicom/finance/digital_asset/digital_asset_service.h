/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/digital_asset_service.h
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DIGITAL_ASSET_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DIGITAL_ASSET_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/finance/digital_asset/network_catalogue.h"
#include "umicom/finance/digital_asset/asset_catalogue.h"
#include "umicom/finance/digital_asset/wallet_book.h"
#include "umicom/finance/digital_asset/custody_book.h"
#include "umicom/finance/digital_asset/balance_book.h"
#include "umicom/finance/digital_asset/transaction_book.h"
#include "umicom/finance/digital_asset/transfer_book.h"
#include "umicom/finance/digital_asset/digital_market_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalAssetService {
    UmiDigitalNetworkCatalogue networks;
    UmiDigitalAssetCatalogue assets;
    UmiDigitalAssetWalletBook wallets;
    UmiDigitalCustodyBook custody_accounts;
    UmiDigitalBalanceBook balances;
    UmiDigitalTransactionBook transactions;
    UmiDigitalTransferBook transfers;
    UmiDigitalMarketCatalogue markets;
    bool started;
    uint64_t revision;
} UmiDigitalAssetService;

/* Initialise all provider-neutral digital-asset registries. */
void umi_digital_asset_digital_asset_service_init(UmiDigitalAssetService *service);

/* Activate the service after Framework dependency composition is ready. */
UmiStatus umi_digital_asset_digital_asset_service_start(UmiDigitalAssetService *service);

/* Return true when the aggregate digital-asset service is ready for thin clients. */
bool umi_digital_asset_digital_asset_service_ready(const UmiDigitalAssetService *service);

#ifdef __cplusplus
}
#endif

#endif
