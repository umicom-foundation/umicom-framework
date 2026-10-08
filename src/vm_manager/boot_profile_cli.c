/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vm_manager/boot_profile_cli.c
 * PURPOSE: Keep named boot settings local and route launches through fresh input review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "boot_internal.h"
#include "umicom/vm_manager/boot_profile.h"

static UmiStatus ProfileList(const UmiVmBootProfile *profile, void *context)
{
    (void)context;
    printf("%s | %s | revision %" PRIu64 "\n", profile->id, profile->name, profile->revision);
    return UMI_STATUS_OK;
}

static int CopyProfileText(char *out, size_t capacity, const char *text)
{
    if (!text || strlen(text) >= capacity) return 0;
    strcpy(out, text);
    return 1;
}

int VmBootProfilesMain(int argc, char **argv)
{
    if (!argc || (argc == 1 && strcmp(argv[0], "--help") == 0)) {
        puts("Usage: umicom qemu profiles list --database FILE\n"
             "       umicom qemu profiles save --database FILE --id ID --name NAME\n"
             "         [--expected REVISION] <the same boot options used by qemu plan>\n"
             "       umicom qemu profiles show|review|run|remove --database FILE --id ID\n"
             "         [--expected REVISION (remove)] [--expect SHA256 (run)]\n"
             "Use an absolute local database path. Save starts no process.\n"
             "Existing profiles need their current --expected revision to be replaced.");
        return 0;
    }
    if (argc < 1 || (argc - 1) % 2) return 2;
    int save = strcmp(argv[0], "save") == 0;
    int list = strcmp(argv[0], "list") == 0;
    int show = strcmp(argv[0], "show") == 0;
    int review = strcmp(argv[0], "review") == 0;
    int run = strcmp(argv[0], "run") == 0;
    int remove = strcmp(argv[0], "remove") == 0;
    if (!save && !list && !show && !review && !run && !remove) return 2;
    const char *database = NULL, *id = NULL, *name = NULL, *expectedText = NULL, *fingerprint = NULL;
    char *bootOptions[33];
    int bootCount = 1;
    bootOptions[0] = "plan";
    /* Separate profile metadata from boot options. The shared boot parser still
     * owns duplicate detection and request validation for all launch fields. */
    for (int index = 1; index < argc; index += 2) {
        const char **field = NULL;
        if (strcmp(argv[index], "--database") == 0) field = &database;
        else if (strcmp(argv[index], "--id") == 0) field = &id;
        else if (strcmp(argv[index], "--name") == 0) field = &name;
        else if (strcmp(argv[index], "--expected") == 0) field = &expectedText;
        else if (strcmp(argv[index], "--expect") == 0) field = &fingerprint;
        if (field) {
            if (*field) return 2;
            *field = argv[index + 1];
        } else {
            if (!save || bootCount + 2 > (int)(sizeof bootOptions / sizeof bootOptions[0])) return 2;
            bootOptions[bootCount++] = argv[index];
            bootOptions[bootCount++] = argv[index + 1];
        }
    }
    uint64_t expected = 0U;
    if (!database || !VmPath(database, 0) || (!list && !VmId(id)) ||
        (list && id) || (save ? !name : name != NULL) ||
        ((!save && !remove) && expectedText) || (remove && !expectedText) ||
        (expectedText && !VmNumber(expectedText, &expected)) ||
        (save && expected == UINT64_MAX) || (remove && expected == 0U) ||
        (name && (!name[0] || !VmUtf8(name, sizeof ((UmiVmBootProfile *)0)->name))) ||
        (run ? !VmHash(fingerprint) : fingerprint != NULL)) return 2;

    UmiVmBootProfile profile = {0};
    const char *unusedFingerprint = NULL;
    if (save && (!CopyProfileText(profile.id, sizeof profile.id, id) ||
        !CopyProfileText(profile.name, sizeof profile.name, name) ||
        !VmBootParseRequest(bootCount, bootOptions, &profile.request, &unusedFingerprint))) return 2;
    /* Refuse invalid commands before opening a database, so a mistyped option
     * cannot create a surprising empty file. Opening never authenticates online. */
    UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_sqlite(database, &server);
    UmiVmBootPlan *plan = NULL;
    UmiVmSession *session = NULL;
    UmiVmReport report = {0};
    int result = 0;
    if (status == UMI_STATUS_OK && save) {
        uint64_t revision = 0U;
        status = UmiVmBootProfileSave(server, &profile, expected, &revision);
        if (status == UMI_STATUS_OK) printf("Saved %s at revision %" PRIu64 ". No VM started.\n", id, revision);
    } else if (status == UMI_STATUS_OK && list) {
        status = UmiVmBootProfileVisit(server, ProfileList, NULL);
    } else if (status == UMI_STATUS_OK && remove) {
        status = UmiVmBootProfileRemove(server, id, expected);
        if (status == UMI_STATUS_OK) puts("Profile removed. Guest files and running sessions are unchanged.");
    } else if (status == UMI_STATUS_OK) {
        status = UmiVmBootProfileLoad(server, id, &profile);
        if (status == UMI_STATUS_OK) status = UmiVmBootPlanCreate(&profile.request, &plan);
        if (status == UMI_STATUS_OK) {
            (void)ProfileList(&profile, NULL);
            VmBootPrintPlan(plan);
            if (review) status = UmiVmBootReview(plan, &report);
            else if (run) status = UmiVmBootStart(plan, fingerprint, &session, &report);
        }
    }
    /* Release the database before entering a long-running console. The session
     * uses its copied plan and never retains a live configuration connection. */
    umi_data_server_destroy(server);
    if (report.detail[0]) puts(report.detail);
    if (status == UMI_STATUS_OK && review) printf("Fingerprint: %s\n", report.fingerprint);
    if (status == UMI_STATUS_OK && session) result = VmSessionConsole(session);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Boot profile: %s\n", UmiSetupStatusText(status));
        result = 1;
    }
    UmiVmSessionDestroy(session);
    UmiVmBootPlanDestroy(plan);
    return result;
}
