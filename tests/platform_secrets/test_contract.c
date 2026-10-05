/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform_secrets/test_contract.c
 * PURPOSE: Check namespace validation without writing any operating-system credential.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/platform_secrets.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "line %d: %s\n", __LINE__, #c); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiSecretProvider provider = {0};
    if (strcmp(argv[1], "invalid") == 0) {
        const char *invalid[] = {"", "UPPER", "../owner", "owner/key", "owner\\key", "two words", "a:b", "caf\xc3\xa9", "0owner"};
        for (size_t i = 0U; i < sizeof(invalid)/sizeof(invalid[0]); ++i) {
            CHECK(umi_secret_provider_platform(invalid[i], "owner", &provider) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_secret_provider_platform("org.umicom", invalid[i], &provider) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(provider.instance == NULL && provider.get == NULL);
        }
        CHECK(umi_secret_provider_platform(NULL, "owner", &provider) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_secret_provider_platform("org.umicom", NULL, &provider) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_secret_provider_platform("org.umicom", "owner", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "boundaries") == 0) {
        char name[UMI_PLATFORM_SECRET_SCOPE_CAPACITY+1U]; memset(name, 'a', sizeof(name));
        name[sizeof(name)-1U] = '\0';
        CHECK(umi_secret_provider_platform(name, "owner", &provider) == UMI_STATUS_CAPACITY_EXCEEDED);
        name[UMI_PLATFORM_SECRET_SCOPE_CAPACITY-1U] = '\0';
        UmiStatus status = umi_secret_provider_platform(name, name, &provider);
#ifdef _WIN32
        CHECK(status == UMI_STATUS_OK);
        umi_secret_provider_dispose(&provider);
#else
        CHECK(status == UMI_STATUS_UNAVAILABLE && provider.instance == NULL);
#endif
    } else if (strcmp(argv[1], "factory") == 0) {
        UmiStatus status = umi_secret_provider_platform("org.umicom.media", "local-owner", &provider);
#ifdef _WIN32
        CHECK(status == UMI_STATUS_OK && provider.get != NULL && provider.set != NULL && provider.remove != NULL);
        umi_secret_provider_dispose(&provider);
        CHECK(provider.instance == NULL && provider.get == NULL);
#else
        CHECK(status == UMI_STATUS_UNAVAILABLE && provider.get == NULL);
#endif
    } else return 2;
    return 0;
}
