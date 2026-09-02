/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/identifier.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_IDENTIFIER_H
#define UMICOM_APPLICATION_PRODUCTION_IDENTIFIER_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application production identifier data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationProductionIdentifier {
    char value[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
} UmiApplicationProductionIdentifier;

/**
 * Copy application production identifier into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_production_identifier_set(
    UmiApplicationProductionIdentifier *identifier, const char *value);
/**
 * Check that application production identifier satisfies its contract before another
 * service relies on it.
 */
int umi_application_production_identifier_valid(
    const UmiApplicationProductionIdentifier *identifier);
/**
 * Provide the application production identifier equal operation used by this module and
 * its client applications.
 */
int umi_application_production_identifier_equal(
    const UmiApplicationProductionIdentifier *left,
    const UmiApplicationProductionIdentifier *right);

#ifdef __cplusplus
}
#endif
#endif

