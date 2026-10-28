/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_access_profiles.c
 *
 * PURPOSE:
 *   Exercise bounded settings parsing, exact round trips and failure preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/access_profiles.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static int Same(const UmiBrokerAccessProfiles *a, const UmiBrokerAccessProfiles *b)
{
    return a->paper.mode == b->paper.mode && a->live.mode == b->live.mode &&
        a->paper.program == b->paper.program && a->live.program == b->live.program &&
        a->paper.port == b->paper.port && a->live.port == b->live.port &&
        a->paper.clientId == b->paper.clientId && a->live.clientId == b->live.clientId;
}
static int Refused(const void *bytes, size_t size)
{
    UmiBrokerAccessProfiles output, before;
    CHECK(UmiBrokerAccessProfilesDefault(&output) == UMI_STATUS_OK);
    output.paper.clientId = 918;
    before = output;
    CHECK(UmiBrokerAccessProfilesDecode(bytes, size, &output) != UMI_STATUS_OK);
    CHECK(Same(&before, &output));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiBrokerAccessProfiles profiles, decoded;
    CHECK(UmiBrokerAccessProfilesDefault(&profiles) == UMI_STATUS_OK);
    char text[UMI_BROKER_PROFILES_TEXT_CAPACITY];
    size_t size = 0U;
    CHECK(UmiBrokerAccessProfilesEncode(&profiles, text, sizeof(text), &size) == UMI_STATUS_OK);
    const char *name = argv[1];
    if (strcmp(name, "defaults") == 0) {
        CHECK(strcmp(text, "UMICOM-BROKER-PROFILES 1\npaper 0 7497 35\nlive 0 7496 35\n") == 0);
        CHECK(size == strlen(text));
    } else if (strcmp(name, "roundtrip") == 0) {
        profiles.paper.program = UMI_IBKR_GATEWAY;
        profiles.paper.port = UINT16_MAX; profiles.paper.clientId = INT_MAX;
        profiles.live.port = 1U; profiles.live.clientId = 1;
        CHECK(UmiBrokerAccessProfilesEncode(&profiles, text, sizeof(text), &size) == UMI_STATUS_OK);
        CHECK(UmiBrokerAccessProfilesDecode(text, size, &decoded) == UMI_STATUS_OK);
        CHECK(Same(&profiles, &decoded));
    } else if (strcmp(name, "capacity") == 0) {
        char untouched[UMI_BROKER_PROFILES_TEXT_CAPACITY];
        memset(untouched, 'x', sizeof(untouched));
        size_t oldSize = 17U;
        CHECK(UmiBrokerAccessProfilesEncode(&profiles, untouched, size, &oldSize) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i < sizeof(untouched); ++i) CHECK(untouched[i] == 'x');
        CHECK(oldSize == 17U);
    } else if (strcmp(name, "truncation") == 0) {
        for (size_t i = 0U; i < size; ++i) CHECK(Refused(text, i) == 0);
    } else if (strcmp(name, "binary") == 0) {
        for (size_t i = 0U; i < size; ++i) {
            char old = text[i]; text[i] = '\0';
            CHECK(Refused(text, size) == 0);
            text[i] = old;
        }
        CHECK(Refused(text, size + 1U) == 0);
    } else if (strcmp(name, "malformed") == 0) {
        const char *bad[] = {
            "UMICOM-BROKER-PROFILES 2\npaper 0 7497 35\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 2 7497 35\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 0 35\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 65536 35\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 1 99999999999999999999\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 1 -1\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 1 0\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 01 35\nlive 0 7496 35\n",
            "UMICOM-BROKER-PROFILES 1\nlive 0 7496 35\npaper 0 7497 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 7497 35\npaper 0 7497 35\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 7497 35\nlive 0 7496 35\nack 1\n",
            "UMICOM-BROKER-PROFILES 1\npaper 0 7497 35\nlive 0 7496 35\npassword example\n",
            "UMICOM-BROKER-PROFILES 1\r\npaper 0 7497 35\nlive 0 7496 35\n"
        };
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
            CHECK(Refused(bad[i], strlen(bad[i])) == 0);
    } else if (strcmp(name, "oversize") == 0) {
        char big[UMI_BROKER_PROFILES_TEXT_CAPACITY + 1U];
        memset(big, 'x', sizeof(big));
        CHECK(Refused(big, sizeof(big)) == 0);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(UmiBrokerAccessProfilesDefault(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessProfilesValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessProfilesDecode(NULL, 0U, &profiles) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessProfilesDecode(text, size, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        profiles.live.mode = UMI_BROKER_ACCESS_GUEST;
        CHECK(UmiBrokerAccessProfilesValidate(&profiles) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBrokerAccessProfilesEncode(&profiles, text, sizeof(text), &size) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
