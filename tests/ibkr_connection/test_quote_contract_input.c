/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_quote_contract_input.c
 *
 * PURPOSE:
 *   Verify bounded contract input parsing and unchanged outputs on rejected identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/quotes.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiIbkrQuoteContract value;
    memset(&value, 0x5a, sizeof(value));
    UmiIbkrQuoteContract before = value;
    if (strcmp(argv[1], "valid") == 0) {
        CHECK(UmiIbkrQuoteContractParse("0000265598", "SMART", &value) == UMI_STATUS_OK);
        CHECK(value.contractId == 265598U && strcmp(value.exchange, "SMART") == 0);
    } else if (strcmp(argv[1], "maximum") == 0) {
        char maximum[24], exchange[64];
        (void)snprintf(maximum, sizeof(maximum), "%d", INT_MAX);
        memset(exchange, 'A', sizeof(exchange)); exchange[63] = '\0';
        CHECK(UmiIbkrQuoteContractParse(maximum, exchange, &value) == UMI_STATUS_OK);
        CHECK(value.contractId == (uint32_t)INT_MAX && strlen(value.exchange) == 63U);
    } else if (strcmp(argv[1], "alias") == 0) {
        memcpy(value.exchange, "SMART", 6U);
        CHECK(UmiIbkrQuoteContractParse("1", value.exchange, &value) == UMI_STATUS_OK);
        CHECK(value.contractId == 1U && strcmp(value.exchange, "SMART") == 0);
    } else if (strcmp(argv[1], "invalid-id") == 0) {
        const char *ids[] = {"", "0", "-1", "+1", " 1", "1 ", "1.0", "1e2", "2147483648", "00000000001", "１", "1\n"};
        for (size_t i = 0U; i < sizeof(ids) / sizeof(ids[0]); ++i) {
            CHECK(UmiIbkrQuoteContractParse(ids[i], "SMART", &value) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&value, &before, sizeof(value)) == 0);
        }
    } else if (strcmp(argv[1], "invalid-exchange") == 0) {
        const char *exchanges[] = {"", "SMART\n", "SMART\t", "\xff", "\xc0\x80"};
        for (size_t i = 0U; i < sizeof(exchanges) / sizeof(exchanges[0]); ++i) {
            CHECK(UmiIbkrQuoteContractParse("1", exchanges[i], &value) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&value, &before, sizeof(value)) == 0);
        }
    } else if (strcmp(argv[1], "unterminated") == 0) {
        char id[11], exchange[64];
        memset(id, '1', sizeof(id)); memset(exchange, 'A', sizeof(exchange));
        CHECK(UmiIbkrQuoteContractParse(id, "SMART", &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrQuoteContractParse("1", exchange, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    } else if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiIbkrQuoteContractParse(NULL, "SMART", &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrQuoteContractParse("1", NULL, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrQuoteContractParse("1", "SMART", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&value, &before, sizeof(value)) == 0);
    } else return 2;
    return 0;
}
