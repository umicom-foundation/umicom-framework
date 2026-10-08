/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_boot_session.c
 * PURPOSE: Check file review and owned child cleanup with an inert QMP peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vm_manager/boot.h"
#include "umicom/setup_centre/files.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#define BOOT_PROCESS_ID ((unsigned long)GetCurrentProcessId())
#else
#include <unistd.h>
#define BOOT_PROCESS_ID ((unsigned long)getpid())
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

static int Join(char *out, size_t capacity, const char *root, const char *name)
{
    int length = snprintf(out, capacity, "%s/%s", root, name);
    return length > 0 && (size_t)length < capacity;
}

int main(int argc, char **argv)
{
    CHECK(argc == 4);
    const char *test = argv[1];
    char directory[UMI_VM_PATH], leaf[96];
    UmiStatus created = UMI_STATUS_ALREADY_EXISTS;
    /* Every process owns a new test directory. Retained fixtures let failures
     * be inspected and never collide with a developer's VM files. */
    for (unsigned attempt = 0; attempt < 100U && created == UMI_STATUS_ALREADY_EXISTS; ++attempt) {
        int length = snprintf(leaf, sizeof leaf, "boot-%s-%lu-%u", test, BOOT_PROCESS_ID, attempt);
        CHECK(length > 0 && (size_t)length < sizeof leaf);
        CHECK(Join(directory, sizeof directory, argv[3], leaf));
        created = UmiSetupDirectoryCreate(directory, NULL);
    }
    CHECK(created == UMI_STATUS_OK);
    UmiVmBootRequest request;
    CHECK(UmiVmBootRequestInit(&request, UMI_VM_BOOT_UMICOM_KERNEL) == UMI_STATUS_OK);
    CHECK(strlen(directory) < sizeof request.workingDirectory);
    strcpy(request.workingDirectory, directory);
    CHECK(Join(request.executable, sizeof request.executable, directory, "boot-peer.exe"));
    char digest[65];
    uint64_t bytes = 0;
    CHECK(UmiSetupFileDigest(argv[2], digest, &bytes, NULL) == UMI_STATUS_OK);
    CHECK(UmiSetupFileCopyChecked(argv[2], request.executable, digest, bytes, NULL) == UMI_STATUS_OK);
    CHECK(UmiSetupFileGrantOwnerExecute(request.executable, NULL) == UMI_STATUS_OK);
    const char *firmware = strcmp(test, "running-child") == 0 ? "running.fw" :
        strcmp(test, "bad-handshake") == 0 ? "malformed.fw" : "system.fw";
    CHECK(Join(request.firmware, sizeof request.firmware, directory, firmware));
    CHECK(UmiSetupFileWriteNew(request.firmware, "firmware fixture", 16U, NULL) == UMI_STATUS_OK);
    UmiVmBootPlan *plan = NULL;
    CHECK(UmiVmBootPlanCreate(&request, &plan) == UMI_STATUS_OK);
    UmiVmReport report;
    CHECK(UmiVmBootReview(plan, &report) == UMI_STATUS_OK);
    CHECK(!report.processLaunched && !report.outputCreated && strlen(report.fingerprint) == 64U);
    char expected[65];
    strcpy(expected, report.fingerprint);
    if (strcmp(test, "stable-review") == 0) {
        CHECK(UmiVmBootReview(plan, &report) == UMI_STATUS_OK);
        CHECK(strcmp(expected, report.fingerprint) == 0);
    } else if (strcmp(test, "changed-options") == 0) {
        UmiVmBootPlan *changed = NULL;
        request.memoryMiB = 256U;
        CHECK(UmiVmBootPlanCreate(&request, &changed) == UMI_STATUS_OK);
        UmiVmSession *session = NULL;
        CHECK(UmiVmBootStart(changed, expected, &session, &report) == UMI_STATUS_INVALID_STATE);
        CHECK(!report.processLaunched && session == NULL);
        UmiVmBootPlanDestroy(changed);
    } else if (strcmp(test, "changed-input") == 0 || strcmp(test, "empty-input") == 0) {
        /* Mutate only this test's owned fixture, after the first review. */
        FILE *file = fopen(request.firmware, "wb");
        CHECK(file != NULL);
        if (strcmp(test, "empty-input") != 0) CHECK(fwrite("changed", 1U, 7U, file) == 7U);
        CHECK(fclose(file) == 0);
        UmiVmSession *session = NULL;
        CHECK(UmiVmBootStart(plan, expected, &session, &report) != UMI_STATUS_OK);
        CHECK(!report.processLaunched && session == NULL);
    } else if (strcmp(test, "bad-fingerprint") == 0) {
        UmiVmSession *session = NULL;
        CHECK(UmiVmBootStart(plan, "not-a-digest", &session, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(session == NULL && !report.processLaunched);
    } else if (strcmp(test, "running-child") == 0 || strcmp(test, "bad-handshake") == 0) {
        UmiVmSession *session = NULL;
        CHECK(UmiVmBootStart(plan, expected, &session, &report) != UMI_STATUS_OK);
        CHECK(session == NULL && report.processLaunched);
    } else if (strcmp(test, "lifecycle") == 0 || strcmp(test, "report-alias") == 0) {
        UmiVmSession *session = NULL;
        CHECK(UmiVmBootStart(plan, strcmp(test, "report-alias") == 0
            ? report.fingerprint : expected, &session, &report) == UMI_STATUS_OK);
        UmiVmSnapshot snapshot;
        CHECK(session != NULL && report.processLaunched);
        CHECK(UmiVmObserve(session, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.processRunning && !snapshot.guestRunning && snapshot.controlAvailable);
        /* Session strings no longer borrow the plan once the process opens. */
        UmiVmBootPlanDestroy(plan);
        plan = NULL;
        CHECK(UmiVmControl(session, UMI_VM_RESUME, NULL, 0U, NULL, 0U, NULL, &report) == UMI_STATUS_OK);
        CHECK(UmiVmObserve(session, &snapshot) == UMI_STATUS_OK && snapshot.guestRunning);
        CHECK(UmiVmControl(session, UMI_VM_PAUSE, NULL, 0U, NULL, 0U, NULL, &report) == UMI_STATUS_OK);
        unsigned char console[UMI_VM_CONSOLE_CHUNK];
        size_t read = 0;
        CHECK(UmiVmControl(session, UMI_VM_CONSOLE_READ, NULL, 0U, console,
            sizeof console, &read, &report) == UMI_STATUS_OK);
        CHECK(read == 14U && memcmp(console, "guest fixture\n", read) == 0);
        CHECK(UmiVmControl(session, UMI_VM_QUIT, NULL, 0U, NULL, 0U, NULL, &report) == UMI_STATUS_OK);
        UmiVmSessionDestroy(session);
    } else {
        UmiVmBootPlanDestroy(plan);
        return 2;
    }
    UmiVmBootPlanDestroy(plan);
    return 0;
}
