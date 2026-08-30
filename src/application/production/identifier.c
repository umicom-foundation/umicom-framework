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

UmiStatus umi_application_production_identifier_set(
    UmiApplicationProductionIdentifier *identifier, const char *value)
{
    size_t length;
    if (identifier == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value);
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= sizeof(identifier->value)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(identifier->value, value, length + 1U);
    return UMI_STATUS_OK;
}

int umi_application_production_identifier_valid(
    const UmiApplicationProductionIdentifier *identifier)
{
    return identifier != NULL && identifier->value[0] != '\0';
}

int umi_application_production_identifier_equal(
    const UmiApplicationProductionIdentifier *left,
    const UmiApplicationProductionIdentifier *right)
{
    return umi_application_production_identifier_valid(left) &&
           umi_application_production_identifier_valid(right) &&
           strcmp(left->value, right->value) == 0;
}

