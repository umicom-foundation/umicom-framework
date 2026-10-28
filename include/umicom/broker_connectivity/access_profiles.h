/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/access_profiles.h
 *
 * PURPOSE:
 *   Persist separate local connection plans without retaining authentication or consent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_ACCESS_PROFILES_H
#define UMICOM_BROKER_ACCESS_PROFILES_H
#include "umicom/broker_connectivity/access_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_PROFILES_TEXT_CAPACITY 128U
typedef struct UmiBrokerAccessProfiles {
    UmiBrokerAccessPlan paper;
    UmiBrokerAccessPlan live;
} UmiBrokerAccessProfiles;

/* Profiles contain only mode, program, port and client ID. Authentication,
 * account identity, Live acknowledgement and auto-connect are deliberately
 * absent. A loaded plan must still pass the ordinary connection policy. */
UmiStatus UmiBrokerAccessProfilesDefault(UmiBrokerAccessProfiles *outProfiles);
UmiStatus UmiBrokerAccessProfilesValidate(const UmiBrokerAccessProfiles *profiles);

/* The bounded text format accepts exactly one Paper and one Live entry.
 * Unknown fields, trailing bytes, embedded NULs and unsupported versions fail.
 * All output arguments remain unchanged on failure. Encode includes a trailing
 * NUL in capacity requirements but excludes it from outSize. Buffers and input
 * structures must not overlap. Decode accepts a byte span, not a C string. */
UmiStatus UmiBrokerAccessProfilesEncode(const UmiBrokerAccessProfiles *profiles,
    char *outText, size_t capacity, size_t *outSize);
UmiStatus UmiBrokerAccessProfilesDecode(const void *bytes, size_t size,
    UmiBrokerAccessProfiles *outProfiles);

/* These blocking operations belong on a worker in a GUI. Use an ordinary
 * absolute path in a trusted, stable directory that already exists.
 * Load never writes; NOT_FOUND does not silently replace caller settings.
 * Store updates one mode, preserving the other mode from the current valid
 * file. Missing files start with defaults; damaged or unknown files are refused.
 * Writers cooperate through path + ".umi-guard"; that persistent guard must
 * never be removed or replaced. BUSY means retry after reviewing the result,
 * not an instruction to delete the guard. The complete text is atomically
 * replaced through UmiLocalFileReplace. Hostile parent changes and arbitrary
 * noncooperating writers are outside this contract. A failed lease release may
 * follow successful publication; reload before retrying an uncertain save.
 * These functions create no sockets, directories or authentication state. */
UmiStatus UmiBrokerAccessProfilesLoad(const char *path,
    UmiBrokerAccessProfiles *outProfiles);
UmiStatus UmiBrokerAccessProfilesStore(const char *path,
    const UmiBrokerAccessPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
