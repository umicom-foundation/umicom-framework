/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/image_inspection/gpt.h
 *
 * PURPOSE:
 *   Inspect GUID partition tables through a bounded, caller-owned byte source.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_IMAGE_INSPECTION_GPT_H
#define UMICOM_IMAGE_INSPECTION_GPT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Return success only after copying every requested byte. The caller retains
 * context and stable source bytes for the entire synchronous inspection. */
typedef UmiStatus (*UmiGptReadAt)(void *context, uint64_t offset,
    void *destination, size_t length);

typedef struct UmiGptImageInfo {
    unsigned partitions;
    uint32_t logicalBlockBytes;
    uint64_t firstUsableLba, lastUsableLba;
} UmiGptImageInfo;

/** Inspect a GPT header pair and both entry arrays. The source extent must be
 * an exact multiple of logicalBlockBytes (512 or 4096). This bounded profile
 * supports revision 1, 1..256 slots and 128, 256 or 512-byte entries. Unknown
 * revisions and larger valid profiles are UNAVAILABLE, not a successful check.
 * Invalid geometry, CRCs, reserved bytes, overlapping partitions, duplicate
 * partition GUIDs and inconsistent backup metadata are PARSE_ERROR.
 *
 * At least 16 KiB is reserved on each side for the entry array, even when the
 * declared entries occupy fewer bytes. Empty tables are valid observations.
 * The output changes only on success; callback failures propagate unchanged.
 * Allocation is bounded to two arrays of at most 128 KiB each. No device,
 * filesystem, process, boot code or firmware is opened or executed.
 *
 * Protective MBR policy belongs to the caller. Filesystem contents, partition
 * names, publisher signatures and actual bootability are not validated here.
 * Output storage must not overlap context or the borrowed source bytes. */
UmiStatus UmiGptImageInspect(UmiGptReadAt readAt, void *context,
    uint64_t bytes, uint32_t logicalBlockBytes, UmiGptImageInfo *out);
#ifdef __cplusplus
}
#endif
#endif
