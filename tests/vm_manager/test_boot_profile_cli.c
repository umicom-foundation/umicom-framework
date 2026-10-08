/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/test_boot_profile_cli.c
 * PURPOSE: Exercise named profile commands and reject invalid metadata before database access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vm_manager/boot_profile.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#define PROFILE_CLI_PID ((unsigned long)GetCurrentProcessId())
#define ROOT_PATH "C:/VM practice/"
#else
#include <unistd.h>
#define PROFILE_CLI_PID ((unsigned long)getpid())
#define ROOT_PATH "/tmp/VM practice/"
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)

int main(int argc, char **argv)
{
    CHECK(argc >= 2);
    char database[UMI_VM_PATH];
    strcpy(database, ROOT_PATH "must-not-be-created.sqlite");
    char *options[] = {"profiles", "save", "--database", database, "--id", "umicom",
        "--name", "My system", "--target", "umicom-kernel", "--qemu", ROOT_PATH "qemu",
        "--directory", ROOT_PATH "runs", "--firmware", ROOT_PATH "system.elf", NULL, NULL};
    if (strcmp(argv[1], "flow") == 0) {
        CHECK(argc == 3);
        UmiDataServer *server = NULL;
        UmiStatus available = umi_data_server_create_sqlite(":memory:", &server);
        if (available == UMI_STATUS_UNAVAILABLE) return 77;
        CHECK(available == UMI_STATUS_OK);
        umi_data_server_destroy(server);
        int length = snprintf(database, sizeof database, "%s/boot-cli-%lu.sqlite",
            argv[2], PROFILE_CLI_PID);
        CHECK(length > 0 && (size_t)length < sizeof database);
        CHECK(UmiVmBootMain(16, options) == 0);
        char *list[] = {"profiles", "list", "--database", database};
        char *show[] = {"profiles", "show", "--database", database, "--id", "umicom"};
        CHECK(UmiVmBootMain(4, list) == 0 && UmiVmBootMain(6, show) == 0);
        CHECK(UmiVmBootMain(16, options) == 1);
        options[16] = "--expected"; options[17] = "1"; options[7] = "Updated name";
        CHECK(UmiVmBootMain(18, options) == 0);
        CHECK(umi_data_server_create_sqlite(database, &server) == UMI_STATUS_OK);
        UmiVmBootProfile loaded;
        CHECK(UmiVmBootProfileLoad(server, "umicom", &loaded) == UMI_STATUS_OK);
        CHECK(loaded.revision == 2U && strcmp(loaded.name, "Updated name") == 0);
        umi_data_server_destroy(server);
        char *removeProfile[] = {"profiles", "remove", "--database", database,
            "--id", "umicom", "--expected", "2"};
        CHECK(UmiVmBootMain(8, removeProfile) == 0);
        CHECK(UmiVmBootMain(6, show) == 1);
        return 0;
    }
    int count = 16;
    if (strcmp(argv[1], "empty-name") == 0) options[7] = "";
    else if (strcmp(argv[1], "bad-id") == 0) options[5] = "../umicom";
    else if (strcmp(argv[1], "relative-database") == 0) options[3] = "relative.sqlite";
    else if (strcmp(argv[1], "duplicate-database") == 0) {
        options[count++] = "--database"; options[count++] = database;
    } else if (strcmp(argv[1], "overflow-revision") == 0) {
        options[count++] = "--expected"; options[count++] = "18446744073709551615";
    } else if (strcmp(argv[1], "save-with-approval") == 0) {
        options[count++] = "--expect";
        options[count++] = "0000000000000000000000000000000000000000000000000000000000000000";
    } else if (strcmp(argv[1], "missing-target") == 0) options[8] = "--unknown";
    else return 2;
    CHECK(UmiVmBootMain(count, options) == 2);
    return 0;
}
