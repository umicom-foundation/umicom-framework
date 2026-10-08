/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_boot_profiles.c
 * PURPOSE: Verify persistent boot requests, conflict handling and failure isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vm_manager/boot_profile.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#define PROFILE_PROCESS_ID ((unsigned long)GetCurrentProcessId())
#define ROOT_PATH "C:/VM practice/"
#else
#include <unistd.h>
#define PROFILE_PROCESS_ID ((unsigned long)getpid())
#define ROOT_PATH "/tmp/VM practice/"
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

static void Profile(UmiVmBootProfile *profile)
{
    memset(profile, 0, sizeof *profile);
    strcpy(profile->id, "umicom");
    strcpy(profile->name, "My Umicom system");
    (void)UmiVmBootRequestInit(&profile->request, UMI_VM_BOOT_UMICOM_KERNEL);
    strcpy(profile->request.executable, ROOT_PATH "qemu");
    strcpy(profile->request.workingDirectory, ROOT_PATH "runs");
    strcpy(profile->request.firmware, ROOT_PATH "caf\xc3\xa9.elf");
}

/* Count values while exercising callback lifetime. A cancellation result must
 * propagate without changing either the catalogue or its transaction state. */
static UmiStatus Count(const UmiVmBootProfile *profile, void *context)
{
    if (strcmp(profile->id, "umicom") != 0) return UMI_STATUS_INVALID_STATE;
    ++*(unsigned *)context;
    return UMI_STATUS_OK;
}
static UmiStatus Cancel(const UmiVmBootProfile *profile, void *context)
{
    (void)profile;
    (void)context;
    return UMI_STATUS_CANCELLED;
}

