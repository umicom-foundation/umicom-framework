/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/types.h
 *
 * PURPOSE:
 *   Define provider-neutral digital-asset, network, custody and transfer value types
 *   without embedding vendor SDK or blockchain implementation details.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCE_DIGITAL_ASSET_TYPES_H
#define UMICOM_FINANCE_DIGITAL_ASSET_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIGITAL_ASSET_ID_CAPACITY 72U
#define UMI_DIGITAL_ASSET_NAME_CAPACITY 96U
#define UMI_DIGITAL_ASSET_SYMBOL_CAPACITY 24U
#define UMI_DIGITAL_ASSET_ADDRESS_CAPACITY 160U
#define UMI_DIGITAL_ASSET_HASH_CAPACITY 96U
#define UMI_DIGITAL_ASSET_MAX_ITEMS 64U

typedef enum UmiDigitalNetworkFamily {
    UMI_DIGITAL_NETWORK_UNKNOWN = 0,
    UMI_DIGITAL_NETWORK_UTXO = 1,
    UMI_DIGITAL_NETWORK_ACCOUNT = 2,
    UMI_DIGITAL_NETWORK_PERMISSIONED = 3,
    UMI_DIGITAL_NETWORK_OTHER = 4
} UmiDigitalNetworkFamily;

typedef enum UmiDigitalTransactionState {
    UMI_DIGITAL_TX_CREATED = 0,
    UMI_DIGITAL_TX_SIGNING = 1,
    UMI_DIGITAL_TX_SUBMITTED = 2,
    UMI_DIGITAL_TX_CONFIRMED = 3,
    UMI_DIGITAL_TX_FINAL = 4,
    UMI_DIGITAL_TX_FAILED = 5,
    UMI_DIGITAL_TX_CANCELLED = 6
} UmiDigitalTransactionState;

typedef struct UmiDigitalAssetId {
    char value[UMI_DIGITAL_ASSET_ID_CAPACITY];
} UmiDigitalAssetId;

typedef struct UmiDigitalAmount {
    int64_t units;
    int32_t scale;
    char asset_symbol[UMI_DIGITAL_ASSET_SYMBOL_CAPACITY];
} UmiDigitalAmount;

/* Copy text into fixed Framework buffers while preserving NUL termination. */
UmiStatus umi_digital_asset_copy_text(char *destination, size_t capacity, const char *source);

/* Return true when a fixed-buffer identifier contains a non-empty value. */
bool umi_digital_asset_text_valid(const char *text);

#ifdef __cplusplus
}
#endif
#endif
