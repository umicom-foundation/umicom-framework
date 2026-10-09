/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/profile_store_internal.h
 * PURPOSE: Share field-level profile persistence inside the owning Data Server transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_PROFILE_STORE_INTERNAL_H
#define UMICOM_BUILD_PROFILE_STORE_INTERNAL_H
#include "umicom/build/profile_store.h"
/* Internal callers own a transaction. Prefixes are bounded module-generated
 * keys, never user-supplied SQL. The same field table serves the current
 * workspace profile and saved configurations, so new settings cannot drift. */
UmiStatus UmiBuildProfileStorageIdentity(const char *root, char *normalised, char *prefix);
UmiStatus UmiBuildProfileStorageRead(UmiDataServer *server, const char *prefix, const char *root,
                                     UmiBuildProfile *profile, uint64_t *revision);
UmiStatus UmiBuildProfileStorageWrite(UmiDataServer *server, const char *prefix,
                                      const UmiBuildProfile *profile, uint64_t revision);
/* Remove only recognised fields while the caller owns a transaction. Unknown
 * keys under the record prefix refuse removal instead of discarding future data. */
UmiStatus UmiBuildProfileStorageRemove(UmiDataServer *server, const char *prefix);
#endif
