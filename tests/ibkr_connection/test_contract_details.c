/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_contract_details.c
 * PURPOSE: Review real request framing and reject incomplete or mismatched contract metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
/* Wire examples are specified independently of the decoder's indexes.
 * They contain invented instrument data and never open a broker socket. */
static int Detail(Fixture *fixture, const char *request, const char *identity, const char *exchange,
                  const char *mode)
{
    const char *fields[] = {"10",
                            request,
                            "WORKSHOP",
                            "STK",
                            "",
                            "0",
                            "",
                            exchange,
                            "GBP",
                            "WORKSHOP",
                            "Example market",
                            "CLASS",
                            identity,
                            "0.01",
                            "1",
                            "LMT,MKT",
                            "SMART,LSE",
                            "1",
                            "0",
                            "Workshop plc",
                            "LSE",
                            "",
                            "Industry",
                            "Category",
                            "Subcategory",
                            "Europe/London",
                            "20261007:0800-1630",
                            "20261007:0800-1630",
                            "",
                            "0",
                            "0",
                            "1",
                            "",
                            "",
                            "26,26",
                            "",
                            "COMMON",
                            "1",
                            "1",
                            "1"};
    if (strcmp(mode, "malformed") == 0)
        fields[30] = "99";
    if (strcmp(mode, "numeric") == 0)
        fields[13] = "not-a-tick";
    if (strcmp(mode, "long-name") == 0)
    {
        static char name[600];
        memset(name, 'a', sizeof name);
        name[sizeof name - 1U] = '\0';
        fields[19] = name;
    }
    if (strcmp(mode, "invalid-utf8") == 0)
        fields[19] = "\xc0\xaf";
    return Feed(fixture, fields, sizeof fields / sizeof fields[0]);
}
static int Run(Fixture *fixture, const char *mode, UmiIbkrContractDetailsSnapshot *copy)
{
    CHECK(Connect(fixture) == 0);
    UmiIbkrQuoteContract contract = {0};
    contract.contractId = 123U;
    strcpy(contract.exchange, "SMART");
    uint32_t request = 99U;
    if (strcmp(mode, "old-protocol") == 0)
    {
        fixture->c->snapshot.protocolVersion = 163;
        CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 10U, &request) ==
              UMI_STATUS_NOT_IMPLEMENTED);
        CHECK(request == 99U && fixture->c->contractDetails.requestId == 0U);
        return 0;
    }
    if (strcmp(mode, "invalid") == 0)
    {
        contract.contractId = 0U;
        CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 10U, &request) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(request == 99U);
        return 0;
    }
    if (strcmp(mode, "queue-full") == 0)
    {
        fixture->c->txSize = UMI_IBKR_TX_LIMIT;
        CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 10U, &request) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(request == 99U && fixture->c->contractDetails.requestId == 0U);
        return 0;
    }
    size_t before = fixture->outSize;
    CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 10U, &request) == UMI_STATUS_OK &&
          request == 36000U);
    if (strcmp(mode, "wire") == 0)
    {
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        const char *expected[] = {"9",     "8", "36000", "123", "", "",  "", "0", "", "",
                                  "SMART", "",  "",      "",    "", "0", "", "",  ""};
        const unsigned char *wire = fixture->output + before;
        size_t bytes = fixture->outSize - before;
        CHECK(bytes >= 4U);
        size_t length =
            ((size_t)wire[0] << 24U) | ((size_t)wire[1] << 16U) | ((size_t)wire[2] << 8U) | wire[3];
        CHECK(length == bytes - 4U);
        size_t used = 4U;
        for (size_t index = 0U; index < sizeof expected / sizeof expected[0]; ++index)
        {
            CHECK(used < bytes && memchr(wire + used, 0, bytes - used) != NULL);
            CHECK(strcmp((const char *)wire + used, expected[index]) == 0);
            used += strlen(expected[index]) + 1U;
        }
        CHECK(used == bytes);
        const char *order[] = {"3", "1"};
        CHECK(UmiIbkrQueueFields(fixture->c, order, 2U) == UMI_STATUS_PERMISSION_DENIED);
        return 0;
    }
    if (strcmp(mode, "shared-identity") == 0)
    {
        uint32_t quote = 0U;
        CHECK(UmiIbkrQuoteSubscribe(fixture->c, &contract, 10U, &quote) == UMI_STATUS_OK && quote == 36001U);
        return 0;
    }
    if (strcmp(mode, "busy") == 0)
    {
        uint32_t another = 99U;
        CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 11U, &another) == UMI_STATUS_BUSY);
        CHECK(another == 99U);
        return 0;
    }
    if (strcmp(mode, "timeout") == 0)
    {
        CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 10010U, copy) == UMI_STATUS_OK);
        CHECK(copy->failed && copy->stale && !copy->complete);
        CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 10010U, &request) == UMI_STATUS_OK &&
              request == 36001U);
        return 0;
    }
    if (strcmp(mode, "refusal") == 0)
    {
        FEED(fixture, "4", "2", "36000", "200", "Invented contract refusal");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->failed && copy->providerCode == 200 && fixture->c->snapshot.state == UMI_IBKR_READY);
        return 0;
    }
    if (strcmp(mode, "abandon") == 0 || strcmp(mode, "late-error") == 0)
    {
        CHECK(UmiIbkrContractDetailsAbandon(fixture->c, request) == UMI_STATUS_OK);
        if (strcmp(mode, "late-error") == 0)
        {
            CHECK(UmiIbkrContractDetailsRequest(fixture->c, &contract, 11U, &request) == UMI_STATUS_OK);
            FEED(fixture, "4", "2", "36000", "200", "Late retired lookup refusal");
        }
        else
            CHECK(Detail(fixture, "36000", "123", "SMART", "valid") == 0);
        CHECK(UmiIbkrConnectionPump(fixture->c, 12U) == UMI_STATUS_OK);
        CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 12U, copy) == UMI_STATUS_OK &&
              copy->count == 0U);
        return 0;
    }
    if (strcmp(mode, "bond") == 0)
    {
        FEED(fixture, "18", "36000");
        CHECK(UmiIbkrConnectionPump(fixture->c, 11U) == UMI_STATUS_OK);
        CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 11U, copy) == UMI_STATUS_OK && copy->failed);
        return 0;
    }
    CHECK(Detail(fixture, strcmp(mode, "foreign") == 0 ? "39999" : "36000",
                 strcmp(mode, "wrong-contract") == 0 ? "124" : "123", "SMART", mode) == 0);
    UmiStatus status = UmiIbkrConnectionPump(fixture->c, 11U);
    bool invalid = strcmp(mode, "malformed") == 0 || strcmp(mode, "numeric") == 0 ||
                   strcmp(mode, "invalid-utf8") == 0 || strcmp(mode, "wrong-contract") == 0 ||
                   strcmp(mode, "long-name") == 0;
    if (invalid)
    {
        CHECK(status ==
              (strcmp(mode, "long-name") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR));
        CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 11U, copy) == UMI_STATUS_OK);
        CHECK(copy->count == 0U && copy->stale && !copy->complete);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 11U, copy) == UMI_STATUS_OK && !copy->complete);
    if (strcmp(mode, "foreign") == 0)
    {
        CHECK(copy->count == 0U);
        return 0;
    }
    CHECK(copy->count == 1U);
    CHECK(copy->items[0].contractId == 123U && strcmp(copy->items[0].minimumSize, "1") == 0);
    CHECK(strcmp(copy->items[0].orderTypes, "LMT,MKT") == 0);
    CHECK(strcmp(copy->items[0].marketRuleIds, "26,26") == 0);
    if (strcmp(mode, "duplicate") == 0)
    {
        CHECK(Detail(fixture, "36000", "123", "SMART", "valid") == 0);
        CHECK(UmiIbkrConnectionPump(fixture->c, 12U) == UMI_STATUS_OK);
    }
    FEED(fixture, "52", "1", "36000");
    CHECK(UmiIbkrConnectionPump(fixture->c, 13U) == UMI_STATUS_OK);
    CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 13U, copy) == UMI_STATUS_OK);
    CHECK(copy->count == 1U && copy->complete && copy->completedAtMilliseconds == 13U && !copy->stale);
    if (strcmp(mode, "disconnected") == 0)
    {
        UmiIbkrConnectionClose(fixture->c);
        CHECK(UmiIbkrContractDetailsCopy(fixture->c, request, 14U, copy) == UMI_STATUS_OK);
        CHECK(copy->complete && copy->stale);
    }
    return 0;
}
int main(int argc, char **argv)
{
    (void)PositionFeed; /* This fixture helper is used by sibling observation checks. */
    if (argc != 2)
        return 2;
    Fixture *fixture = New();
    UmiIbkrContractDetailsSnapshot *copy = calloc(1U, sizeof *copy);
    if (fixture == NULL || copy == NULL)
    {
        Delete(fixture);
        free(copy);
        return 1;
    }
    int result = Run(fixture, argv[1], copy);
    Delete(fixture);
    free(copy);
    return result;
}
