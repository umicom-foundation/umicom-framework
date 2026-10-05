/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/platform_secrets.c
 * PURPOSE: Implement local credential storage through the shared secret-provider boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/platform_secrets.h"
#include "umicom/base/version.h"
#include <stdlib.h>
#include <string.h>

/* Names are aliases, not paths or URLs. Restricting their alphabet prevents
 * one profile from accidentally selecting another profile's target through
 * a separator. Requiring lower case also avoids Windows case-folding aliases. */
static UmiStatus PlatformSecretName(const char *name, size_t capacity)
{
    if (name == NULL || name[0] < 'a' || name[0] > 'z')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < capacity; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (c == 0U) return UMI_STATUS_OK;
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
              c == '.' || c == '_' || c == '-')) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wincred.h>
#include <wchar.h>

/* The operating system protects the secret bytes. This object only remembers
 * the non-secret namespace, so disposal never needs to retrieve saved keys. */
typedef struct PlatformSecrets { wchar_t prefix[224]; } PlatformSecrets;
#define PLATFORM_SECRET_TARGET_CAPACITY 322U

static UmiStatus PlatformSecretError(DWORD error)
{
    if (error == ERROR_NOT_FOUND) return UMI_STATUS_NOT_FOUND;
    if (error == ERROR_ACCESS_DENIED) return UMI_STATUS_PERMISSION_DENIED;
    if (error == ERROR_NOT_ENOUGH_MEMORY || error == ERROR_OUTOFMEMORY)
        return UMI_STATUS_OUT_OF_MEMORY;
    return UMI_STATUS_UNAVAILABLE;
}

/* Every caller validates the bounded alias before this copy. No locale or
 * UTF-16 conversion is needed for the deliberately ASCII-only target name. */
static UmiStatus PlatformSecretTarget(const PlatformSecrets *store,
    const char *name, wchar_t target[PLATFORM_SECRET_TARGET_CAPACITY])
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PlatformSecretName(name, UMI_PLATFORM_SECRET_NAME_CAPACITY);
    if (status != UMI_STATUS_OK) return status;
    size_t used = wcslen(store->prefix);
    memcpy(target, store->prefix, used * sizeof(wchar_t));
    for (size_t i = 0U; name[i] != '\0'; ++i) target[used++] = (wchar_t)(unsigned char)name[i];
    target[used] = L'\0';
    return UMI_STATUS_OK;
}

