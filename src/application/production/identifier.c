/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/identifier.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/identifier.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Copy application production identifier into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_production_identifier_set(
    UmiApplicationProductionIdentifier *identifier, const char *value)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (identifier == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(identifier->value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(identifier->value, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Check that application production identifier satisfies its contract before another
 * service relies on it.
 */
int umi_application_production_identifier_valid(
    const UmiApplicationProductionIdentifier *identifier)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (identifier == NULL) return 0;
    if (memchr(identifier->value, '\0', sizeof(identifier->value)) == NULL) return 0;

    return identifier != NULL && identifier->value[0] != '\0';
}

/*
 * Provide the application production identifier equal operation used by this module and
 * its client applications.
 */
int umi_application_production_identifier_equal(
    const UmiApplicationProductionIdentifier *left,
    const UmiApplicationProductionIdentifier *right)
{
    return umi_application_production_identifier_valid(left) &&
           umi_application_production_identifier_valid(right) &&
           strcmp(left->value, right->value) == 0;
}


/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiApplicationProductionIdentifierArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6b8537c231beaf3f);
    schema = (schema ^ (uint64_t)sizeof(((UmiApplicationProductionIdentifier *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiApplicationProductionIdentifierArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiApplicationProductionIdentifier *)0)->value) - 1U;
}
static void UmiApplicationProductionIdentifierArchiveWrite(UmiArchiveWriter *writer, const UmiApplicationProductionIdentifier *value)
{
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
}
static void UmiApplicationProductionIdentifierArchiveRead(UmiArchiveReader *reader, UmiApplicationProductionIdentifier *value)
{
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
}
static UmiStatus UmiApplicationProductionIdentifierArchiveValidate(const UmiApplicationProductionIdentifier *value)
{
    return umi_application_production_identifier_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_application_production_identifier_archive_encode, umi_application_production_identifier_archive_decode,
    UmiApplicationProductionIdentifier, UmiApplicationProductionIdentifierArchiveSchema, UmiApplicationProductionIdentifierArchiveBound, UmiApplicationProductionIdentifierArchiveWrite, UmiApplicationProductionIdentifierArchiveRead, UmiApplicationProductionIdentifierArchiveValidate)
