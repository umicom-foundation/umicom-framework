/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/profile_secrets.c
 * PURPOSE: Centralise profile verification before touching provider credentials.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/profile_secrets.h"
#include "umicom/base/version.h"
#include <stdlib.h>
#include <string.h>

struct UmiProfileSecrets {
    UmiLocalProfileStore *profiles;
    UmiSecretProvider provider;
    char profile[UMI_LOCAL_PROFILE_NAME_CAPACITY];
};

static UmiStatus Alias(const char *text, size_t capacity)
{
    if (text == NULL || text[0] < 'a' || text[0] > 'z') return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < capacity; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U) return UMI_STATUS_OK;
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-'))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus CanonicalProfile(const char *name, char out[UMI_LOCAL_PROFILE_NAME_CAPACITY])
{
    UmiStatus status = UmiLocalProfileName(name, out);
    if (status == UMI_STATUS_OK && strcmp(name, out) != 0) status = UMI_STATUS_INVALID_ARGUMENT;
    return status;
}
UmiStatus UmiProfileSecretsScopeValidate(const char *application_id, const char *profile_name)
{
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    UmiStatus status = Alias(application_id, 96U);
    return status == UMI_STATUS_OK ? CanonicalProfile(profile_name, canonical) : status;
}
UmiStatus UmiProfileSecretsCreate(UmiLocalProfileStore *profiles, const char *profile_name,
    UmiSecretProvider *provider, UmiProfileSecrets **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    if (profiles == NULL || provider == NULL || provider->structure_size < sizeof(*provider) ||
        provider->abi_version != UMICOM_FRAMEWORK_ABI_VERSION || provider->get == NULL ||
        provider->set == NULL || provider->remove == NULL ||
        CanonicalProfile(profile_name, canonical) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProfileSecrets *secrets = calloc(1U, sizeof(*secrets));
    if (secrets == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiLocalProfileStoreRetain(profiles);
    secrets->profiles = profiles;
    secrets->provider = *provider;
    memcpy(secrets->profile, canonical, sizeof(canonical));
    /* Ownership changes only after every failing construction step has passed. */
    memset(provider, 0, sizeof(*provider));
    *out = secrets;
    return UMI_STATUS_OK;
}
UmiStatus UmiProfileSecretsPlatform(const char *application_id, const char *profile_name,
    UmiProfileSecrets **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = UmiProfileSecretsScopeValidate(application_id, profile_name);
    if (status != UMI_STATUS_OK) return status;
    UmiLocalProfileStore *profiles = NULL;
    UmiSecretProvider provider = {0};
    status = UmiLocalProfileStorePlatform(application_id, &profiles);
    if (status == UMI_STATUS_OK) status = umi_secret_provider_platform(application_id, profile_name, &provider);
    if (status == UMI_STATUS_OK) status = UmiProfileSecretsCreate(profiles, profile_name, &provider, out);
    /* Create retained the profile and transferred the provider on success.
     * These releases also cover a partially constructed native pair. */
    umi_secret_provider_dispose(&provider);
    UmiLocalProfileStoreRelease(profiles);
    return status;
}
void UmiProfileSecretsDestroy(UmiProfileSecrets *secrets)
{
    if (secrets == NULL) return;
    umi_secret_provider_dispose(&secrets->provider);
    UmiLocalProfileStoreRelease(secrets->profiles);
    umi_secret_clear(secrets, sizeof(*secrets));
    free(secrets);
}
UmiStatus UmiProfileSecretsRegister(UmiProfileSecrets *secrets, const char *password)
{
    return secrets == NULL ? UMI_STATUS_INVALID_ARGUMENT :
        UmiLocalProfileRegister(secrets->profiles, secrets->profile, password);
}
static UmiStatus Authorise(UmiProfileSecrets *secrets, const char *password, const char *name)
{
    if (secrets == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = Alias(name, UMI_PLATFORM_SECRET_NAME_CAPACITY);
    /* Do not inspect key existence before authentication: an incorrect local
     * password cannot distinguish a missing alias from a stored credential. */
    if (status == UMI_STATUS_OK) status = UmiLocalProfileVerify(secrets->profiles, secrets->profile, password);
    return status;
}
UmiStatus UmiProfileSecretsGet(UmiProfileSecrets *secrets, const char *password,
    const char *name, char *out_value, size_t capacity)
{
    if (out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_secret_clear(out_value, capacity);
    UmiStatus status = Authorise(secrets, password, name);
    if (status == UMI_STATUS_OK) status = umi_secret_get(&secrets->provider, name, out_value, capacity);
    if (status == UMI_STATUS_OK) {
        const char *end = memchr(out_value, '\0', capacity);
        if (end == NULL || end == out_value || (size_t)(end - out_value) >= UMI_PLATFORM_SECRET_VALUE_CAPACITY)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status != UMI_STATUS_OK) umi_secret_clear(out_value, capacity);
    return status;
}
UmiStatus UmiProfileSecretsSet(UmiProfileSecrets *secrets, const char *password,
    const char *name, const char *value)
{
    if (value == NULL || value[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PLATFORM_SECRET_VALUE_CAPACITY && value[length] != '\0') ++length;
    if (length == UMI_PLATFORM_SECRET_VALUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = Authorise(secrets, password, name);
    if (status == UMI_STATUS_OK) status = umi_secret_set(&secrets->provider, name, value);
    return status;
}
UmiStatus UmiProfileSecretsRemove(UmiProfileSecrets *secrets, const char *password, const char *name)
{
    UmiStatus status = Authorise(secrets, password, name);
    return status == UMI_STATUS_OK ? umi_secret_remove(&secrets->provider, name) : status;
}
