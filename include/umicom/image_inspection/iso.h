/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/image_inspection/iso.h
 *
 * PURPOSE:
 *   Share bounded ISO and boot-catalogue observations between native media workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IMAGE_INSPECTION_ISO_H
#define UMICOM_IMAGE_INSPECTION_ISO_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Read exactly length bytes or return an error. The caller owns context and
 * stable source bytes throughout this synchronous call. No handle is retained.
 * A callback must not report success for a short read. */
typedef UmiStatus (*UmiIsoReadAt)(void *context,uint64_t offset,
    void *destination,size_t length);

typedef struct UmiIsoImageInfo {
    int iso9660;
    int elTorito;
    int biosEntry;
    int efiEntry;
} UmiIsoImageInfo;

/** Inspect a bounded ISO9660 / no-emulation El Torito profile without I/O policy.
 * Non-ISO input succeeds with zero flags. A complete CD001 identifier at the
 * descriptor offset is enough to recognise a truncated structure and refuse it.
 * A shorter prefix cannot establish this format and returns zero flags without
 * reading beyond the supplied extent. Callback errors leave output unchanged.
 * Malformed recognised structures fail;
 * unsupported boot platforms, emulation or catalogue extensions are unavailable.
 * At most 64 volume descriptors and one 2048-byte catalogue are read. The output
 * is replaced only on success. This does not validate every filesystem record,
 * the EFI partition's filesystem, signatures, publisher identity or bootability.
 *
 * EFI sector counts zero and one extend to the end of the source, as specified
 * by UEFI. Other counts describe 512-byte units; catalogue LBAs use 2048 bytes.
 * This service never allocates a boot image, executes code or opens a device. */
UmiStatus UmiIsoImageInspect(UmiIsoReadAt readAt,void *context,
    uint64_t bytes,UmiIsoImageInfo *out);

#ifdef __cplusplus
}
#endif
#endif
