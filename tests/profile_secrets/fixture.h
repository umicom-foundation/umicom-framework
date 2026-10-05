/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/profile_secrets/fixture.h
 * PURPOSE: Supply isolated profile and credential adapters with observable access counts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROFILE_SECRETS_TEST_FIXTURE_H
#define UMICOM_PROFILE_SECRETS_TEST_FIXTURE_H
#include "../local_profile/fixture.h"
#include "umicom/security/profile_secrets.h"
#include "umicom/base/version.h"
#include <stdbool.h>

/* These are deliberately fake, in-memory records. The existing fixture's
 * cheap derivation is not cryptography and must never be a production adapter. */
#define KEY_PASSWORD "a-local-test-password"
#define KEY_VALUE "example-key-for-tests-only"
typedef struct KeyFixture {
    ProfileFixture profile;
    char value[UMI_PLATFORM_SECRET_VALUE_CAPACITY];
    bool present;
    unsigned reads, writes, removes, disposals;
    UmiStatus provider_error;
    unsigned malformed;
} KeyFixture;
static UmiStatus KeyGet(void *data, const char *name, char *out, size_t capacity)
{
    KeyFixture *f = data; (void)name; ++f->reads;
    if (f->provider_error != UMI_STATUS_OK) {
        memset(out, 'x', capacity); return f->provider_error;
    }
    if (f->malformed == 1U) { memset(out, 'x', capacity); return UMI_STATUS_OK; }
    if (f->malformed == 2U) { out[0] = '\0'; return UMI_STATUS_OK; }
    if (!f->present) return UMI_STATUS_NOT_FOUND;
    if (strlen(f->value) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(out, f->value); return UMI_STATUS_OK;
}
static UmiStatus KeySet(void *data, const char *name, const char *value)
{
    KeyFixture *f = data; (void)name; ++f->writes;
    if (f->provider_error != UMI_STATUS_OK) return f->provider_error;
    strcpy(f->value, value); f->present = true; return UMI_STATUS_OK;
}
static UmiStatus KeyRemove(void *data, const char *name)
{
    KeyFixture *f = data; (void)name; ++f->removes;
    if (f->provider_error != UMI_STATUS_OK) return f->provider_error;
    if (!f->present) return UMI_STATUS_NOT_FOUND;
    umi_secret_clear(f->value, sizeof(f->value)); f->present = false; return UMI_STATUS_OK;
}
static void KeyDispose(void *data) { ++((KeyFixture *)data)->disposals; }
static UmiSecretProvider KeyProvider(KeyFixture *f)
{
    UmiSecretProvider provider = {(uint32_t)sizeof(UmiSecretProvider), UMICOM_FRAMEWORK_ABI_VERSION,
        f, KeyGet, KeyDispose, KeySet, KeyRemove};
    return provider;
}
static UmiProfileSecrets *KeyService(KeyFixture *f, const char *name)
{
    UmiLocalProfileStore *profiles = FixtureStore(&f->profile);
    UmiSecretProvider provider = KeyProvider(f);
    UmiProfileSecrets *secrets = NULL;
    CHECK(UmiProfileSecretsCreate(profiles, name, &provider, &secrets) == UMI_STATUS_OK);
    CHECK(provider.instance == NULL && provider.get == NULL && provider.structure_size == 0U);
    UmiLocalProfileStoreRelease(profiles);
    return secrets;
}
static bool AllZero(const char *bytes, size_t size)
{
    for (size_t i = 0U; i < size; ++i) if (bytes[i] != '\0') return false;
    return true;
}
#endif
