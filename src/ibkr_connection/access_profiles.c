/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/access_profiles.c
 *
 * PURPOSE:
 *   Validate and encode a small credential-free broker settings format.
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

UmiStatus UmiBrokerAccessProfilesDefault(UmiBrokerAccessProfiles *outProfiles)
{
    if (outProfiles == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBrokerAccessProfiles value = {0};
    (void)UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_PAPER, &value.paper);
    (void)UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_LIVE, &value.live);
    *outProfiles = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiBrokerAccessProfilesValidate(const UmiBrokerAccessProfiles *profiles)
{
    if (profiles == NULL || profiles->paper.mode != UMI_BROKER_ACCESS_PAPER ||
        profiles->live.mode != UMI_BROKER_ACCESS_LIVE ||
        UmiBrokerAccessPlanValidate(&profiles->paper) != UMI_STATUS_OK ||
        UmiBrokerAccessPlanValidate(&profiles->live) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiBrokerAccessProfilesEncode(const UmiBrokerAccessProfiles *profiles,
    char *outText, size_t capacity, size_t *outSize)
{
    if (outText == NULL || outSize == NULL ||
        UmiBrokerAccessProfilesValidate(profiles) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Format into private storage first: even a short destination must remain
     * untouched. Fixed integer fields cannot carry passwords or extra keys. */
    char text[UMI_BROKER_PROFILES_TEXT_CAPACITY];
    int count = snprintf(text, sizeof(text),
        "UMICOM-BROKER-PROFILES 1\npaper %u %u %d\nlive %u %u %d\n",
        (unsigned)profiles->paper.program, (unsigned)profiles->paper.port,
        profiles->paper.clientId, (unsigned)profiles->live.program,
        (unsigned)profiles->live.port, profiles->live.clientId);
    if (count < 0 || (size_t)count >= sizeof(text)) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)count >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outText, text, (size_t)count + 1U);
    *outSize = (size_t)count;
    return UMI_STATUS_OK;
}

typedef struct ProfileCursor {
    const unsigned char *bytes;
    size_t size, offset;
} ProfileCursor;

static bool Literal(ProfileCursor *cursor, const char *text)
{
    size_t length = strlen(text);
    if (length > cursor->size - cursor->offset ||
        memcmp(cursor->bytes + cursor->offset, text, length) != 0) return false;
    cursor->offset += length;
    return true;
}

static bool Number(ProfileCursor *cursor, unsigned maximum, unsigned *out)
{
    size_t start = cursor->offset;
    unsigned value = 0U;
    while (cursor->offset < cursor->size) {
        unsigned char digit = cursor->bytes[cursor->offset];
        if (digit < '0' || digit > '9') break;
        unsigned add = (unsigned)(digit - '0');
        if (value > maximum / 10U ||
            (value == maximum / 10U && add > maximum % 10U)) return false;
        value = value * 10U + add;
        ++cursor->offset;
    }
    if (cursor->offset == start ||
        (cursor->offset - start > 1U && cursor->bytes[start] == '0')) return false;
    *out = value;
    return true;
}

static bool Entry(ProfileCursor *cursor, const char *name,
    UmiBrokerAccessMode mode, UmiBrokerAccessPlan *out)
{
    unsigned program, port, client;
    if (!Literal(cursor, name) || !Number(cursor, 1U, &program) ||
        !Literal(cursor, " ") || !Number(cursor, UINT16_MAX, &port) ||
        !Literal(cursor, " ") || !Number(cursor, INT_MAX, &client) ||
        !Literal(cursor, "\n")) return false;
    UmiBrokerAccessPlan value = {mode, (UmiIbkrProgram)program, (uint16_t)port, (int)client};
    if (UmiBrokerAccessPlanValidate(&value) != UMI_STATUS_OK) return false;
    *out = value;
    return true;
}

UmiStatus UmiBrokerAccessProfilesDecode(const void *bytes, size_t size,
    UmiBrokerAccessProfiles *outProfiles)
{
    if (bytes == NULL || outProfiles == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (size >= UMI_BROKER_PROFILES_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    ProfileCursor cursor = {bytes, size, 0U};
    UmiBrokerAccessProfiles value = {0};
    if (!Literal(&cursor, "UMICOM-BROKER-PROFILES 1\n") ||
        !Entry(&cursor, "paper ", UMI_BROKER_ACCESS_PAPER, &value.paper) ||
        !Entry(&cursor, "live ", UMI_BROKER_ACCESS_LIVE, &value.live) ||
        cursor.offset != size) return UMI_STATUS_PARSE_ERROR;
    *outProfiles = value;
    return UMI_STATUS_OK;
}
