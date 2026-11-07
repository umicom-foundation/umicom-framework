/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/security/test_credential_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded credential references, rejected input and registry preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/credential.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto finish; } } while (0)

static UmiCredentialRecord example_record(void)
{
    UmiCredentialRecord record = {0};
    (void)snprintf(record.credential_id, sizeof record.credential_id, "%s", "example.connection");
    (void)snprintf(record.principal_id, sizeof record.principal_id, "%s", "example.reader");
    (void)snprintf(record.provider_id, sizeof record.provider_id, "%s", "example.vault");
    (void)snprintf(record.secret_reference, sizeof record.secret_reference, "%s", "vault://example/reference");
    record.expires_at_ns = 900U;
    record.active = 1;
    return record;
}

int main(int argc, char **argv)
{
    static const char *const names[] = {
        "unterminated", "empty", "maximum", "lookup-bounds", "revoke-bounds",
        "missing", "duplicate", "capacity", "revocation", "null"
    };
    int result = 0;
    int known = 0;
    UmiCredentialRegistry *registry = NULL;
    UmiCredentialRecord original = example_record(), output, before;
    char unclosed[UMI_CREDENTIAL_ID_CAPACITY];
    if (argc != 2) return 2;
    for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
        if (strcmp(argv[1], names[i]) == 0) known = 1;
    if (!known) return 2;
    memset(&output, 0x5a, sizeof output);
    memcpy(&before, &output, sizeof before);
    memset(unclosed, 'x', sizeof unclosed);
    CHECK(umi_credential_registry_create(&registry) == UMI_STATUS_OK);

    if (strcmp(argv[1], "unterminated") == 0 || strcmp(argv[1], "empty") == 0) {
        for (size_t field = 0U; field < 4U; ++field) {
            UmiCredentialRecord candidate = original;
            char *fields[] = {candidate.credential_id, candidate.principal_id,
                candidate.provider_id, candidate.secret_reference};
            const size_t sizes[] = {sizeof candidate.credential_id, sizeof candidate.principal_id,
                sizeof candidate.provider_id, sizeof candidate.secret_reference};
            if (strcmp(argv[1], "empty") == 0) fields[field][0] = '\0';
            else memset(fields[field], 'x', sizes[field]);
            CHECK(umi_credential_registry_register(registry, &candidate) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_credential_registry_count(registry) == 0U);
        }
        CHECK(umi_credential_registry_register(registry, &original) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "maximum") == 0) {
        char *fields[] = {original.credential_id, original.principal_id,
            original.provider_id, original.secret_reference};
        const size_t sizes[] = {sizeof original.credential_id, sizeof original.principal_id,
            sizeof original.provider_id, sizeof original.secret_reference};
        for (size_t field = 0U; field < 4U; ++field) {
            memset(fields[field], (int)('a' + (int)field), sizes[field] - 1U);
            fields[field][sizes[field] - 1U] = '\0';
        }
        CHECK(umi_credential_registry_register(registry, &original) == UMI_STATUS_OK);
        CHECK(umi_credential_registry_get(registry, original.credential_id, &output) == UMI_STATUS_OK);
        CHECK(strcmp(output.credential_id, original.credential_id) == 0);
        CHECK(strcmp(output.principal_id, original.principal_id) == 0);
        CHECK(strcmp(output.provider_id, original.provider_id) == 0);
        CHECK(strcmp(output.secret_reference, original.secret_reference) == 0);
        CHECK(umi_credential_registry_revoke(registry, original.credential_id) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "null") == 0) {
        CHECK(umi_credential_registry_register(NULL, &original) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_credential_registry_register(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_credential_registry_get(registry, NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_credential_registry_get(NULL, "example", &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_credential_registry_get(registry, "example", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_credential_registry_revoke(NULL, "example") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_credential_registry_revoke(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&output, &before, sizeof output) == 0);
        CHECK(umi_credential_registry_count(registry) == 0U);
    } else {
        CHECK(umi_credential_registry_register(registry, &original) == UMI_STATUS_OK);
        if (strcmp(argv[1], "lookup-bounds") == 0) {
            CHECK(umi_credential_registry_get(registry, unclosed, &output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_credential_registry_get(registry, "", &output) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&output, &before, sizeof output) == 0);
        } else if (strcmp(argv[1], "revoke-bounds") == 0) {
            CHECK(umi_credential_registry_revoke(registry, unclosed) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_credential_registry_revoke(registry, "") == UMI_STATUS_INVALID_ARGUMENT);
        } else if (strcmp(argv[1], "missing") == 0) {
            CHECK(umi_credential_registry_get(registry, "missing", &output) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&output, &before, sizeof output) == 0);
            CHECK(umi_credential_registry_revoke(registry, "missing") == UMI_STATUS_NOT_FOUND);
        } else if (strcmp(argv[1], "duplicate") == 0) {
            UmiCredentialRecord duplicate = original;
            duplicate.active = 0;
            duplicate.expires_at_ns = 1U;
            CHECK(umi_credential_registry_register(registry, &duplicate) == UMI_STATUS_ALREADY_EXISTS);
        } else if (strcmp(argv[1], "capacity") == 0) {
            for (size_t i = 1U; i < UMI_CREDENTIAL_REGISTRY_MAX; ++i) {
                UmiCredentialRecord candidate = original;
                int written = snprintf(candidate.credential_id, sizeof candidate.credential_id, "reference.%zu", i);
                CHECK(written > 0 && (size_t)written < sizeof candidate.credential_id);
                CHECK(umi_credential_registry_register(registry, &candidate) == UMI_STATUS_OK);
            }
            UmiCredentialRecord candidate = original;
            (void)snprintf(candidate.credential_id, sizeof candidate.credential_id, "%s", "one.more");
            CHECK(umi_credential_registry_register(registry, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(umi_credential_registry_register(registry, &original) == UMI_STATUS_ALREADY_EXISTS);
            CHECK(umi_credential_registry_count(registry) == UMI_CREDENTIAL_REGISTRY_MAX);
        } else if (strcmp(argv[1], "revocation") == 0) {
            CHECK(umi_credential_registry_revoke(registry, original.credential_id) == UMI_STATUS_OK);
            CHECK(umi_credential_registry_revoke(registry, original.credential_id) == UMI_STATUS_OK);
        }
        CHECK(umi_credential_registry_get(registry, original.credential_id, &output) == UMI_STATUS_OK);
        CHECK(output.active == (strcmp(argv[1], "revocation") == 0 ? 0 : 1));
        CHECK(output.expires_at_ns == original.expires_at_ns);
        CHECK(strcmp(output.secret_reference, original.secret_reference) == 0);
        if (strcmp(argv[1], "capacity") != 0)
            CHECK(umi_credential_registry_count(registry) == 1U);
    }
finish:
    umi_credential_registry_destroy(registry);
    return result;
}
