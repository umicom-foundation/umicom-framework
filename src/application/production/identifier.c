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

