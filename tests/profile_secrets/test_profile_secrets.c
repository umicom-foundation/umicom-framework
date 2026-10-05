/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/profile_secrets/test_profile_secrets.c
 * PURPOSE: Verify authentication ordering, ownership, scope and secret-buffer failure semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

static void Denied(UmiProfileSecrets *s, KeyFixture *f, const char *password)
{
    char out[80]; memset(out, 'z', sizeof(out));
    CHECK(UmiProfileSecretsGet(s, password, "personal-key", out, sizeof(out)) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(AllZero(out, sizeof(out)));
    CHECK(UmiProfileSecretsSet(s, password, "personal-key", KEY_VALUE) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiProfileSecretsRemove(s, password, "personal-key") == UMI_STATUS_PERMISSION_DENIED);
    CHECK(f->reads == 0U && f->writes == 0U && f->removes == 0U);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    KeyFixture f = {0}; UmiProfileSecrets *s = KeyService(&f, "desktop");
    char out[80];
    if (strcmp(argv[1], "unknown-profile") == 0) {
        Denied(s, &f, KEY_PASSWORD);
    } else if (strcmp(argv[1], "scope") == 0) {
        CHECK(UmiProfileSecretsScopeValidate("studio", "desktop") == UMI_STATUS_OK);
        CHECK(UmiProfileSecretsScopeValidate("org.umicom.studio", "owner_01") == UMI_STATUS_OK);
        const char *apps[] = {NULL, "", "Studio", "../studio", "1studio", "studio/other"};
        for (size_t i = 0U; i < sizeof(apps)/sizeof(apps[0]); ++i)
            CHECK(UmiProfileSecretsScopeValidate(apps[i], "desktop") != UMI_STATUS_OK);
        const char *names[] = {NULL, "", "ab", "Desktop", "owner.name", "../owner"};
        for (size_t i = 0U; i < sizeof(names)/sizeof(names[0]); ++i)
            CHECK(UmiProfileSecretsScopeValidate("studio", names[i]) != UMI_STATUS_OK);
        char app[97]; memset(app, 'a', 95U); app[95] = '\0';
        CHECK(UmiProfileSecretsScopeValidate(app, "desktop") == UMI_STATUS_OK);
        app[95] = 'a'; app[96] = '\0';
        CHECK(UmiProfileSecretsScopeValidate(app, "desktop") == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(argv[1], "invalid-construction") == 0) {
        UmiLocalProfileStore *profiles = FixtureStore(&f.profile);
        UmiSecretProvider provider = KeyProvider(&f), old = provider;
        UmiProfileSecrets *other = NULL;
        CHECK(UmiProfileSecretsCreate(profiles, "Desktop", &provider, &other) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(other == NULL && provider.instance == old.instance && provider.get == old.get);
        provider.set = NULL;
        CHECK(UmiProfileSecretsCreate(profiles, "desktop", &provider, &other) == UMI_STATUS_INVALID_ARGUMENT);
        provider = old; provider.abi_version = 0U;
        CHECK(UmiProfileSecretsCreate(profiles, "desktop", &provider, &other) == UMI_STATUS_INVALID_ARGUMENT);
        provider = old; provider.structure_size = 0U;
        CHECK(UmiProfileSecretsCreate(profiles, "desktop", &provider, &other) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f.disposals == 0U);
        provider = old; umi_secret_provider_dispose(&provider); CHECK(f.disposals == 1U);
        UmiLocalProfileStoreRelease(profiles);
        CHECK(UmiProfileSecretsPlatform("../studio", "desktop", &other) == UMI_STATUS_INVALID_ARGUMENT && other == NULL);
    } else {
        CHECK(UmiProfileSecretsRegister(s, KEY_PASSWORD) == UMI_STATUS_OK);
        CHECK(f.profile.writes == 1 && f.reads == 0U && f.writes == 0U);
        if (strcmp(argv[1], "lifecycle") == 0) {
            CHECK(UmiProfileSecretsRegister(s, "another-password") == UMI_STATUS_ALREADY_EXISTS);
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", KEY_VALUE) == UMI_STATUS_OK);
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_OK);
            CHECK(strcmp(out, KEY_VALUE) == 0);
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", "replacement-test-key") == UMI_STATUS_OK);
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_OK);
            CHECK(strcmp(out, "replacement-test-key") == 0);
            CHECK(UmiProfileSecretsRemove(s, KEY_PASSWORD, "personal-key") == UMI_STATUS_OK);
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_NOT_FOUND);
            CHECK(AllZero(out, sizeof(out)) && f.profile.present);
        } else if (strcmp(argv[1], "denied") == 0) {
            Denied(s, &f, "wrong-local-password");
        } else if (strcmp(argv[1], "reverify") == 0) {
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", KEY_VALUE) == UMI_STATUS_OK);
            int before = f.profile.derivations;
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_OK);
            CHECK(f.profile.derivations == before + 1);
            /* Deleting the profile invalidates subsequent operations even in
             * the same service object; no remembered authentication survives. */
            f.profile.present = 0; f.reads = f.writes = 0U;
            Denied(s, &f, KEY_PASSWORD); CHECK(f.present);
        } else if (strcmp(argv[1], "corrupt-profile") == 0) {
            f.profile.record.iterations = 1U;
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_PARSE_ERROR);
            CHECK(f.reads == 0U && AllZero(out, sizeof(out)));
        } else if (strcmp(argv[1], "backend-failure") == 0) {
            f.profile.derive_error = UMI_STATUS_UNAVAILABLE;
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", KEY_VALUE) == UMI_STATUS_UNAVAILABLE);
            CHECK(f.writes == 0U);
            f.profile.derive_error = UMI_STATUS_OK; f.provider_error = UMI_STATUS_IO_ERROR;
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", KEY_VALUE) == UMI_STATUS_IO_ERROR);
            CHECK(!f.present);
            CHECK(UmiProfileSecretsRemove(s, KEY_PASSWORD, "personal-key") == UMI_STATUS_IO_ERROR);
        } else if (strcmp(argv[1], "read-clearing") == 0) {
            memset(out, 'z', sizeof(out)); f.provider_error = UMI_STATUS_IO_ERROR;
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_IO_ERROR);
            CHECK(AllZero(out, sizeof(out)));
            memset(out, 'z', sizeof(out));
            CHECK(UmiProfileSecretsGet(NULL, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(AllZero(out, sizeof(out)));
            memset(out, 'z', sizeof(out));
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "../alias", out, sizeof(out)) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(AllZero(out, sizeof(out)));
        } else if (strcmp(argv[1], "malformed-result") == 0) {
            for (unsigned mode = 1U; mode <= 2U; ++mode) {
                f.malformed = mode;
                CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, "personal-key", out, sizeof(out)) == UMI_STATUS_PARSE_ERROR);
                CHECK(AllZero(out, sizeof(out)));
            }
        } else if (strcmp(argv[1], "limits") == 0) {
            char key[UMI_PLATFORM_SECRET_VALUE_CAPACITY + 1U];
            memset(key, 'k', sizeof(key)); key[2048] = '\0';
            char alias[UMI_PLATFORM_SECRET_NAME_CAPACITY + 1U]; memset(alias, 'a', sizeof(alias)); alias[96] = '\0';
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, alias, key) == UMI_STATUS_OK);
            CHECK(UmiProfileSecretsGet(s, KEY_PASSWORD, alias, out, sizeof(out)) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(AllZero(out, sizeof(out)));
            unsigned writes = f.writes; key[2048] = 'k'; key[2049] = '\0';
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, alias, key) == UMI_STATUS_CAPACITY_EXCEEDED);
            alias[96] = 'a'; alias[97] = '\0';
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, alias, KEY_VALUE) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", "") == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(f.writes == writes);
        } else if (strcmp(argv[1], "ownership") == 0) {
            CHECK(f.disposals == 0U);
            UmiProfileSecretsDestroy(s); s = NULL; CHECK(f.disposals == 1U);
            CHECK(f.profile.present); /* Disposal never removes a profile or key. */
        } else if (strcmp(argv[1], "isolation") == 0) {
            KeyFixture other = {0}; UmiProfileSecrets *second = KeyService(&other, "another");
            CHECK(UmiProfileSecretsRegister(second, "another-test-password") == UMI_STATUS_OK);
            CHECK(UmiProfileSecretsSet(s, KEY_PASSWORD, "personal-key", KEY_VALUE) == UMI_STATUS_OK);
            CHECK(UmiProfileSecretsGet(second, "another-test-password", "personal-key", out, sizeof(out)) == UMI_STATUS_NOT_FOUND);
            CHECK(AllZero(out, sizeof(out)) && f.reads == 0U && other.reads == 1U);
            UmiProfileSecretsDestroy(second);
        } else { UmiProfileSecretsDestroy(s); return 2; }
    }
    UmiProfileSecretsDestroy(s);
    return 0;
}
