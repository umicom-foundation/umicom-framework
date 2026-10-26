/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/os_image/boot_firmware.h
 *
 * PURPOSE:
 *   Bind direct-kernel RISC-V boot observations to explicitly selected firmware bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OS_IMAGE_BOOT_FIRMWARE_H
#define UMICOM_OS_IMAGE_BOOT_FIRMWARE_H
#include "umicom/os_image/image.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Borrowed inputs, valid until the call returns. sha256 is exactly 64 lower-case
 * hexadecimal characters; bytes is nonzero and at most UMI_OS_IMAGE_MAX_FILE.
 * The caller selects a reviewed OpenSBI binary for the QEMU virt machine.
 * A digest proves byte identity, not compatibility, licence or authenticity. */
typedef struct UmiOsImageFirmware {
    const char *path;
    const char *sha256;
    uint64_t bytes;
} UmiOsImageFirmware;

/** Read an explicit regular file without following symbolic/reparse links.
 * No file is written and no process is launched. completed reports a matching
 * digest and byte count only, never a successful guest boot. */
UmiStatus UmiOsImageCheckFirmware(const UmiOsImageFirmware *firmware,
    UmiOsImageReport *report);

/** Explicit-firmware variant of UmiOsImageBoot, currently for RISC-V virt only.
 * Before execution, verify the pin and copy those bytes into the NEW result
 * directory as firmware.bin. QEMU receives that absolute captured path.
 * Recheck the capture after execution. Retain request.umi, console.log and
 * result.umi on failure as well as success. Existing outputs are never replaced.
 * The original file is not modified. The result is not secure-boot attestation;
 * host executables, recipes and the directory owner remain trusted.
 * x86 BIOS/UEFI firmware is a different contract and is not inferred here. */
UmiStatus UmiOsImageBootWithFirmware(const char *bundle, const char *qemuProgram,
    const UmiOsImageFirmware *firmware, UmiOsImageMode mode,
    unsigned timeoutSeconds, const char *resultDirectory,
    const UmiCancellationToken *cancel, UmiOsImageReport *report);

#ifdef __cplusplus
}
#endif
#endif
