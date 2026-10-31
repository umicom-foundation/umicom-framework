/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/bond_description.inc
 *
 * PURPOSE:
 *   Decode bond contract identities without treating metadata as permission to trade.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiStatus BondDescription(char **fields, size_t count, UmiIbkrContractDescription *out)
{
    uint64_t pairs, id;
    /* Versions 164..176 omit the legacy message version and do not yet contain
     * bond trading hours. Keep the count exact so a shifted frame is refused. */
    if (count < 36U || !UmiIbkrUnsigned(fields[30], &pairs) || pairs > 16U ||
        count != 36U + 2U * (size_t)pairs || !UmiIbkrUnsigned(fields[19], &id) ||
        !id || id > INT_MAX || strcmp(fields[3],"BOND")) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 2U; i < count; ++i)
        if (!UmiIbkrText(fields[i],4096U,true)) return UMI_STATUS_PARSE_ERROR;
    if (!UmiIbkrDecimalText(fields[20])) return UMI_STATUS_PARSE_ERROR;
    UmiIbkrContractDescription row = {0}; row.contractId = (uint32_t)id;
    size_t tail = 31U + 2U * (size_t)pairs;
    char *dest[] = {row.symbol,row.securityType,row.expiry,row.exchange,row.currency,
        row.marketName,row.tradingClass,row.minimumTick,row.orderTypes,row.validExchanges,
        row.longName,row.marketRuleIds,row.minimumSize,row.sizeIncrement,row.suggestedSizeIncrement};
    const size_t caps[] = {sizeof row.symbol,sizeof row.securityType,sizeof row.expiry,
        sizeof row.exchange,sizeof row.currency,sizeof row.marketName,sizeof row.tradingClass,
        sizeof row.minimumTick,sizeof row.orderTypes,sizeof row.validExchanges,sizeof row.longName,
        sizeof row.marketRuleIds,sizeof row.minimumSize,sizeof row.sizeIncrement,sizeof row.suggestedSizeIncrement};
    const size_t indexes[] = {2U,3U,6U,15U,16U,17U,18U,20U,21U,22U,27U,tail+1U,tail+2U,tail+3U,tail+4U};
    for (size_t i = 0U; i < sizeof indexes / sizeof indexes[0]; ++i) {
        size_t size = strlen(fields[indexes[i]]) + 1U;
        if (size > caps[i]) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(dest[i],fields[indexes[i]],size);
    }
    if (!row.symbol[0] || !row.exchange[0] || !row.currency[0]) return UMI_STATUS_PARSE_ERROR;
    *out = row; return UMI_STATUS_OK;
}
