/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_image/test_profile.c
 *
 * PURPOSE:
 *   Verify profile grammar, immutable ownership and preservation of prepared build context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/os_image/profile.h"
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do { if (!(e)) { fprintf(stderr, "check %d: %s\n", __LINE__, #e); return 1; } } while (0)
static const char Prefix[] =
    "UMICOM_OS_PROFILE\t1\n"
    "buildroot\t0123456789abcdef0123456789abcdef01234567\n"
    "linux\t6.12.109\n"
    "linux-sha256\t0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\n";
static const char Files[] = "file\tframework/notice\nfile\tumicomOS/image/native/profile.umi\n";
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char bytes[2048];
    int written = snprintf(bytes, sizeof bytes, "%s%s", Prefix, Files);
    CHECK(written > 0 && (size_t)written < sizeof bytes);
    size_t size = (size_t)written;
    UmiOsImageProfile *owner = NULL;
    CHECK(UmiOsImageProfileOpen(bytes, size, &owner) == UMI_STATUS_OK);
    UmiOsImageProfileInfo info;
    CHECK(UmiOsImageProfileInfoRead(owner, &info) == UMI_STATUS_OK);
    const char *name = argv[1];
    if (!strcmp(name, "valid")) {
        CHECK(info.fileCount == 2 && !strcmp(info.linuxVersion, "6.12.109"));
        CHECK(!strcmp(info.buildrootCommit, "0123456789abcdef0123456789abcdef01234567"));
        CHECK(strlen(info.profileSha256) == 64);
        CHECK(!strcmp(UmiOsImageProfileFileAt(owner, 0), "framework/notice"));
        CHECK(!strcmp(UmiOsImageProfileFileAt(owner, 1), "umicomOS/image/native/profile.umi"));
    } else if (!strcmp(name, "ownership")) {
        memset(bytes, '?', size);
        CHECK(!strcmp(UmiOsImageProfileFileAt(owner, 0), "framework/notice"));
        CHECK(UmiOsImageProfileInfoRead(owner, &info) == UMI_STATUS_OK);
        CHECK(!strcmp(info.linuxVersion, "6.12.109"));
    } else if (!strcmp(name, "bounds")) {
        CHECK(!UmiOsImageProfileFileAt(owner, 2));
        CHECK(!UmiOsImageProfileFileAt(owner, SIZE_MAX));
        CHECK(!UmiOsImageProfileFileAt(NULL, 0));
    } else if (!strcmp(name, "arguments")) {
        CHECK(UmiOsImageProfileOpen(NULL, size, &owner) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOsImageProfileOpen(bytes, 0, &owner) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOsImageProfileOpen(bytes, size, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiOsImageProfileInfoRead(owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        unsigned char before[sizeof info]; memcpy(before, &info, sizeof info);
        CHECK(UmiOsImageProfileInfoRead(NULL, &info) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(before, &info, sizeof info));
        CHECK(UmiOsImageProfileRead("relative.umi", &owner) == UMI_STATUS_INVALID_ARGUMENT);
        UmiOsImageProfileDestroy(NULL);
    } else if (!strcmp(name, "prepare-preservation")) {
        OiPlan *plan = calloc(1, sizeof *plan);
        CHECK(plan);
        plan->arch = UMI_OS_IMAGE_X86_64;
        strcpy(plan->git, "/trusted/git"); strcpy(plan->buildroot, "/trusted/buildroot");
        CHECK(OiProfileDecode(bytes, size, plan) == UMI_STATUS_OK);
        CHECK(plan->arch == UMI_OS_IMAGE_X86_64 && !strcmp(plan->git, "/trusted/git"));
        CHECK(!strcmp(plan->buildroot, "/trusted/buildroot") && plan->count == 2);
        OiPlan *before = malloc(sizeof *before); CHECK(before); memcpy(before, plan, sizeof *plan);
        bytes[size - 1] = 'x';
        CHECK(OiProfileDecode(bytes, size, plan) == UMI_STATUS_PARSE_ERROR);
        CHECK(!memcmp(before, plan, sizeof *plan));
        free(before); free(plan);
    } else if (!strcmp(name, "truncated")) {
        for (size_t n = 1; n < size; ++n) {
            UmiOsImageProfile *unchanged = owner;
            CHECK(UmiOsImageProfileOpen(bytes, n, &unchanged) != UMI_STATUS_OK);
            CHECK(unchanged == owner);
        }
    } else {
        if (!strcmp(name, "missing-self")) snprintf(bytes, sizeof bytes, "%sfile\tframework/notice\n", Prefix);
        else if (!strcmp(name, "duplicate")) snprintf(bytes, sizeof bytes, "%s%sfile\tumicomOS/image/native/profile.umi\n", Prefix, Files);
        else if (!strcmp(name, "unsorted")) snprintf(bytes, sizeof bytes, "%sfile\tumicomOS/image/native/profile.umi\nfile\tframework/notice\n", Prefix);
        else if (!strcmp(name, "escape")) snprintf(bytes, sizeof bytes, "%sfile\tframework/../secret\nfile\tumicomOS/image/native/profile.umi\n", Prefix);
        else if (!strcmp(name, "unknown-owner")) snprintf(bytes, sizeof bytes, "%sfile\toutside/notice\nfile\tumicomOS/image/native/profile.umi\n", Prefix);
        else if (!strcmp(name, "extra-field")) snprintf(bytes, sizeof bytes, "%s%scommand\trun\n", Prefix, Files);
        else if (!strcmp(name, "empty-files")) snprintf(bytes, sizeof bytes, "%s", Prefix);
        else if (!strcmp(name, "uppercase-hash")) { char *hash = strstr(bytes, "buildroot\t"); CHECK(hash); hash[9] = 'A'; }
        else if (!strcmp(name, "version")) { char *version = strstr(bytes, "linux\t"); CHECK(version); version[6] = 'x'; }
        else if (!strcmp(name, "embedded-nul")) bytes[10] = 0;
        else if (!strcmp(name, "control")) bytes[10] = '\r';
        else { UmiOsImageProfileDestroy(owner); return 2; }
        if (strcmp(name, "embedded-nul")) size = strlen(bytes);
        UmiOsImageProfile *unchanged = owner;
        CHECK(UmiOsImageProfileOpen(bytes, size, &unchanged) == UMI_STATUS_PARSE_ERROR);
        CHECK(unchanged == owner);
    }
    UmiOsImageProfileDestroy(owner);
    return 0;
}
