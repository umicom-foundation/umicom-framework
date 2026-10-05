/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_secrets/test_windows_store.c
 * PURPOSE: Exercise native credential semantics with isolated fake Windows credential calls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wincred.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/security/secrets.h"
#include "umicom/security/secret_provider.h"

/* Compile the production adapter with only its OS boundary replaced. These
 * cases cannot enumerate, create or remove credentials in the user's vault. */
typedef struct Stored { wchar_t target[322]; unsigned char bytes[4096]; DWORD length; int used; } Stored;
static Stored stored[8];
static DWORD read_error, write_error, delete_error;
static unsigned reads, writes, removals, frees;
static int protocol_error, corrupt_blob;
static Stored *FindStored(LPCWSTR target)
{
    for (size_t i = 0U; i < 8U; ++i) if (stored[i].used && wcscmp(stored[i].target, target) == 0) return &stored[i];
    return NULL;
}
static BOOL WINAPI FakeRead(LPCWSTR target, DWORD type, DWORD flags, PCREDENTIALW *out)
{
    ++reads;
    if (type != CRED_TYPE_GENERIC || flags != 0U) protocol_error = 1;
    if (read_error != 0U) { SetLastError(read_error); return FALSE; }
    Stored *item = FindStored(target);
    if (item == NULL) { SetLastError(ERROR_NOT_FOUND); return FALSE; }
    size_t length = corrupt_blob == 2 ? 2050U : (size_t)item->length;
    PCREDENTIALW result = calloc(1U, sizeof(*result)+length);
    if (result == NULL) { SetLastError(ERROR_OUTOFMEMORY); return FALSE; }
    result->Type = CRED_TYPE_GENERIC;
    result->CredentialBlobSize = (DWORD)length;
    result->CredentialBlob = (LPBYTE)(result+1);
    memset(result->CredentialBlob, 'x', length);
    if (corrupt_blob != 2) memcpy(result->CredentialBlob, item->bytes, length);
    if (corrupt_blob == 1) result->CredentialBlob[0] = 0U;
    if (corrupt_blob == 3) result->Type = CRED_TYPE_DOMAIN_PASSWORD;
    *out = result; return TRUE;
}
static BOOL WINAPI FakeWrite(PCREDENTIALW value, DWORD flags)
{
    ++writes;
    if (value->Type != CRED_TYPE_GENERIC || value->Persist != CRED_PERSIST_LOCAL_MACHINE ||
        flags != 0U || value->Flags != 0U || value->AttributeCount != 0U ||
        value->CredentialBlobSize > 2048U || value->UserName != NULL) protocol_error = 1;
    if (write_error != 0U) { SetLastError(write_error); return FALSE; }
    Stored *item = FindStored(value->TargetName);
    if (item == NULL) for (size_t i = 0U; i < 8U; ++i) if (!stored[i].used) { item = &stored[i]; break; }
    if (item == NULL) { SetLastError(ERROR_OUTOFMEMORY); return FALSE; }
    wcscpy(item->target, value->TargetName); item->used = 1; item->length = value->CredentialBlobSize;
    memcpy(item->bytes, value->CredentialBlob, item->length); return TRUE;
}
static BOOL WINAPI FakeDelete(LPCWSTR target, DWORD type, DWORD flags)
{
    ++removals;
    if (type != CRED_TYPE_GENERIC || flags != 0U) protocol_error = 1;
    if (delete_error != 0U) { SetLastError(delete_error); return FALSE; }
    Stored *item = FindStored(target);
    if (item == NULL) { SetLastError(ERROR_NOT_FOUND); return FALSE; }
    memset(item, 0, sizeof(*item)); return TRUE;
}
static void WINAPI FakeFree(PVOID memory)
{
    PCREDENTIALW value = memory; ++frees;
    /* Even rejected and oversized records must be cleared before release. */
    for (DWORD i = 0U; i < value->CredentialBlobSize; ++i)
        if (value->CredentialBlob[i] != 0U) protocol_error = 1;
    free(memory);
}
#define CredReadW FakeRead
#define CredWriteW FakeWrite
#define CredDeleteW FakeDelete
#define CredFree FakeFree
#define umi_secret_provider_platform umi_test_secret_provider_platform
#include "../../src/security/platform_secrets.c"
#undef umi_secret_provider_platform
#undef CredReadW
#undef CredWriteW
#undef CredDeleteW
#undef CredFree

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "line %d: %s\n", __LINE__, #c); return 1; } } while (0)
static int Cleared(const char *text, size_t length)
{
    for (size_t i = 0U; i < length; ++i) if (text[i] != '\0') return 0;
    return 1;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiSecretProvider provider = {0}; char output[4096];
    CHECK(umi_test_secret_provider_platform("org.umicom", "owner", &provider) == UMI_STATUS_OK);
    if (strcmp(argv[1], "roundtrip") == 0) {
        CHECK(umi_secret_set(&provider, "model-key", "fixture-only-value") == UMI_STATUS_OK);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(strcmp(output, "fixture-only-value") == 0 && frees == 1U);
        CHECK(wcscmp(stored[0].target, L"Umicom/ProviderSecrets/org.umicom/owner/model-key") == 0);
        CHECK(umi_secret_remove(&provider, "model-key") == UMI_STATUS_OK);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_NOT_FOUND);
        CHECK(Cleared(output, sizeof(output)));
    } else if (strcmp(argv[1], "reopen") == 0) {
        CHECK(umi_secret_set(&provider, "model-key", "fixture") == UMI_STATUS_OK);
        umi_secret_provider_dispose(&provider);
        CHECK(removals == 0U);
        CHECK(umi_test_secret_provider_platform("org.umicom", "owner", &provider) == UMI_STATUS_OK);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(strcmp(output, "fixture") == 0);
    } else if (strcmp(argv[1], "scope") == 0) {
        UmiSecretProvider other = {0};
        CHECK(umi_secret_set(&provider, "model-key", "first") == UMI_STATUS_OK);
        CHECK(umi_test_secret_provider_platform("org.umicom", "guest", &other) == UMI_STATUS_OK);
        CHECK(umi_secret_get(&other, "model-key", output, sizeof(output)) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_secret_set(&other, "model-key", "second") == UMI_STATUS_OK);
        umi_secret_provider_dispose(&other);
        CHECK(umi_test_secret_provider_platform("org.umicom.media", "owner", &other) == UMI_STATUS_OK);
        CHECK(umi_secret_get(&other, "model-key", output, sizeof(output)) == UMI_STATUS_NOT_FOUND);
        umi_secret_provider_dispose(&other);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK && strcmp(output, "first") == 0);
    } else if (strcmp(argv[1], "replace") == 0 || strcmp(argv[1], "write-failure") == 0) {
        CHECK(umi_secret_set(&provider, "model-key", "first") == UMI_STATUS_OK);
        int fail = strcmp(argv[1], "write-failure") == 0;
        if (fail) write_error = ERROR_ACCESS_DENIED;
        CHECK(umi_secret_set(&provider, "model-key", "second") == (fail ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK));
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(strcmp(output, fail ? "first" : "second") == 0);
    } else if (strcmp(argv[1], "small-buffer") == 0) {
        CHECK(umi_secret_set(&provider, "model-key", "four") == UMI_STATUS_OK);
        memset(output, 'x', sizeof(output));
        CHECK(umi_secret_get(&provider, "model-key", output, 4U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(Cleared(output, 4U) && output[4] == 'x' && frees == 1U);
        CHECK(umi_secret_get(&provider, "model-key", output, 5U) == UMI_STATUS_OK && strcmp(output, "four") == 0);
    } else if (strcmp(argv[1], "unicode") == 0) {
        const char value[] = "fixture-caf\xc3\xa9-\xf0\x9f\x94\x90";
        CHECK(umi_secret_set(&provider, "model-key", value) == UMI_STATUS_OK);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(memcmp(output, value, sizeof(value)) == 0);
    } else if (strcmp(argv[1], "limits") == 0) {
        char value[UMI_PLATFORM_SECRET_VALUE_CAPACITY+1U]; memset(value, 'q', sizeof(value));
        value[UMI_PLATFORM_SECRET_VALUE_CAPACITY-1U] = '\0';
        CHECK(umi_secret_set(&provider, "model-key", value) == UMI_STATUS_OK);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(strlen(output) == UMI_PLATFORM_SECRET_VALUE_CAPACITY-1U);
        value[UMI_PLATFORM_SECRET_VALUE_CAPACITY-1U] = 'q'; value[UMI_PLATFORM_SECRET_VALUE_CAPACITY] = '\0';
        CHECK(umi_secret_set(&provider, "model-key", value) == UMI_STATUS_CAPACITY_EXCEEDED && writes == 1U);
        CHECK(umi_secret_set(&provider, "model-key", "") == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "invalid-name") == 0) {
        const char *names[] = {"", "../key", "other/key", "UPPER", "with space", "a:key"};
        for (size_t i = 0U; i < sizeof(names)/sizeof(names[0]); ++i) {
            CHECK(umi_secret_set(&provider, names[i], "fixture") == UMI_STATUS_INVALID_ARGUMENT);
            memset(output, 'x', sizeof(output));
            CHECK(umi_secret_get(&provider, names[i], output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(Cleared(output, sizeof(output)));
        }
        CHECK(reads == 0U && writes == 0U);
    } else if (strcmp(argv[1], "read-failure") == 0) {
        read_error = ERROR_NO_SUCH_LOGON_SESSION; memset(output, 'x', sizeof(output));
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_UNAVAILABLE);
        CHECK(Cleared(output, sizeof(output)) && frees == 0U);
    } else if (strcmp(argv[1], "remove-failure") == 0) {
        CHECK(umi_secret_remove(&provider, "model-key") == UMI_STATUS_NOT_FOUND);
        CHECK(umi_secret_set(&provider, "model-key", "fixture") == UMI_STATUS_OK);
        delete_error = ERROR_ACCESS_DENIED;
        CHECK(umi_secret_remove(&provider, "model-key") == UMI_STATUS_PERMISSION_DENIED);
        CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "corrupt") == 0) {
        CHECK(umi_secret_set(&provider, "model-key", "fixture") == UMI_STATUS_OK);
        for (corrupt_blob = 1; corrupt_blob <= 3; ++corrupt_blob) {
            memset(output, 'x', sizeof(output));
            CHECK(umi_secret_get(&provider, "model-key", output, sizeof(output)) == UMI_STATUS_PARSE_ERROR);
            CHECK(Cleared(output, sizeof(output)));
        }
        CHECK(frees == 3U);
    } else if (strcmp(argv[1], "registry") == 0) {
        UmiSecretProviderRegistry *registry = NULL;
        CHECK(umi_secret_provider_registry_create(&registry) == UMI_STATUS_OK);
        CHECK(umi_secret_provider_registry_add(registry, "vault", &provider) == UMI_STATUS_OK);
        CHECK(provider.instance == NULL);
        CHECK(umi_secret_provider_registry_store(registry, "vault://model-key", "fixture") == UMI_STATUS_OK);
        CHECK(umi_secret_provider_registry_resolve(registry, "vault://model-key", output, sizeof(output)) == UMI_STATUS_OK);
        CHECK(strcmp(output, "fixture") == 0);
        CHECK(umi_secret_provider_registry_remove(registry, "vault://model-key") == UMI_STATUS_OK);
        umi_secret_provider_registry_destroy(registry);
    } else return 2;
    umi_secret_clear(output, sizeof(output));
    umi_secret_provider_dispose(&provider);
    CHECK(protocol_error == 0);
    return 0;
}
