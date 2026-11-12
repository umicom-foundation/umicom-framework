/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_refusal_report.c
 *
 * PURPOSE:
 *   Verify early media refusals publish matching reports before any source or device acquisition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include "umicom/boot_media/media.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static int refuse_allocation;
static void *ReviewCalloc(size_t count, size_t bytes)
{
    if (refuse_allocation) return NULL;
    return calloc(count, bytes);
}
/* Allocation refusal is injected into the real review implementation. Every
 * other input below is rejected before file/device acquisition; no synthetic
 * path is ever opened as a native device. */
#define calloc ReviewCalloc
#include "../../src/boot_media/transfer.c"
#undef calloc

static UmiStatus Invoke(const char *mode, UmiBootMediaReport *report)
{
    UmiBootMediaImage image;
    UmiBootMediaPlan *output = (UmiBootMediaPlan *)(void *)&image;
    UmiStatus status;
    const char *source = "relative-source.img", *target = "relative-target.img";
#ifdef _WIN32
    const char *absolute_source = "C:/umicom-media-fixture/input.img";
    const char *absolute_target = "C:/umicom-media-fixture/output.img";
#else
    const char *absolute_source = "/umicom-media-fixture/input.img";
    const char *absolute_target = "/umicom-media-fixture/output.img";
#endif
    if (!strcmp(mode, "inspect-no-output"))
        return UmiBootMediaInspect(source, NULL, NULL, NULL, report);
    if (!strcmp(mode, "inspect-null-path") || !strcmp(mode, "inspect-relative"))
        return UmiBootMediaInspect(!strcmp(mode, "inspect-null-path") ? NULL : source,
                                  &image, NULL, NULL, report);
    if (!strncmp(mode, "review-", 7U)) {
        UmiBootMediaPlan **out = &output;
        char long_target[UMI_BOOT_MEDIA_PATH + 1U];
        if (!strcmp(mode, "review-null-output")) out = NULL;
        else if (!strcmp(mode, "review-null-source")) source = NULL;
        else if (!strcmp(mode, "review-relative-source")) {}
        else if (!strcmp(mode, "review-null-target")) { source = absolute_source; target = NULL; }
        else if (!strcmp(mode, "review-relative-target")) source = absolute_source;
        else if (!strcmp(mode, "review-long-target")) {
            source = absolute_source;
            memset(long_target, 'x', sizeof(long_target));
            long_target[sizeof(long_target) - 1U] = '\0';
            target = long_target;
        } else if (!strcmp(mode, "review-no-memory")) {
            source = absolute_source; target = absolute_target; refuse_allocation = 1;
        } else return UMI_STATUS_OK;
        status = UmiBootMediaReviewFile(source, target, out, NULL, NULL, report);
        if (out != NULL) assert(output == NULL);
        refuse_allocation = 0;
        return status;
    }
    if (!strncmp(mode, "device-", 7U)) {
        UmiBootMediaDevice device = {0};
        const UmiBootMediaDevice *input = &device;
        UmiBootMediaPlan **out = &output;
        if (!strcmp(mode, "device-null")) input = NULL;
        else if (!strcmp(mode, "device-path")) memset(device.path, 'x', sizeof(device.path));
        else if (!strcmp(mode, "device-identity")) memset(device.identity, 'x', sizeof(device.identity));
        else if (!strcmp(mode, "device-model")) memset(device.model, 'x', sizeof(device.model));
        else if (!strcmp(mode, "device-serial")) memset(device.serial, 'x', sizeof(device.serial));
        else if (!strcmp(mode, "device-reason")) memset(device.reason, 'x', sizeof(device.reason));
        else if (!strcmp(mode, "device-null-output")) out = NULL;
        else return UMI_STATUS_OK;
        status = UmiBootMediaReviewDevice(source, input, out, NULL, NULL, report);
        if (out != NULL) assert(output == NULL);
        return status;
    }
    if (!strncmp(mode, "apply-", 6U)) {
        UmiBootMediaPlan plan = {0};
        strcpy(plan.fingerprint, "fixture");
        strcpy(plan.confirmation, "fixture");
        UmiBootMediaPlan *input = &plan;
        const char *fingerprint = plan.fingerprint, *confirmation = plan.confirmation;
        if (!strcmp(mode, "apply-null-plan")) input = NULL;
        else if (!strcmp(mode, "apply-null-fingerprint")) fingerprint = NULL;
        else if (!strcmp(mode, "apply-null-confirmation")) confirmation = NULL;
        else if (!strcmp(mode, "apply-mismatch")) fingerprint = "different";
        else if (!strcmp(mode, "apply-consumed")) plan.consumed = 1;
        else return UMI_STATUS_OK;
        return UmiBootMediaApply(input, fingerprint, confirmation, NULL, NULL, report);
    }
    if (!strcmp(mode, "verify-null-source") || !strcmp(mode, "verify-relative-source"))
        return UmiBootMediaVerifyFile(!strcmp(mode, "verify-null-source") ? NULL : source,
                                      target, NULL, NULL, report);
    if (!strcmp(mode, "discover-null-output"))
        return UmiBootMediaDiscover(NULL, report);
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiBootMediaReport report;
    memset(&report, 0x5a, sizeof(report));
    report.status = UMI_STATUS_OK;
    report.phase = UMI_BOOT_MEDIA_FINISHED;
    report.outputCreated = report.writeStarted = report.verified = 1;
    UmiStatus expected = !strcmp(argv[1], "review-no-memory") ? UMI_STATUS_OUT_OF_MEMORY :
        !strncmp(argv[1], "apply-", 6U) ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = Invoke(argv[1], &report);
    assert(status == expected && report.status == status);
    assert(!report.outputCreated && !report.writeStarted && !report.verified);
    assert(report.bytesDone == 0U && report.bytesTotal == 0U);
    assert(report.detail[0] != '\0' && memchr(report.detail, 0, sizeof(report.detail)) != NULL);
    /* Optional diagnostics never change refusal semantics. */
    assert(Invoke(argv[1], NULL) == expected);
    return 0;
}
