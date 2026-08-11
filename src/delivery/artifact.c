/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/artifact.c
 *
 * PURPOSE:
 *   Implement one distributable file or metadata artifact and its verification state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * An artifact is one output of a release, such as an executable, ZIP file, installer, SBOM or checksum.
 */

#include "umicom/delivery/artifact.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_delivery_artifact_init(UmiDeliveryArtifact *artifact,
                                     const char *artifact_id,
                                     UmiArtifactKind kind,
                                     const char *path)
{
    UmiStatus status;
    if (artifact == NULL || artifact_id == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(artifact, 0, sizeof(*artifact));
    status = umi_delivery_copy_text(artifact->artifact_id,
                                    sizeof(artifact->artifact_id),
                                    artifact_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(artifact->path, sizeof(artifact->path), path);
    if (status != UMI_STATUS_OK) return status;
    artifact->kind = kind;
    artifact->required = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_delivery_artifact_set_digest(UmiDeliveryArtifact *artifact,
                                           const char *digest)
{
    if (artifact == NULL || digest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(artifact->digest, sizeof(artifact->digest), digest);
}

void umi_delivery_artifact_mark_verified(UmiDeliveryArtifact *artifact,
                                         int verified)
{
    if (artifact != NULL) artifact->verified = verified != 0;
}
