/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/artifact.c
 * PURPOSE: Construct and validate target-to-CTest artifact mappings.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/artifact.h"

#include <string.h>

static UmiStatus copy_required(char *destination, size_t capacity,
                               const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_build_artifact_init(
    UmiTestPlatformBuildArtifact *artifact, const char *product_id,
    const char *target_name, const char *test_name, const char *labels,
    const char *preset, bool required)
{
    UmiStatus status;
    if (artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(artifact, 0, sizeof(*artifact));
    artifact->structure_size = (uint32_t)sizeof(*artifact);
    artifact->api_version = UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION;
#define COPY_FIELD(field, value)                                                \
    do {                                                                         \
        status = copy_required(artifact->field, sizeof(artifact->field), value); \
        if (status != UMI_STATUS_OK) return status;                              \
    } while (0)
    COPY_FIELD(product_id, product_id);
    COPY_FIELD(target_name, target_name);
    COPY_FIELD(test_name, test_name);
    COPY_FIELD(labels, labels);
    COPY_FIELD(preset, preset);
#undef COPY_FIELD
    artifact->required = required;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_build_artifact_validate(
    const UmiTestPlatformBuildArtifact *artifact)
{
    if (artifact == NULL ||
        artifact->structure_size != sizeof(*artifact) ||
        artifact->api_version != UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION ||
        artifact->product_id[0] == '\0' || artifact->target_name[0] == '\0' ||
        artifact->test_name[0] == '\0' || artifact->labels[0] == '\0' ||
        artifact->preset[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
