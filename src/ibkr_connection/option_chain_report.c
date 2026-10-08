/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/option_chain_report.c
 * PURPOSE: Keep option expiry and strike sets distinct in an owned discovery report.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/discovery_report.h"
#include <stdlib.h>
enum ChainColumn
{
    CHAIN_KIND,
    CHAIN_REQUEST,
    CHAIN_UNDERLYING,
    CHAIN_SYMBOL,
    CHAIN_TYPE,
    CHAIN_FILTER,
    CHAIN_COMPLETE,
    CHAIN_FAILED,
    CHAIN_ABANDONED,
    CHAIN_STALE,
    CHAIN_CONNECTION,
    CHAIN_ENVIRONMENT,
    CHAIN_ATTESTED,
    CHAIN_REQUESTED,
    CHAIN_COMPLETED,
    CHAIN_EXPORTED,
    CHAIN_PROVIDER,
    CHAIN_MESSAGE,
    CHAIN_INDEX,
    CHAIN_EXCHANGE,
    CHAIN_CLASS,
    CHAIN_MULTIPLIER,
    CHAIN_EXPIRY_COUNT,
    CHAIN_STRIKE_COUNT,
    CHAIN_VALUE,
    CHAIN_EXACT,
    CHAIN_COLUMNS
};
static UmiCsvCell ChainFlag(bool value) { return UmiCsvText(value ? "true" : "false"); }
static void ChainMetadata(const UmiIbkrConnection *c, const UmiIbkrOptionChainSnapshot *s, uint64_t now,
                          const char *kind, size_t index, const UmiIbkrOptionChain *chain, UmiCsvCell *cells)
{
    for (size_t i = 0; i < CHAIN_COLUMNS; ++i)
        cells[i] = UmiCsvText("");
    cells[CHAIN_KIND] = UmiCsvText(kind);
    cells[CHAIN_REQUEST] = UmiCsvUnsigned(s->requestId);
    cells[CHAIN_UNDERLYING] = UmiCsvUnsigned(s->query.underlyingContractId);
    cells[CHAIN_SYMBOL] = UmiCsvText(s->query.underlyingSymbol);
    cells[CHAIN_TYPE] = UmiCsvText(s->query.underlyingSecurityType);
    cells[CHAIN_FILTER] = UmiCsvText(s->query.exchange);
    cells[CHAIN_COMPLETE] = ChainFlag(s->complete);
    cells[CHAIN_FAILED] = ChainFlag(s->failed);
    cells[CHAIN_ABANDONED] = ChainFlag(s->abandoned);
    cells[CHAIN_STALE] = ChainFlag(s->stale);
    cells[CHAIN_CONNECTION] = UmiCsvText(UmiIbkrConnectionStateName(c->snapshot.state));
    cells[CHAIN_ENVIRONMENT] =
        UmiCsvText(c->snapshot.requestedEnvironment == UMI_TRADING_LIVE ? "live" : "paper");
    cells[CHAIN_ATTESTED] = ChainFlag(c->snapshot.environmentAttested);
    cells[CHAIN_REQUESTED] = UmiCsvUnsigned(s->requestedAtMilliseconds);
    cells[CHAIN_COMPLETED] = UmiCsvUnsigned(s->completedAtMilliseconds);
    cells[CHAIN_EXPORTED] = UmiCsvUnsigned(now);
    cells[CHAIN_PROVIDER] = UmiCsvSigned(s->providerCode);
    cells[CHAIN_MESSAGE] = UmiCsvText(s->message);
    if (chain)
    {
        cells[CHAIN_INDEX] = UmiCsvUnsigned((uint64_t)index);
        cells[CHAIN_EXCHANGE] = UmiCsvText(chain->exchange);
        cells[CHAIN_CLASS] = UmiCsvText(chain->tradingClass);
        cells[CHAIN_MULTIPLIER] = UmiCsvText(chain->multiplier);
        cells[CHAIN_EXPIRY_COUNT] = UmiCsvUnsigned((uint64_t)chain->expiryCount);
        cells[CHAIN_STRIKE_COUNT] = UmiCsvUnsigned((uint64_t)chain->strikeCount);
    }
}
UmiStatus UmiIbkrOptionChainExportCsv(const UmiIbkrConnection *c, uint32_t request, uint64_t now,
                                      UmiCsvDocument **out)
{
    if (!out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiIbkrOptionChainSnapshot s;
    UmiStatus status = UmiIbkrOptionChainCopy(c, request, now, &s);
    if (status != UMI_STATUS_OK)
        return status;
    static const char *const headers[] = {"record_kind",
                                          "request_id",
                                          "underlying_contract_id",
                                          "symbol",
                                          "underlying_type",
                                          "exchange_filter",
                                          "complete",
                                          "failed",
                                          "abandoned",
                                          "stale",
                                          "connection_state",
                                          "requested_environment",
                                          "environment_attested",
                                          "requested_monotonic_ms",
                                          "completed_monotonic_ms",
                                          "exported_monotonic_ms",
                                          "provider_code",
                                          "message",
                                          "chain_index",
                                          "exchange",
                                          "trading_class",
                                          "multiplier",
                                          "expiry_count",
                                          "strike_count",
                                          "value",
                                          "exact"};
    _Static_assert(sizeof headers / sizeof headers[0] == CHAIN_COLUMNS, "Option CSV columns must agree");
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
    UmiCsvCell cells[CHAIN_COLUMNS];
    for (size_t i = 0; i < CHAIN_COLUMNS; ++i)
        cells[i] = UmiCsvText(headers[i]);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, CHAIN_COLUMNS);
    ChainMetadata(c, &s, now, "metadata", 0U, NULL, cells);
    if (status == UMI_STATUS_OK)
        status = UmiCsvDocumentAppendRow(document, cells, CHAIN_COLUMNS);
    UmiIbkrOptionChain *chain = s.count ? malloc(sizeof *chain) : NULL;
    if (s.count && !chain)
        status = UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0; status == UMI_STATUS_OK && i < s.count; ++i)
    {
        status = UmiIbkrOptionChainItemCopy(c, request, i, chain);
        if (status != UMI_STATUS_OK)
            break;
        ChainMetadata(c, &s, now, "chain", i, chain, cells);
        status = UmiCsvDocumentAppendRow(document, cells, CHAIN_COLUMNS);
        for (size_t j = 0; status == UMI_STATUS_OK && j < chain->expiryCount; ++j)
        {
            ChainMetadata(c, &s, now, "expiry", i, chain, cells);
            cells[CHAIN_VALUE] = UmiCsvText(chain->expirations[j]);
            status = UmiCsvDocumentAppendRow(document, cells, CHAIN_COLUMNS);
        }
        for (size_t j = 0; status == UMI_STATUS_OK && j < chain->strikeCount; ++j)
        {
            ChainMetadata(c, &s, now, "strike", i, chain, cells);
            cells[CHAIN_VALUE] = UmiCsvText(chain->strikes[j].reportedText);
            cells[CHAIN_EXACT] = ChainFlag(chain->strikes[j].exact);
            status = UmiCsvDocumentAppendRow(document, cells, CHAIN_COLUMNS);
        }
    }
    free(chain);
    if (status != UMI_STATUS_OK)
    {
        UmiCsvDocumentDestroy(document);
        return status;
    }
    *out = document;
    return UMI_STATUS_OK;
}
