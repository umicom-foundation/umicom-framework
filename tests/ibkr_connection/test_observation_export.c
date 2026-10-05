/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_observation_export.c
 * PURPOSE: Exercise captured report formatting and new-file writing without broker traffic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/observation_export.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
/* Invented provider observations are never sent to a connection. Decimal
 * spellings deliberately include trailing zeros and a fractional position. */
static void Sample(UmiIbkrConnectionSnapshot *snapshot)
{
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->state = UMI_IBKR_READY;
    snapshot->readOnly = true;
    snapshot->requestedEnvironment = UMI_TRADING_PAPER;
    snapshot->accountCount = 1U;
    strcpy(snapshot->accounts[0], "DEMO-ACCOUNT");
    strcpy(snapshot->selectedAccount, "DEMO-ACCOUNT");
    snapshot->requestIssued = true;
    snapshot->summaryComplete = true;
    snapshot->positionsComplete = true;
    snapshot->requestedAtMilliseconds = 10U;
    snapshot->summaryAtMilliseconds = 20U;
    snapshot->positionsAtMilliseconds = 30U;
    snapshot->valueCount = 1U;
    strcpy(snapshot->values[0].tag, "NetLiquidation");
    strcpy(snapshot->values[0].value, "12345.6700");
    strcpy(snapshot->values[0].currency, "GBP");
    snapshot->positionCount = 1U;
    UmiIbkrPositionObservation *position = &snapshot->positions[0];
    strcpy(position->contractId, "12345");
    strcpy(position->symbol, "DEMO");
    strcpy(position->securityType, "STK");
    strcpy(position->exchange, "DEMO");
    strcpy(position->currency, "USD");
    strcpy(position->quantity, "-2.500");
    strcpy(position->averageCost, "101.2500");
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    int failed = 0, owned = 0;
    UmiIbkrConnectionSnapshot *snapshot = malloc(sizeof(*snapshot));
    UmiCsvDocument *document = NULL;
    UmiCancellationToken *cancel = NULL;
    unsigned char *read = NULL;
    char root[UMI_PATH_CAPACITY] = {0}, destination[UMI_PATH_CAPACITY];
    CHECK(snapshot != NULL);
    Sample(snapshot);
    bool accepted = true;
    if (strcmp(name, "partial") == 0)
    {
        snapshot->summaryComplete = false;
        snapshot->positionsComplete = false;
    }
    if (strcmp(name, "disconnected") == 0)
    {
        snapshot->state = UMI_IBKR_DISCONNECTED;
        snapshot->stale = true;
    }
    if (strcmp(name, "empty") == 0)
    {
        snapshot->positionCount = 0U;
        snapshot->valueCount = 0U;
    }
    if (strcmp(name, "live-label") == 0)
        snapshot->requestedEnvironment = UMI_TRADING_LIVE;
    if (strcmp(name, "unicode") == 0)
        strcpy(snapshot->positions[0].symbol, "caf\xc3\xa9");
    if (strcmp(name, "quoted") == 0)
        strcpy(snapshot->values[0].value, "first,\"quoted\"\r\nsecond");
    if (strcmp(name, "formula") == 0)
        strcpy(snapshot->values[0].value, "=1+2");
    if (strcmp(name, "unlisted") == 0)
    {
        strcpy(snapshot->selectedAccount, "ANOTHER");
        accepted = false;
    }
    if (strcmp(name, "duplicate-account") == 0)
    {
        snapshot->accountCount = 2U;
        strcpy(snapshot->accounts[1], snapshot->accounts[0]);
        accepted = false;
    }
    if (strcmp(name, "no-request") == 0)
    {
        snapshot->requestIssued = false;
        accepted = false;
    }
    if (strcmp(name, "writable") == 0)
    {
        snapshot->readOnly = false;
        accepted = false;
    }
    if (strcmp(name, "attested") == 0)
    {
        snapshot->environmentAttested = true;
        accepted = false;
    }
    if (strcmp(name, "simulation") == 0)
    {
        snapshot->requestedEnvironment = UMI_TRADING_SIMULATION;
        accepted = false;
    }
    if (strcmp(name, "not-ready") == 0)
    {
        snapshot->state = UMI_IBKR_CONNECTING;
        accepted = false;
    }
    if (strcmp(name, "future-summary") == 0)
    {
        snapshot->summaryAtMilliseconds = 41U;
        accepted = false;
    }
    if (strcmp(name, "early-position") == 0)
    {
        snapshot->positionsAtMilliseconds = 9U;
        accepted = false;
    }
    if (strcmp(name, "backwards-capture") == 0)
    {
        snapshot->requestedAtMilliseconds = 41U;
        accepted = false;
    }
    if (strcmp(name, "account-limit") == 0)
    {
        snapshot->accountCount = UMI_IBKR_ACCOUNT_LIMIT + 1U;
        accepted = false;
    }
    if (strcmp(name, "value-limit") == 0)
    {
        snapshot->valueCount = UMI_IBKR_VALUE_LIMIT + 1U;
        accepted = false;
    }
    if (strcmp(name, "position-limit") == 0)
    {
        snapshot->positionCount = UMI_IBKR_POSITION_LIMIT + 1U;
        accepted = false;
    }
    if (strcmp(name, "unterminated") == 0)
    {
        memset(snapshot->positions[0].averageCost, 'x', sizeof(snapshot->positions[0].averageCost));
        accepted = false;
    }
    if (strcmp(name, "invalid-utf8") == 0)
    {
        strcpy(snapshot->values[0].value, "\xc0\xaf");
        accepted = false;
    }
    if (strcmp(name, "maximum") == 0)
    {
        snapshot->positionCount = UMI_IBKR_POSITION_LIMIT;
        snapshot->valueCount = UMI_IBKR_VALUE_LIMIT;
        for (size_t i = 1U; i < snapshot->positionCount; ++i)
            snapshot->positions[i] = snapshot->positions[0];
        for (size_t i = 1U; i < snapshot->valueCount; ++i)
            snapshot->values[i] = snapshot->values[0];
    }
    UmiStatus status = UmiIbkrObservationsExportCsv(snapshot, 40U, &document);
    CHECK((status == UMI_STATUS_OK) == accepted && (document != NULL) == accepted);
    if (!accepted)
        goto cleanup;
    CHECK(UmiCsvDocumentRows(document) == 2U + snapshot->valueCount + snapshot->positionCount);
    const char *text = UmiCsvDocumentData(document);
    CHECK(strstr(text, "\"metadata\",\"DEMO-ACCOUNT\"") != NULL);
    CHECK(strstr(text, "requested_monotonic_ms") != NULL);
    if (strcmp(name, "formula") == 0)
        CHECK(strstr(text, "\"'=1+2\"") != NULL);
    if (strcmp(name, "quoted") == 0)
        CHECK(strstr(text, "first,\"\"quoted\"\"\r\nsecond") != NULL);
    if (strcmp(name, "unicode") == 0)
        CHECK(strstr(text, "caf\xc3\xa9") != NULL);
    if (strcmp(name, "partial") == 0)
        CHECK(strstr(text, "\"false\",\"false\",\"10\",\"\",\"\",\"40\"") != NULL);
    if (strcmp(name, "live-label") == 0)
        CHECK(strstr(text, "\"live\",\"false\"") != NULL);
    if (strcmp(name, "valid") == 0)
        CHECK(strstr(text, "12345.6700") != NULL && strstr(text, "'-2.500") != NULL &&
              strstr(text, "101.2500") != NULL);
    if (strcmp(name, "independent") == 0)
    {
        free(snapshot);
        snapshot = NULL;
        CHECK(strstr(text, "DEMO-ACCOUNT") != NULL);
    }
    if (strncmp(name, "write-", 6U) == 0)
    {
        char temporary[UMI_PATH_CAPACITY], leaf[100];
#ifdef _WIN32
        unsigned long process = (unsigned long)GetCurrentProcessId();
#else
        unsigned long process = (unsigned long)getpid();
#endif
        int length = snprintf(leaf, sizeof(leaf), "umicom-observation-%lu-%s", process, name);
        CHECK(length > 0 && (size_t)length < sizeof(leaf) && strchr(name, '/') == NULL &&
              strchr(name, '\\') == NULL);
        CHECK(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
        CHECK(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK && !umi_fs_exists(root));
        CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
        owned = 1;
        CHECK(umi_path_join(root, "report.csv", destination, sizeof(destination)) == UMI_STATUS_OK);
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        if (strcmp(name, "write-cancel") == 0)
            umi_cancellation_token_request(cancel);
        if (strcmp(name, "write-existing") == 0)
            CHECK(umi_fs_write_text(destination, "keep") == UMI_STATUS_OK);
        status = UmiIbkrObservationsWriteNew(document, destination, cancel);
        if (strcmp(name, "write-cancel") == 0)
            CHECK(status == UMI_STATUS_CANCELLED && !umi_fs_exists(destination));
        else
        {
            size_t size = 0U;
            CHECK(UmiInputFileRead(destination, UMI_CSV_MAX_BYTES, &read, &size) == UMI_STATUS_OK);
            if (strcmp(name, "write-existing") == 0)
                CHECK(status == UMI_STATUS_ALREADY_EXISTS && size == 4U && memcmp(read, "keep", size) == 0);
            else
                CHECK(status == UMI_STATUS_OK && size == UmiCsvDocumentBytes(document) &&
                      memcmp(read, text, size) == 0);
        }
    }
cleanup:
    UmiInputFileFree(read);
    umi_cancellation_token_destroy(cancel);
    UmiCsvDocumentDestroy(document);
    free(snapshot);
    if (owned && umi_fs_remove_tree(root) != UMI_STATUS_OK)
        failed = 1;
    return failed;
}
