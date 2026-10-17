/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_log_status.c
 * PURPOSE: Verify bounded status evidence and unchanged diagnostic sink ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

typedef struct LogProbe {
    unsigned calls;
    UmiDiagnosticSeverity severity;
    uint64_t correlation;
    char message[256];
    char source[128];
} LogProbe;

/* Sink arguments are borrowed. Copy them synchronously, as a real store or
 * console consumer must do before the formatting call returns. */
static void capture(const UmiDiagnostic *event, void *data)
{
    LogProbe *probe = data;
    ++probe->calls;
    probe->severity = event->severity;
    probe->correlation = event->correlation_id;
    (void)snprintf(probe->message, sizeof probe->message, "%s", event->message);
    (void)snprintf(probe->source, sizeof probe->source, "%s", event->source);
}

int main(void)
{
    LogProbe probe = {0};
    UmiLogger logger = {capture, &probe, "application-startup"};
    umi_log_status(&logger, "adapter.prepare", UMI_STATUS_OK, 1U);
    CHECK(probe.calls == 0U);
    umi_log_status(&logger, "adapter.prepare", UMI_STATUS_INVALID_STATE, 42U);
    CHECK(probe.calls == 1U && probe.severity == UMI_DIAGNOSTIC_ERROR);
    CHECK(probe.correlation == 42U);
    CHECK(strcmp(probe.source, "application-startup") == 0);
    CHECK(strcmp(probe.message, "Operation adapter.prepare failed: Invalid state (2).") == 0);

    /* Failure output preserves the numeric status even when a future producer
     * reports a value that this Framework build does not yet name. */
    umi_log_status(&logger, "runtime_create-2", (UmiStatus)123, 43U);
    CHECK(strstr(probe.message, "Unknown status (123)") != NULL);
    umi_log_status(&logger, "runtime.allocate", UMI_STATUS_OUT_OF_MEMORY, 44U);
    CHECK(strstr(probe.message, "Out of memory (6)") != NULL);

    const char *invalid[] = {NULL, "", "line\nforged", "C:/private/project", "user name", "caf\xc3\xa9"};
    for (size_t index = 0U; index < sizeof invalid / sizeof invalid[0]; ++index) {
        unsigned before = probe.calls;
        umi_log_status(&logger, invalid[index], UMI_STATUS_IO_ERROR, 50U + index);
        CHECK(probe.calls == before + 1U);
        CHECK(strcmp(probe.message, "Operation unspecified-operation failed: Input/output error (8).") == 0);
    }
    char operation[98];
    memset(operation, 'x', 96U);
    operation[96] = '\0';
    umi_log_status(&logger, operation, UMI_STATUS_TIMEOUT, 60U);
    CHECK(strstr(probe.message, operation) != NULL);
    operation[96] = 'x';
    operation[97] = '\0';
    umi_log_status(&logger, operation, UMI_STATUS_TIMEOUT, 61U);
    CHECK(strstr(probe.message, "unspecified-operation") != NULL);
    unsigned before = probe.calls;
    umi_log_status(NULL, "runtime.create", UMI_STATUS_INVALID_STATE, 0U);
    UmiLogger absent = {NULL, &probe, "application-startup"};
    umi_log_status(&absent, "runtime.create", UMI_STATUS_INVALID_STATE, 0U);
    CHECK(probe.calls == before);
    return 0;
}
