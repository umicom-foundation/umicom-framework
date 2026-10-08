/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_session.c
 * PURPOSE: Bind reviewed files to a paused QEMU session using existing process ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "boot_internal.h"
#include "umicom/vm_manager/qmp_channel.h"

/* Length prefixes make argument boundaries unambiguous even when a path
 * contains punctuation. Hash actual strings, never structure padding. */
static UmiStatus BootHashText(UmiNativeSha256 *hash, const char *text)
{
    char length[32];
    int count = snprintf(length, sizeof length, "%zu:", strlen(text));
    if (count < 0 || (size_t)count >= sizeof length) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiNativeSha256Update(hash, length, (size_t)count);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Update(hash, text, strlen(text));
    return status;
}

UmiStatus UmiVmBootReview(const UmiVmBootPlan *plan, UmiVmReport *report)
{
    if (!report) return UMI_STATUS_INVALID_ARGUMENT;
    memset(report, 0, sizeof *report);
    if (!plan) return VmReport(report, UMI_STATUS_INVALID_ARGUMENT, "A boot plan is required.");
    UmiStatus status = UmiSetupFileCheck(plan->request.workingDirectory, 1, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiSetupNativeProgramCheck(plan->request.executable, NULL);
    UmiNativeSha256 hash;
    UmiNativeSha256Init(&hash);
    if (status == UMI_STATUS_OK) status = BootHashText(&hash, "umicom-qemu-boot");
    if (status == UMI_STATUS_OK) status = BootHashText(&hash, plan->request.workingDirectory);
    const char *inputs[] = {plan->request.executable, plan->request.firmware,
        plan->request.kernel, plan->request.initrd, plan->request.disk, plan->request.iso};
    /* FileCheck and Digest share the checked file layer: devices, links and
     * directories cannot masquerade as raw guest images. No format is guessed. */
    for (size_t index = 0; status == UMI_STATUS_OK && index < sizeof inputs / sizeof inputs[0]; ++index) {
        status = BootHashText(&hash, inputs[index]);
        if (status != UMI_STATUS_OK || !inputs[index][0]) continue;
        status = UmiSetupFileCheck(inputs[index], 0, NULL);
        char digest[65];
        uint64_t bytes = 0;
        if (status == UMI_STATUS_OK) status = UmiSetupFileDigest(inputs[index], digest, &bytes, NULL);
        if (status == UMI_STATUS_OK && bytes == 0U) status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK) status = BootHashText(&hash, digest);
    }
    for (size_t index = 0; status == UMI_STATUS_OK && index < plan->argumentCount; ++index)
        status = BootHashText(&hash, plan->arguments[index]);
    unsigned char digest[UMI_NATIVE_SHA256_BYTES];
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Final(&hash, digest);
    if (status == UMI_STATUS_OK) UmiNativeSha256Hex(digest, report->fingerprint);
    return VmReport(report, status, status == UMI_STATUS_OK
        ? "Boot inputs reviewed. No process started. Shared libraries and implicit firmware are not included."
        : "Boot review stopped. Check the executable, working directory and nonempty regular input files.");
}

UmiStatus UmiVmBootStart(const UmiVmBootPlan *plan, const char *expectedFingerprint,
                       UmiVmSession **outSession, UmiVmReport *report)
{
    if (outSession) *outSession = NULL;
    if (!report) return UMI_STATUS_INVALID_ARGUMENT;
    /* The expected value may come from this same report. Copy it before Review
     * clears the report, so reusing an output object does not erase approval. */
    char expected[65] = {0};
    if (VmHash(expectedFingerprint)) strcpy(expected, expectedFingerprint);
    memset(report, 0, sizeof *report);
    if (!plan || !outSession || !expected[0])
        return VmReport(report, UMI_STATUS_INVALID_ARGUMENT, "Review this plan before starting it.");
    UmiStatus status = UmiVmBootReview(plan, report);
    if (status == UMI_STATUS_OK && strcmp(expected, report->fingerprint) != 0)
        return VmReport(report, UMI_STATUS_INVALID_STATE, "Boot inputs changed since review. Review the new inputs.");

    const char *arguments[UMI_CHANNEL_MAX_ARGUMENTS];
    for (size_t index = 0; index < plan->argumentCount; ++index)
        arguments[index] = plan->arguments[index];
    UmiProcessChannel *channel = NULL;
    if (status == UMI_STATUS_OK) {
        UmiProcessChannelRequest request = {plan->request.executable, arguments,
            plan->argumentCount, plan->request.workingDirectory};
        status = UmiProcessChannelOpen(&request, &channel);
        if (status == UMI_STATUS_OK) report->processLaunched = 1;
    }
    if (status == UMI_STATUS_OK) {
        status = UmiVmQmpAdoptChannel(channel, outSession, report);
        /* Adoption takes ownership only on success. The failure path must still
         * destroy the channel; a failed handshake cannot leave an orphan QEMU. */
        if (status == UMI_STATUS_OK) channel = NULL;
    }
    if (status == UMI_STATUS_OK) {
        UmiVmSnapshot observation;
        status = UmiVmObserve(*outSession, &observation);
        if (status == UMI_STATUS_OK && (!observation.processRunning ||
            observation.guestRunning || !observation.controlAvailable ||
            (strcmp(observation.state, "paused") != 0 &&
             strcmp(observation.state, "prelaunch") != 0)))
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status != UMI_STATUS_OK) {
        UmiVmSessionDestroy(*outSession);
        *outSession = NULL;
    }
    UmiProcessChannelDestroy(channel);
    return VmReport(report, status, status == UMI_STATUS_OK
        ? "QEMU is connected and paused. Resume starts the guest; disk changes are temporary."
        : "QEMU start stopped. No successful guest boot is claimed.");
}
