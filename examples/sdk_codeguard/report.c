/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_codeguard/report.c
 *
 * PURPOSE:
 *   Demonstrate bounded report projection owned independently of an installed CodeGuard service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/codeguard.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>

/* A console can perform a synchronous scan. A GUI instead obtains the same
 * borrowed result from its current-context review service on the owner thread. */
int main(int argc, char **argv)
{
    UmiCodeGuardReportFormat format;
    if ((argc != 3 && argc != 4) ||
        (strcmp(argv[2], "text") != 0 && strcmp(argv[2], "json") != 0 &&
         strcmp(argv[2], "sarif") != 0)) {
        fputs("Usage: umicom-sdk-codeguard-report <workspace-root> text|json|sarif [output-file]\n", stderr);
        return 2;
    }
    format = strcmp(argv[2], "text") == 0 ? UMI_CODEGUARD_REPORT_TEXT :
        strcmp(argv[2], "json") == 0 ? UMI_CODEGUARD_REPORT_JSON : UMI_CODEGUARD_REPORT_SARIF;
    UmiCodeGuardService *service = NULL;
    UmiStatus status = umi_codeguard_service_create(argv[1], &service);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot create scan service (status %d).\n", (int)status);
        return 2;
    }
    UmiCodeGuardProfile profile = umi_codeguard_profile_security();
    status = umi_codeguard_service_scan(service, &profile);
    char *projection = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = umi_codeguard_report_render(format, umi_codeguard_service_result(service),
            1024U * 1024U, &projection, &size);
    UmiCodeGuardSummary summary = umi_codeguard_service_summary(service);
    int failed_gate = umi_codeguard_summary_failed(&summary, profile.fail_on);
    /* The successful projection owns its bytes; it survives service teardown.
     * No finding pointer or service-owned array is retained by the client. */
    umi_codeguard_service_destroy(service);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Scan or report projection failed (status %d); output unchanged.\n", (int)status);
        return 3;
    }
    if (argc == 4) status = umi_fs_write_bytes(argv[3], projection, size);
    else if (fwrite(projection, 1U, size, stdout) != size || fflush(stdout) != 0)
        status = UMI_STATUS_IO_ERROR;
    umi_codeguard_report_free(projection);
    if (status != UMI_STATUS_OK) {
        fputs("Report output failed; a partial destination may remain.\n", stderr);
        return 3;
    }
    return failed_gate ? 1 : 0;
}
