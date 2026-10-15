/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/wallet_book.c
 *
 * PURPOSE:
 *   Implement a bounded book of operational and custody wallets.
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
#include "umicom/finance/digital_asset/wallet_book.h"

#include <string.h>

/* Reset catalogue state without heap allocation. */
void umi_digital_asset_wallet_book_init(UmiDigitalAssetWalletBook *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof *catalogue);
    }
}

/* Reject duplicates and preserve insertion order for reproducible snapshots. */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_digital_asset_wallet_book_add(UmiDigitalAssetWalletBook *catalogue, const UmiDigitalAssetWallet *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].id.value, (*item).id.value) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_DIGITAL_ASSET_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->items[catalogue->count++] = *item;
    catalogue->revision++;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_digital_asset_wallet_book_add(UmiDigitalAssetWalletBook *catalogue, const UmiDigitalAssetWallet *item)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (catalogue != NULL && catalogue->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].id.value, (*item).id.value) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_DIGITAL_ASSET_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->items[catalogue->count++] = *item;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

/* Resolve reference data without exposing catalogue storage mutability. */
const UmiDigitalAssetWallet *umi_digital_asset_wallet_book_find(const UmiDigitalAssetWalletBook *catalogue, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].id.value, id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}
