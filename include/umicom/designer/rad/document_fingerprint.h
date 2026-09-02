/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/document_fingerprint.h
 *
 * PURPOSE:
 *   Calculate deterministic FNV-1a fingerprints for designer manifests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DOCUMENT_FINGERPRINT_H
#define UMICOM_DESIGNER_RAD_DOCUMENT_FINGERPRINT_H
#include "umicom/designer/rad/document_manifest.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the rad document fingerprint compute operation used by this module and its
 * client applications.
 */
uint64_t umi_rad_document_fingerprint_compute(const UmiRadDocumentManifest *manifest);
#ifdef __cplusplus
}
#endif
#endif
