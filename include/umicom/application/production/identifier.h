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
#include "umicom/base/value_archive.h"

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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_application_production_identifier_archive_encode(const UmiApplicationProductionIdentifier *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_application_production_identifier_archive_decode(const void *bytes, size_t byte_count,
    UmiApplicationProductionIdentifier *value);

#ifdef __cplusplus
}
#endif
#endif

