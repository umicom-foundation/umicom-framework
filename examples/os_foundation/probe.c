/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/os_foundation/probe.c
 *
 * PURPOSE:
 *   Exercise normal user-space Data Server transactions after independent OS boot.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/data/data_server.h"
#include "umicom/platform/boot_report.h"
#include <stdio.h>
#include <string.h>

static int DataCheck(void)
{
    UmiDataServer *server = NULL; char text[32]; int outcome = 1;
    if (umi_data_server_create_memory(&server) != UMI_STATUS_OK) return 1;
    if (umi_data_server_set(server, "boot.check", "original") != UMI_STATUS_OK) goto done;
    if (umi_data_server_begin(server) != UMI_STATUS_OK) goto done;
    if (umi_data_server_set(server, "boot.check", "uncommitted") != UMI_STATUS_OK) goto done;
    if (umi_data_server_rollback(server) != UMI_STATUS_OK) goto done;
    if (umi_data_server_get(server, "boot.check", text, sizeof text) != UMI_STATUS_OK ||
        strcmp(text, "original") != 0) goto done;
    if (umi_data_server_begin(server) != UMI_STATUS_OK ||
        umi_data_server_set(server, "boot.check", "committed") != UMI_STATUS_OK ||
        umi_data_server_commit(server) != UMI_STATUS_OK) goto done;
    if (umi_data_server_get(server, "boot.check", text, sizeof text) != UMI_STATUS_OK ||
        strcmp(text, "committed") != 0 || umi_data_server_count(server) != 1U) goto done;
    outcome = 0;
done:
    umi_data_server_destroy(server);
    return outcome;
}
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--self-test") == 0) {
        if (DataCheck() != 0) return 1;
        puts("Framework memory Data Server: commit and rollback passed. No guest boot was attempted.");
        return 0;
    }
    if (argc != 1) { fputs("Use --self-test for a host-only memory check.\n", stderr); return 2; }
    FILE *file = fopen("/run/umicom/boot.report", "rb");
    if (!file) { fputs("No guest boot report. The host-only check is --self-test.\n", stderr); return 3; }
    char bytes[UMI_BOOT_REPORT_MAX_BYTES + 1U];
    size_t count = fread(bytes, 1, sizeof bytes, file); int failed = ferror(file);
    if (fclose(file) != 0 || failed || count > UMI_BOOT_REPORT_MAX_BYTES) return 4;
    UmiBootReport report;
    if (UmiBootReportParse(bytes, count, &report) != UMI_STATUS_OK ||
        report.mode != UMI_BOOT_REPORT_NORMAL || report.state != UMI_BOOT_REPORT_STARTING) return 5;
    if (DataCheck() != 0) return 6;
    puts("Framework guest probe: normal user-space commit and rollback passed.");
    return 0;
}
