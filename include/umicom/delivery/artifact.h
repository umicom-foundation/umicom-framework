/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/artifact.h
 *
 * PURPOSE:
 *   Represent one distributable file or metadata artifact and its verification state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * An artifact is one output of a release, such as an executable, ZIP file, installer, SBOM or checksum.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ARTIFACT_H
#define INCLUDE_UMICOM_DELIVERY_ARTIFACT_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryArtifact {
    char artifact_id[UMI_DELIVERY_ID_CAPACITY];
    UmiArtifactKind kind;
    char path[UMI_DELIVERY_PATH_CAPACITY];
    uint64_t size_bytes;
    char digest[UMI_DELIVERY_DIGEST_CAPACITY];
    int required;
    int verified;
} UmiDeliveryArtifact;

UmiStatus umi_delivery_artifact_init(UmiDeliveryArtifact *artifact,
                                     const char *artifact_id,
                                     UmiArtifactKind kind,
                                     const char *path);
UmiStatus umi_delivery_artifact_set_digest(UmiDeliveryArtifact *artifact,
                                           const char *digest);
void umi_delivery_artifact_mark_verified(UmiDeliveryArtifact *artifact,
                                         int verified);

#ifdef __cplusplus
}
#endif

#endif
