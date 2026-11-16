/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/model_formats/safetensors_file.h
 *
 * PURPOSE:
 *   Bind Safetensors layout inspection to complete bytes streamed from one checked regular-file owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MODEL_FORMATS_SAFETENSORS_FILE_H
#define UMICOM_MODEL_FORMATS_SAFETENSORS_FILE_H
#include "umicom/model_formats/safetensors.h"
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSafetensorsFileIdentity {
    uint64_t bytes;
    unsigned char sha256[UMI_SHA256_BYTES];
} UmiSafetensorsFileIdentity;
typedef struct UmiSafetensorsFileInspection {
    UmiSafetensorsFileIdentity file;
    UmiSafetensorsHeaderInfo layout;
} UmiSafetensorsFileInspection;

/* Stream an ordinary absolute UTF-8 path through UmiInputFileVisit, hash all
 * bytes and retain at most the bounded length/header prefix. Reuse that owner's
 * trusted-parent, regular-leaf, no-final-link and before/after metadata checks.
 * Blocking file I/O belongs on an application worker. The reader is neither an
 * immutable snapshot nor a filesystem sandbox; later path reuse needs recheck.
 *
 * Optional expected identity requires 10..MAX_FILE_BYTES and a binary SHA-256.
 * It can borrow out_inspection->file. Its byte count also bounds the read: an
 * oversized file returns CAPACITY_EXCEEDED before data callbacks. A shorter
 * file or different digest returns INVALID_STATE after a successful read.
 * NULL expected records observed identity without asserting publisher trust.
 *
 * Publish identity and layout together only after read, cleanup, expected
 * identity and header inspection all succeed. Preserve every output byte on
 * failure. The path, input objects and token must remain alive and serialized.
 * No download, tensor numerical evaluation, runtime launch or readiness claim
 * occurs. A matching digest is not a signature or proof of model compatibility. */
UmiStatus UmiSafetensorsFileInspect(const char *path,
    const UmiSafetensorsFileIdentity *expected,const UmiCancellationToken *cancel,
    UmiSafetensorsFileInspection *out_inspection);
#ifdef __cplusplus
}
#endif
#endif
