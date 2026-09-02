/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/artifact.c
 *
 * PURPOSE:
 *   Implement one distributable file or metadata artifact and its verification state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An artifact is one output of a release, such as an executable, ZIP file, installer, SBOM or checksum.
 */

#include "umicom/delivery/artifact.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise delivery artifact from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_delivery_artifact_init(UmiDeliveryArtifact *artifact,
                                     const char *artifact_id,
                                     UmiArtifactKind kind,
                                     const char *path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (artifact == NULL || artifact_id == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(artifact, 0, sizeof(*artifact));
    status = umi_delivery_copy_text(artifact->artifact_id,
                                    sizeof(artifact->artifact_id),
                                    artifact_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(artifact->path, sizeof(artifact->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    artifact->kind = kind;
    artifact->required = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the delivery artifact set digest operation used by this module and its client
 * applications.
 */
UmiStatus umi_delivery_artifact_set_digest(UmiDeliveryArtifact *artifact,
                                           const char *digest)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (artifact == NULL || digest == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_delivery_copy_text(artifact->digest, sizeof(artifact->digest), digest);
}

/*
 * Provide the delivery artifact mark verified operation used by this module and its client
 * applications.
 */
void umi_delivery_artifact_mark_verified(UmiDeliveryArtifact *artifact,
                                         int verified)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (artifact != NULL) artifact->verified = verified != 0;
}
