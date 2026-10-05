/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/artifact.c
 * PURPOSE: Construct and validate target-to-CTest artifact mappings.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/artifact.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Provide the copy required operation used by this module and its client applications. */
static UmiStatus copy_required(char *destination, size_t capacity,
                               const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise test platform build artifact from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_build_artifact_init(
    UmiTestPlatformBuildArtifact *artifact, const char *product_id,
    const char *target_name, const char *test_name, const char *labels,
    const char *preset, bool required)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Check that test platform build artifact satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_platform_build_artifact_validate(
    const UmiTestPlatformBuildArtifact *artifact)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (artifact == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(artifact->product_id, '\0', sizeof(artifact->product_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(artifact->target_name, '\0', sizeof(artifact->target_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(artifact->test_name, '\0', sizeof(artifact->test_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(artifact->labels, '\0', sizeof(artifact->labels)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(artifact->preset, '\0', sizeof(artifact->preset)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (artifact == NULL ||
        artifact->structure_size != sizeof(*artifact) ||
        artifact->api_version != UMI_TEST_PLATFORM_BUILD_READINESS_API_VERSION ||
        artifact->product_id[0] == '\0' || artifact->target_name[0] == '\0' ||
        artifact->test_name[0] == '\0' || artifact->labels[0] == '\0' ||
        artifact->preset[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTestPlatformBuildArtifactArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfba6113840654223);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformBuildArtifact *)0)->product_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformBuildArtifact *)0)->target_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformBuildArtifact *)0)->test_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformBuildArtifact *)0)->labels)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTestPlatformBuildArtifact *)0)->preset)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTestPlatformBuildArtifactArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiTestPlatformBuildArtifact *)0)->product_id) - 1U +
        8U + sizeof(((UmiTestPlatformBuildArtifact *)0)->target_name) - 1U +
        8U + sizeof(((UmiTestPlatformBuildArtifact *)0)->test_name) - 1U +
        8U + sizeof(((UmiTestPlatformBuildArtifact *)0)->labels) - 1U +
        8U + sizeof(((UmiTestPlatformBuildArtifact *)0)->preset) - 1U +
        8U;
}
static void UmiTestPlatformBuildArtifactArchiveWrite(UmiArchiveWriter *writer, const UmiTestPlatformBuildArtifact *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->product_id, sizeof(value->product_id));
    UmiArchiveWriteText(writer, value->target_name, sizeof(value->target_name));
    UmiArchiveWriteText(writer, value->test_name, sizeof(value->test_name));
    UmiArchiveWriteText(writer, value->labels, sizeof(value->labels));
    UmiArchiveWriteText(writer, value->preset, sizeof(value->preset));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required);
}
static void UmiTestPlatformBuildArtifactArchiveRead(UmiArchiveReader *reader, UmiTestPlatformBuildArtifact *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->product_id, sizeof(value->product_id));
    UmiArchiveReadText(reader, value->target_name, sizeof(value->target_name));
    UmiArchiveReadText(reader, value->test_name, sizeof(value->test_name));
    UmiArchiveReadText(reader, value->labels, sizeof(value->labels));
    UmiArchiveReadText(reader, value->preset, sizeof(value->preset));
    value->required = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTestPlatformBuildArtifactArchiveValidate(const UmiTestPlatformBuildArtifact *value)
{
    return umi_test_platform_build_artifact_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_test_platform_build_artifact_archive_encode, umi_test_platform_build_artifact_archive_decode,
    UmiTestPlatformBuildArtifact, UmiTestPlatformBuildArtifactArchiveSchema, UmiTestPlatformBuildArtifactArchiveBound, UmiTestPlatformBuildArtifactArchiveWrite, UmiTestPlatformBuildArtifactArchiveRead, UmiTestPlatformBuildArtifactArchiveValidate)
