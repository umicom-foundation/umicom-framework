/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/interop/profile_name.h
 *
 * PURPOSE:
 *   Expose the shared profile-name policy through fixed-width results and explicit buffer capacity for foreign-language clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INTEROP_PROFILE_NAME_H
#define UMICOM_INTEROP_PROFILE_NAME_H
#include <stddef.h>
#include <stdint.h>
#if defined(_WIN32) && defined(UMICOM_PROFILE_FFI_BUILD)
#define UMICOM_PROFILE_FFI_API __declspec(dllexport)
#elif defined(__GNUC__) && defined(UMICOM_PROFILE_FFI_BUILD)
#define UMICOM_PROFILE_FFI_API __attribute__((visibility("default")))
#else
#define UMICOM_PROFILE_FFI_API
#endif
#ifdef __cplusplus
extern "C" {
#endif
/* The bridge does not expose compiler-dependent enum layout or heap ownership.
 * Version 1 returns Framework status numbers: 0 success, 1 invalid input,
 * 3 insufficient output capacity. Size parameters use the target's size_t ABI. */
UMICOM_PROFILE_FFI_API uint32_t UmiInteropProfileNameVersion(void);
UMICOM_PROFILE_FFI_API uint32_t UmiInteropProfileNameCapacity(void);
/* name must designate length readable bytes; out must designate capacity
 * writable bytes. No pointer is retained. On success exactly the required
 * capacity is written, including the NUL terminator and zero padding. On any
 * refusal every output byte is unchanged. Input/output may overlap. This is
 * naming only, not account creation, authentication or persistent storage. */
UMICOM_PROFILE_FFI_API int32_t UmiInteropProfileNameNormalize(
    const uint8_t *name, size_t length, uint8_t *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
