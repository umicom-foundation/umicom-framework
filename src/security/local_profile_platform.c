/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_profile_platform.c
 * PURPOSE: Use the Windows credential vault and system cryptography for local profiles.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/local_profile.h"
#include "local_profile_platform_internal.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wincred.h>
#include <bcrypt.h>
#include <sddl.h>
#include <wchar.h>

typedef struct ProfilePlatform { wchar_t prefix[160]; HANDLE mutex; } ProfilePlatform;
/* Fixed wire bytes avoid saving compiler padding or a native structure ABI. */
#define PROFILE_BLOB_SIZE 76U
static void ProfileEncode(const UmiLocalProfileRecord *record, unsigned char bytes[PROFILE_BLOB_SIZE])
{
    memcpy(bytes, "UMIP", 4U);
    for (unsigned i = 0U; i < 4U; ++i) {
        bytes[4U+i] = (unsigned char)(record->version >> (i*8U));
        bytes[8U+i] = (unsigned char)(record->iterations >> (i*8U));
    }
    memcpy(bytes+12U, record->salt, 32U); memcpy(bytes+44U, record->verifier, 32U);
}
static UmiStatus ProfileDecode(const unsigned char *bytes, size_t size, UmiLocalProfileRecord *out)
{
    if (bytes == NULL || size != PROFILE_BLOB_SIZE || memcmp(bytes, "UMIP", 4U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    UmiLocalProfileRecord record = {0};
    for (unsigned i = 0U; i < 4U; ++i) {
        record.version |= (uint32_t)bytes[4U+i] << (i*8U);
        record.iterations |= (uint32_t)bytes[8U+i] << (i*8U);
    }
    if (record.version != 1U || record.iterations != UMI_LOCAL_PROFILE_ITERATIONS) return UMI_STATUS_PARSE_ERROR;
    memcpy(record.salt, bytes+12U, 32U); memcpy(record.verifier, bytes+44U, 32U);
    *out = record; umi_secret_clear(&record, sizeof(record)); return UMI_STATUS_OK;
}
static void ProfileTarget(ProfilePlatform *platform, const char *name, wchar_t target[224])
{
    size_t n = wcslen(platform->prefix); memcpy(target, platform->prefix, n*sizeof(wchar_t));
    for (size_t i = 0U; name[i] != '\0'; ++i) target[n++] = (wchar_t)(unsigned char)name[i];
    target[n] = L'\0';
}
static UmiStatus ProfileRead(void *context, const char *name, UmiLocalProfileRecord *out)
{
    wchar_t target[224]; PCREDENTIALW credential = NULL;
    ProfileTarget(context, name, target);
    if (!CredReadW(target, CRED_TYPE_GENERIC, 0U, &credential))
        return GetLastError() == ERROR_NOT_FOUND ? UMI_STATUS_NOT_FOUND : UMI_STATUS_UNAVAILABLE;
    UmiStatus status = ProfileDecode(credential->CredentialBlob, credential->CredentialBlobSize, out);
    if (credential->CredentialBlob != NULL) SecureZeroMemory(credential->CredentialBlob, credential->CredentialBlobSize);
    CredFree(credential); return status;
}
static UmiStatus ProfileEnter(ProfilePlatform *platform)
{
    DWORD wait = WaitForSingleObject(platform->mutex, 5000U);
    return wait == WAIT_OBJECT_0 || wait == WAIT_ABANDONED ? UMI_STATUS_OK : UMI_STATUS_BUSY;
}
static UmiStatus ProfileCreate(void *context, const char *name, const UmiLocalProfileRecord *record)
{
    ProfilePlatform *platform = context;
    UmiStatus status = ProfileEnter(platform);
    if (status != UMI_STATUS_OK) return status;
    UmiLocalProfileRecord existing = {0};
    status = ProfileRead(context, name, &existing); umi_secret_clear(&existing, sizeof(existing));
    if (status == UMI_STATUS_NOT_FOUND) {
        wchar_t target[224], user[UMI_LOCAL_PROFILE_NAME_CAPACITY] = {0};
        unsigned char bytes[PROFILE_BLOB_SIZE]; CREDENTIALW credential = {0};
        ProfileTarget(platform, name, target); ProfileEncode(record, bytes);
        for (size_t i = 0U; name[i] != '\0'; ++i) user[i] = (wchar_t)(unsigned char)name[i];
        credential.Type = CRED_TYPE_GENERIC; credential.TargetName = target; credential.UserName = user;
        credential.CredentialBlob = bytes; credential.CredentialBlobSize = PROFILE_BLOB_SIZE;
        credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
        status = CredWriteW(&credential, 0U) ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
        umi_secret_clear(bytes, sizeof(bytes));
    } else if (status == UMI_STATUS_OK) status = UMI_STATUS_ALREADY_EXISTS;
    (void)ReleaseMutex(platform->mutex); return status;
}
static UmiStatus ProfileRemove(void *context, const char *name, const UmiLocalProfileRecord *expected)
{
    ProfilePlatform *platform = context; UmiStatus status = ProfileEnter(platform);
    if (status != UMI_STATUS_OK) return status;
    UmiLocalProfileRecord current = {0}; status = ProfileRead(context, name, &current);
    if (status == UMI_STATUS_OK) {
        unsigned char a[PROFILE_BLOB_SIZE], b[PROFILE_BLOB_SIZE];
        ProfileEncode(&current,a); ProfileEncode(expected,b);
        if (memcmp(a,b,sizeof(a)) != 0) status = UMI_STATUS_INVALID_STATE;
        umi_secret_clear(a,sizeof(a)); umi_secret_clear(b,sizeof(b));
    }
    if (status == UMI_STATUS_OK) {
        wchar_t target[224]; ProfileTarget(platform,name,target);
        status = CredDeleteW(target,CRED_TYPE_GENERIC,0U) ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
    }
    umi_secret_clear(&current,sizeof(current)); (void)ReleaseMutex(platform->mutex); return status;
}
static UmiStatus ProfileRandom(void *context, unsigned char *out, size_t size)
{
    (void)context;
    if (size > UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    return BCryptGenRandom(NULL,out,(ULONG)size,BCRYPT_USE_SYSTEM_PREFERRED_RNG) >= 0 ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
static UmiStatus ProfileDerive(void *context, const char *password, size_t length,
                              const UmiLocalProfileRecord *record, unsigned char out[32])
{
    (void)context; BCRYPT_ALG_HANDLE algorithm = NULL;
    NTSTATUS result = BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,NULL,BCRYPT_ALG_HANDLE_HMAC_FLAG);
    if (result < 0) return UMI_STATUS_UNAVAILABLE;
    result = BCryptDeriveKeyPBKDF2(algorithm,(PUCHAR)(const void *)password,(ULONG)length,
        (PUCHAR)(const void *)record->salt,32U,(ULONGLONG)record->iterations,out,32U,0U);
    (void)BCryptCloseAlgorithmProvider(algorithm,0U);
    return result >= 0 ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
static void ProfileDestroy(void *context)
{
    ProfilePlatform *platform = context;
    if (platform->mutex != NULL) (void)CloseHandle(platform->mutex);
    umi_secret_clear(platform,sizeof(*platform)); free(platform);
}
static UmiStatus ProfileMutex(ProfilePlatform *platform, const wchar_t *application)
{
    HANDLE token = NULL; DWORD size = 0U; LPWSTR sid = NULL; TOKEN_USER *user = NULL;
    UmiStatus status = UMI_STATUS_UNAVAILABLE;
    if (!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token)) return status;
    (void)GetTokenInformation(token,TokenUser,NULL,0U,&size);
    if (size != 0U && size < 65536U) user = malloc(size);
    if (user != NULL && GetTokenInformation(token,TokenUser,user,size,&size) &&
        ConvertSidToStringSidW(user->User.Sid,&sid)) {
        wchar_t mutex_name[320];
        int n = swprintf(mutex_name,320U,L"Global\\UmicomLocalProfile.%ls.%ls",sid,application);
        if (n > 0 && n < 320) {
            platform->mutex = CreateMutexW(NULL,FALSE,mutex_name);
            if (platform->mutex != NULL) status = UMI_STATUS_OK;
        }
    }
    if (sid != NULL) (void)LocalFree(sid);
    free(user); (void)CloseHandle(token); return status;
}
#endif
/* The native backend is shared with local database persistence so password derivation is not duplicated. The previous vault constructor is retained; its public behavior remains available. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiLocalProfileStorePlatform(const char *application_id, UmiLocalProfileStore **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (application_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    for (; length < 96U && application_id[length] != '\0'; ++length) {
        char c = application_id[length];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_')) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U || length == 96U) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    ProfilePlatform *platform = calloc(1U,sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    wchar_t app[96] = {0};
    for (size_t i = 0U; i < length; ++i) app[i] = (wchar_t)(unsigned char)application_id[i];
    (void)swprintf(platform->prefix,160U,L"Umicom/LocalProfile/%ls/",app);
    UmiStatus status = ProfileMutex(platform,app);
    if (status == UMI_STATUS_OK) {
        UmiLocalProfileBackend backend = {platform,ProfileRead,ProfileCreate,ProfileRemove,ProfileRandom,ProfileDerive,ProfileDestroy};
        status = UmiLocalProfileStoreCreate(&backend,out);
    }
    if (status != UMI_STATUS_OK) ProfileDestroy(platform);
    return status;
#else
    return UMI_STATUS_UNAVAILABLE;
#endif
}
#endif
UmiStatus UmiLocalProfilePlatformBackend(const char *application_id, UmiLocalProfileBackend *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (application_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    for (; length < 96U && application_id[length] != '\0'; ++length) {
        char c = application_id[length];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_')) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U || length == 96U) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    ProfilePlatform *platform = calloc(1U,sizeof(*platform));
    if (platform == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    wchar_t app[96] = {0};
    for (size_t i = 0U; i < length; ++i) app[i] = (wchar_t)(unsigned char)application_id[i];
    (void)swprintf(platform->prefix,160U,L"Umicom/LocalProfile/%ls/",app);
    UmiStatus status = ProfileMutex(platform,app);
    if (status == UMI_STATUS_OK) {
        UmiLocalProfileBackend backend = {platform,ProfileRead,ProfileCreate,ProfileRemove,ProfileRandom,ProfileDerive,ProfileDestroy};
        *out = backend;
    }
    if (status != UMI_STATUS_OK) ProfileDestroy(platform);
    return status;
#else
    return UMI_STATUS_UNAVAILABLE;
#endif
}
UmiStatus UmiLocalProfileStorePlatform(const char *application_id, UmiLocalProfileStore **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiLocalProfileBackend backend;
    UmiStatus status = UmiLocalProfilePlatformBackend(application_id, &backend);
    if (status != UMI_STATUS_OK) return status;
    status = UmiLocalProfileStoreCreate(&backend, out);
    if (status != UMI_STATUS_OK) backend.destroy(backend.context);
    return status;
}
UmiStatus UmiLocalProfilePlatformEnter(void *context)
{
#ifdef _WIN32
    return context != NULL ? ProfileEnter(context) : UMI_STATUS_INVALID_ARGUMENT;
#else
    (void)context;
    return UMI_STATUS_UNAVAILABLE;
#endif
}
void UmiLocalProfilePlatformLeave(void *context)
{
#ifdef _WIN32
    if (context != NULL) (void)ReleaseMutex(((ProfilePlatform *)context)->mutex);
#else
    (void)context;
#endif
}