static UmiStatus PlatformSecretGet(void *instance, const char *name,
    char *out_value, size_t capacity)
{
    if (out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Erase earlier caller content even if this lookup fails. A caller must
     * never mistake a previous successful key for the requested missing key. */
    umi_secret_clear(out_value, capacity);
    wchar_t target[PLATFORM_SECRET_TARGET_CAPACITY];
    UmiStatus status = PlatformSecretTarget(instance, name, target);
    if (status != UMI_STATUS_OK) return status;
    PCREDENTIALW credential = NULL;
    if (!CredReadW(target, CRED_TYPE_GENERIC, 0U, &credential))
        return PlatformSecretError(GetLastError());
    if (credential == NULL) return UMI_STATUS_PARSE_ERROR;
    size_t length = (size_t)credential->CredentialBlobSize;
    /* Treat unexpected stored bytes as invalid, not as a partial credential.
     * Another same-user program may have changed a record since it was saved. */
    if (credential->Type != CRED_TYPE_GENERIC || credential->CredentialBlob == NULL ||
        length == 0U || length >= UMI_PLATFORM_SECRET_VALUE_CAPACITY ||
        memchr(credential->CredentialBlob, '\0', length) != NULL) {
        status = UMI_STATUS_PARSE_ERROR;
    } else if (length >= capacity) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else {
        memcpy(out_value, credential->CredentialBlob, length);
        out_value[length] = '\0';
    }
    /* CredRead allocates a second copy of the secret. Clear that copy before
     * returning its allocation to Windows, regardless of lookup outcome. */
    if (credential->CredentialBlob != NULL)
        umi_secret_clear(credential->CredentialBlob, length);
    CredFree(credential);
    return status;
}

static UmiStatus PlatformSecretSet(void *instance, const char *name, const char *value)
{
    wchar_t target[PLATFORM_SECRET_TARGET_CAPACITY];
    UmiStatus status = PlatformSecretTarget(instance, name, target);
    if (status != UMI_STATUS_OK) return status;
    if (value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PLATFORM_SECRET_VALUE_CAPACITY && value[length] != '\0') ++length;
    if (length == UMI_PLATFORM_SECRET_VALUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Copy into a temporary mutable buffer because the Windows API accepts
     * mutable bytes. Never cast away the caller's const-qualified ownership. */
    unsigned char bytes[UMI_PLATFORM_SECRET_VALUE_CAPACITY] = {0};
    memcpy(bytes, value, length);
    CREDENTIALW credential = {0};
    credential.Type = CRED_TYPE_GENERIC;
    credential.TargetName = target;
    credential.CredentialBlob = bytes;
    credential.CredentialBlobSize = (DWORD)length;
    credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
    if (!CredWriteW(&credential, 0U)) status = PlatformSecretError(GetLastError());
    umi_secret_clear(bytes, sizeof(bytes));
    return status;
}

static UmiStatus PlatformSecretRemove(void *instance, const char *name)
{
    wchar_t target[PLATFORM_SECRET_TARGET_CAPACITY];
    UmiStatus status = PlatformSecretTarget(instance, name, target);
    if (status != UMI_STATUS_OK) return status;
    if (!CredDeleteW(target, CRED_TYPE_GENERIC, 0U)) return PlatformSecretError(GetLastError());
    return UMI_STATUS_OK;
}

static void PlatformSecretDestroy(void *instance)
{
    if (instance == NULL) return;
    umi_secret_clear(instance, sizeof(PlatformSecrets));
    free(instance);
}
#endif

UmiStatus umi_secret_provider_platform(const char *application_id,
    const char *profile_id, UmiSecretProvider *out_provider)
{
    if (out_provider == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PlatformSecretName(application_id, UMI_PLATFORM_SECRET_SCOPE_CAPACITY);
    if (status != UMI_STATUS_OK) return status;
    status = PlatformSecretName(profile_id, UMI_PLATFORM_SECRET_SCOPE_CAPACITY);
    if (status != UMI_STATUS_OK) return status;
#ifdef _WIN32
    PlatformSecrets *store = calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep this prefix separate from the local-password verifier namespace.
     * Future native keyring adapters should preserve these same three scopes:
     * OS user, application/profile aliases, and the caller's secret alias. */
    const wchar_t prefix[] = L"Umicom/ProviderSecrets/";
    size_t used = (sizeof(prefix) / sizeof(prefix[0])) - 1U;
    memcpy(store->prefix, prefix, used * sizeof(wchar_t));
    for (size_t i = 0U; application_id[i] != '\0'; ++i)
        store->prefix[used++] = (wchar_t)(unsigned char)application_id[i];
    store->prefix[used++] = L'/';
    for (size_t i = 0U; profile_id[i] != '\0'; ++i)
        store->prefix[used++] = (wchar_t)(unsigned char)profile_id[i];
    store->prefix[used++] = L'/';
    store->prefix[used] = L'\0';
    UmiSecretProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    provider.instance = store;
    provider.get = PlatformSecretGet;
    provider.set = PlatformSecretSet;
    provider.remove = PlatformSecretRemove;
    provider.destroy = PlatformSecretDestroy;
    *out_provider = provider;
    return UMI_STATUS_OK;
#else
    /* A missing secure backend must stay visible to the caller. Silently
     * substituting a file or database would change the privacy guarantee. */
    return UMI_STATUS_UNAVAILABLE;
#endif
}