int main(int argc, char **argv)
{
    CHECK(argc >= 2);
    const char *test = argv[1];
    UmiDataServer *server = NULL;
    UmiVmBootProfile profile, loaded;
    Profile(&profile);
    uint64_t revision = 0U;
    if (strcmp(test, "sqlite") == 0) {
        CHECK(argc == 3);
        char database[UMI_VM_PATH];
        int length = snprintf(database, sizeof database, "%s/boot-profile-%lu.sqlite",
            argv[2], PROFILE_PROCESS_ID);
        CHECK(length > 0 && (size_t)length < sizeof database);
        UmiStatus status = umi_data_server_create_sqlite(database, &server);
        if (status == UMI_STATUS_UNAVAILABLE) return 77;
        CHECK(status == UMI_STATUS_OK);
        CHECK(UmiVmBootProfileSave(server, &profile, 0U, &revision) == UMI_STATUS_OK);
        umi_data_server_destroy(server);
        CHECK(umi_data_server_create_sqlite(database, &server) == UMI_STATUS_OK);
        CHECK(UmiVmBootProfileLoad(server, "umicom", &loaded) == UMI_STATUS_OK);
        CHECK(loaded.revision == 1U && strcmp(loaded.request.firmware, profile.request.firmware) == 0);
        umi_data_server_destroy(server);
        return 0;
    }
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(umi_data_server_set(server, "other.module", "preserved") == UMI_STATUS_OK);
    CHECK(UmiVmBootProfileSave(server, &profile, 0U, &revision) == UMI_STATUS_OK && revision == 1U);
    if (strcmp(test, "roundtrip") == 0) {
        CHECK(UmiVmBootProfileLoad(server, "umicom", &loaded) == UMI_STATUS_OK);
        CHECK(strcmp(loaded.name, profile.name) == 0);
        CHECK(strcmp(loaded.request.firmware, profile.request.firmware) == 0);
        CHECK(loaded.request.memoryMiB == 128U && loaded.request.processors == 1U && loaded.revision == 1U);
    } else if (strcmp(test, "conflict") == 0) {
        strcpy(profile.name, "Reviewed name");
        CHECK(UmiVmBootProfileSave(server, &profile, 0U, &revision) == UMI_STATUS_INVALID_STATE);
        CHECK(revision == 0U);
        CHECK(UmiVmBootProfileSave(server, &profile, 1U, &revision) == UMI_STATUS_OK && revision == 2U);
        CHECK(UmiVmBootProfileSave(server, &profile, 1U, &revision) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiVmBootProfileLoad(server, "umicom", &loaded) == UMI_STATUS_OK);
        CHECK(strcmp(loaded.name, profile.name) == 0 && loaded.revision == 2U);
    } else if (strcmp(test, "remove") == 0) {
        CHECK(UmiVmBootProfileRemove(server, "umicom", 2U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiVmBootProfileRemove(server, "umicom", 1U) == UMI_STATUS_OK);
        CHECK(UmiVmBootProfileLoad(server, "umicom", &loaded) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(test, "borrowed-transaction") == 0) {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiVmBootProfileSave(server, &profile, 1U, &revision) == UMI_STATUS_BUSY);
        CHECK(UmiVmBootProfileRemove(server, "umicom", 1U) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    } else if (strcmp(test, "visit") == 0) {
        unsigned count = 0U;
        CHECK(UmiVmBootProfileVisit(server, Count, &count) == UMI_STATUS_OK && count == 1U);
        CHECK(UmiVmBootProfileVisit(server, Cancel, NULL) == UMI_STATUS_CANCELLED);
    } else if (strcmp(test, "corrupt") == 0 || strcmp(test, "key-mismatch") == 0 ||
               strcmp(test, "trailing") == 0) {
        char record[4096];
        CHECK(umi_data_server_get(server, "vm.boot.profile.umicom", record, sizeof record) == UMI_STATUS_OK);
        if (strcmp(test, "corrupt") == 0) record[0] = 'X';
        if (strcmp(test, "trailing") == 0) strcat(record, "extra\tignored\n");
        const char *key = strcmp(test, "key-mismatch") == 0 ? "vm.boot.profile.other" : "vm.boot.profile.umicom";
        CHECK(umi_data_server_set(server, key, record) == UMI_STATUS_OK);
        memset(&loaded, 0x5a, sizeof loaded);
        unsigned char before[sizeof loaded];
        memcpy(before, &loaded, sizeof loaded);
        CHECK(UmiVmBootProfileLoad(server, strcmp(test, "key-mismatch") == 0
            ? "other" : "umicom", &loaded) != UMI_STATUS_OK);
        CHECK(memcmp(before, &loaded, sizeof loaded) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (unsigned index = 1U; index < UMI_VM_MAX_PROFILES; ++index) {
            (void)snprintf(profile.id, sizeof profile.id, "system-%u", index);
            CHECK(UmiVmBootProfileSave(server, &profile, 0U, &revision) == UMI_STATUS_OK);
        }
        strcpy(profile.id, "overflow");
        CHECK(UmiVmBootProfileSave(server, &profile, 0U, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiVmBootProfileLoad(server, "overflow", &loaded) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(test, "backend-capacity") == 0) {
        /* Memory Data Server records have a smaller value capacity than SQLite.
         * Encoding overflow must roll back the update and retain the old value. */
        (void)UmiVmBootRequestInit(&profile.request, UMI_VM_BOOT_LINUX_X86_64);
        strcpy(profile.request.executable, ROOT_PATH "qemu");
        strcpy(profile.request.workingDirectory, ROOT_PATH "runs");
        strcpy(profile.request.kernel, ROOT_PATH "Image");
        memset(profile.request.commandLine, 'a', sizeof profile.request.commandLine - 1U);
        profile.request.commandLine[sizeof profile.request.commandLine - 1U] = 0;
        CHECK(UmiVmBootProfileSave(server, &profile, 1U, &revision) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiVmBootProfileLoad(server, "umicom", &loaded) == UMI_STATUS_OK && loaded.revision == 1U);
        CHECK(loaded.request.target == UMI_VM_BOOT_UMICOM_KERNEL);
    } else if (strcmp(test, "invalid") == 0) {
        CHECK(UmiVmBootProfileSave(server, &profile, UINT64_MAX, &revision) == UMI_STATUS_INVALID_ARGUMENT);
        profile.request.processors = 2U;
        CHECK(UmiVmBootProfileSave(server, &profile, 1U, &revision) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiVmBootProfileLoad(server, "../umicom", &loaded) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    CHECK(!umi_data_server_in_transaction(server));
    char retained[32];
    CHECK(umi_data_server_get(server, "other.module", retained, sizeof retained) == UMI_STATUS_OK);
    CHECK(strcmp(retained, "preserved") == 0);
    umi_data_server_destroy(server);
    return 0;
}
