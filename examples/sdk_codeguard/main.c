/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_codeguard/main.c
 *
 * PURPOSE:
 *   Consume installed CodeGuard services from an independent C23 application and distinguish scan completion from a severity gate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "umicom/codeguard/codeguard.h"

/* This client owns one service. Framework owns scanning, findings, reporting
 * and severity policy; no scanner or repository checkout is embedded here. */
/* Demonstrate installed shared cancellation and distinguish a cancelled attempt from failed analysis or a completed severity gate. The previous implementation remains for review. */
#if 0
int main(int argc, char **argv)
{
    if (argc != 2) {
        fputs("Usage: umicom-sdk-codeguard <workspace-root>\n", stderr);
        return 2;
    }
    UmiCodeGuardService *service = NULL;
    UmiStatus status = umi_codeguard_service_create(argv[1], &service);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot create CodeGuard service (status %d).\n", (int)status);
        return 2;
    }
    UmiCodeGuardProfile profile = umi_codeguard_profile_security();
    status = umi_codeguard_service_scan(service, &profile);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Scan did not complete (status %d); no current report published.\n",
            (int)status);
        umi_codeguard_service_destroy(service);
        return 3;
    }
    const UmiCodeGuardResult *result = umi_codeguard_service_result(service);
    status = umi_codeguard_report_write(stdout, UMI_CODEGUARD_REPORT_TEXT, result);
    UmiCodeGuardSummary summary = umi_codeguard_service_summary(service);
    int failed_gate = umi_codeguard_summary_failed(&summary, profile.fail_on);
    umi_codeguard_service_destroy(service);
    if (status != UMI_STATUS_OK || fflush(stdout) != 0) {
        fputs("Could not finish writing the CodeGuard report.\n", stderr);
        return 3;
    }
    return failed_gate ? 1 : 0;
}
#endif
int main(int argc, char **argv)
{
    if ((argc != 2 && argc != 3) ||
        (argc == 3 && strcmp(argv[2], "--cancel-before-scan") != 0)) {
        fputs("Usage: umicom-sdk-codeguard <workspace-root> [--cancel-before-scan]\n", stderr);
        return 2;
    }
    UmiCodeGuardService *service = NULL;
    UmiStatus status = umi_codeguard_service_create(argv[1], &service);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot create CodeGuard service (status %d).\n", (int)status);
        return 2;
    }
    UmiCancellationToken *cancellation = NULL;
    status = umi_cancellation_token_create(&cancellation);
    if (status != UMI_STATUS_OK) {
        umi_codeguard_service_destroy(service);
        fputs("Cannot create cancellation token.\n", stderr);
        return 2;
    }
    /* A deterministic lifecycle demonstration; this console example creates
     * no background worker and installs no asynchronous signal handler. */
    if (argc == 3) umi_cancellation_token_request(cancellation);
    UmiCodeGuardProfile profile = umi_codeguard_profile_security();
    status = umi_codeguard_service_scan_cancellable(service, &profile, cancellation);
    umi_cancellation_token_destroy(cancellation);
    if (status == UMI_STATUS_CANCELLED) {
        fputs("Scan cancelled; the previous complete report is unchanged.\n", stderr);
        umi_codeguard_service_destroy(service);
        return 4;
    }
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Scan did not complete (status %d); no current report published.\n",
            (int)status);
        umi_codeguard_service_destroy(service);
        return 3;
    }
    const UmiCodeGuardResult *result = umi_codeguard_service_result(service);
    status = umi_codeguard_report_write(stdout, UMI_CODEGUARD_REPORT_TEXT, result);
    UmiCodeGuardSummary summary = umi_codeguard_service_summary(service);
    int failed_gate = umi_codeguard_summary_failed(&summary, profile.fail_on);
    umi_codeguard_service_destroy(service);
    if (status != UMI_STATUS_OK || fflush(stdout) != 0) {
        fputs("Could not finish writing the CodeGuard report.\n", stderr);
        return 3;
    }
    return failed_gate ? 1 : 0;
}
