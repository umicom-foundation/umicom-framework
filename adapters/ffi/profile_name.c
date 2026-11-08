/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/ffi/profile_name.c
 *
 * PURPOSE:
 *   Check foreign buffer capacity and delegate all naming rules to the portable Framework policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/interop/profile_name.h"
#include "umicom/security/profile_name.h"
uint32_t UmiInteropProfileNameVersion(void)
{
    return UINT32_C(1);
}
uint32_t UmiInteropProfileNameCapacity(void)
{
    return (uint32_t)UMI_LOCAL_PROFILE_NAME_CAPACITY;
}
int32_t UmiInteropProfileNameNormalize(const uint8_t *name, size_t length,
                                     uint8_t *out, size_t capacity)
{
    /* Foreign callers need an explicit capacity check before the existing
     * fixed-array contract can be used. No temporary allocation crosses ABI. */
    if (name == NULL || out == NULL) return (int32_t)UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < UMI_LOCAL_PROFILE_NAME_CAPACITY) return (int32_t)UMI_STATUS_CAPACITY_EXCEEDED;
    return (int32_t)UmiLocalProfileNameBytes((const char *)name, length, (char *)out);
}
