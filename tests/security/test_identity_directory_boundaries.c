/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/security/test_identity_directory_boundaries.c
 *
 * PURPOSE:
 *   Verify public identity directory bounds and unchanged state after rejected edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/identity_directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

static UmiSecurityIdentityProfile profile(void)
{
    UmiSecurityIdentityProfile value = {0};
    (void)snprintf(value.identity_id, sizeof value.identity_id, "%s", "reader");
    (void)snprintf(value.display_name, sizeof value.display_name, "%s", "Learning reader");
    value.kind = UMI_IDENTITY_HUMAN;
    value.enabled = true;
    return value;
}

/* Byte snapshots make the failure contract observable, including metadata that
 * callers cannot infer from a successful lookup alone. */
static int run_case(const char *name, UmiSecurityIdentityDirectory *directory,
                    UmiSecurityIdentityDirectory *before)
{
    UmiSecurityIdentityProfile value = profile();
    if (strcmp(name, "fields") == 0) {
        for (size_t i = 0U; i < 3U; ++i) {
            UmiSecurityIdentityProfile bad = value;
            char *fields[] = {bad.identity_id, bad.display_name, bad.organisation};
            const size_t sizes[] = {sizeof bad.identity_id, sizeof bad.display_name, sizeof bad.organisation};
            memset(fields[i], 'x', sizes[i]);
            memcpy(before, directory, sizeof *before);
            CHECK(umi_security_identity_directory_add(directory, &bad) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(before, directory, sizeof *before) == 0);
        }
        value.identity_id[0] = '\0';
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_INVALID_ARGUMENT);
        value = profile(); value.display_name[0] = '\0';
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_INVALID_ARGUMENT);
        value = profile(); value.kind = (UmiIdentityKind)0;
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_INVALID_ARGUMENT);
        value = profile();
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
    } else if (strcmp(name, "maximum") == 0) {
        char *fields[] = {value.identity_id, value.display_name, value.organisation};
        const size_t sizes[] = {sizeof value.identity_id, sizeof value.display_name, sizeof value.organisation};
        for (size_t i = 0U; i < 3U; ++i) {
            memset(fields[i], 'a' + (int)i, sizes[i] - 1U);
            fields[i][sizes[i] - 1U] = '\0';
        }
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
        const UmiSecurityIdentityProfile *found = umi_security_identity_directory_find(directory, value.identity_id);
        CHECK(found != NULL && strcmp(found->organisation, value.organisation) == 0);
        CHECK(found->revision == 1U && directory->revision == 1U);
    } else if (strcmp(name, "count") == 0) {
        directory->count = SIZE_MAX;
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_find(directory, "reader") == NULL);
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", false) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
    } else if (strcmp(name, "corrupt-row") == 0) {
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
        (void)snprintf(value.identity_id, sizeof value.identity_id, "%s", "second");
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
        memset(directory->items[1].organisation, 'x', sizeof directory->items[1].organisation);
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_find(directory, "reader") == NULL);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", false) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
    } else if (strcmp(name, "lookup") == 0) {
        char unclosed[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
        memset(unclosed, 'x', sizeof unclosed);
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_find(directory, unclosed) == NULL);
        CHECK(umi_security_identity_directory_find(directory, "") == NULL);
        CHECK(umi_security_identity_directory_set_enabled(directory, unclosed, false) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_set_enabled(directory, "", false) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_set_enabled(directory, "missing", false) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
    } else if (strcmp(name, "revision") == 0) {
        directory->revision = UINT64_MAX;
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
        directory->revision = 0U;
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
        directory->items[0].revision = UINT64_MAX;
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", false) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
        directory->items[0].revision = 1U; directory->revision = UINT64_MAX;
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", true) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
    } else if (strcmp(name, "capacity") == 0) {
        for (size_t i = 0U; i < UMI_SECURITY_GOVERNANCE_MAX_IDENTITIES; ++i) {
            (void)snprintf(value.identity_id, sizeof value.identity_id, "reader-%zu", i);
            CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_OK);
        }
        memcpy(before, directory, sizeof *before);
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_ALREADY_EXISTS);
        (void)snprintf(value.identity_id, sizeof value.identity_id, "%s", "extra");
        CHECK(umi_security_identity_directory_add(directory, &value) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, directory, sizeof *before) == 0);
    } else if (strcmp(name, "lifecycle") == 0) {
        directory->items[0] = value;
        CHECK(umi_security_identity_directory_add(directory, &directory->items[0]) == UMI_STATUS_OK);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", false) == UMI_STATUS_OK);
        CHECK(!directory->items[0].enabled && directory->items[0].revision == 2U && directory->revision == 2U);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", false) == UMI_STATUS_OK);
        CHECK(directory->items[0].revision == 3U && directory->revision == 3U);
        CHECK(umi_security_identity_directory_set_enabled(directory, "reader", true) == UMI_STATUS_OK);
        CHECK(directory->items[0].enabled && directory->items[0].revision == 4U && directory->revision == 4U);
    } else if (strcmp(name, "null") == 0) {
        CHECK(umi_security_identity_directory_find(NULL, "reader") == NULL);
        CHECK(umi_security_identity_directory_find(directory, NULL) == NULL);
        CHECK(umi_security_identity_directory_add(NULL, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_add(directory, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_set_enabled(NULL, "reader", true) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_security_identity_directory_set_enabled(directory, NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiSecurityIdentityDirectory *directory = calloc(1U, sizeof *directory);
    UmiSecurityIdentityDirectory *before = calloc(1U, sizeof *before);
    if (directory == NULL || before == NULL) { free(directory); free(before); return 1; }
    const int result = run_case(argv[1], directory, before);
    free(before);
    free(directory);
    return result;
}
