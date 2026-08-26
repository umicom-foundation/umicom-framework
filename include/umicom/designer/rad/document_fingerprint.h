/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/document_fingerprint.h
 *
 * PURPOSE:
 *   Calculate deterministic FNV-1a fingerprints for designer manifests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DOCUMENT_FINGERPRINT_H
#define UMICOM_DESIGNER_RAD_DOCUMENT_FINGERPRINT_H
#include "umicom/designer/rad/document_manifest.h"
#ifdef __cplusplus
extern "C" {
#endif
uint64_t umi_rad_document_fingerprint_compute(const UmiRadDocumentManifest *manifest);
#ifdef __cplusplus
}
#endif
#endif
