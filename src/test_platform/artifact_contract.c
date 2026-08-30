/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/artifact_contract.c
 *
 * PURPOSE:
 *   Validate tool-neutral test artifacts and convert them to retained records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/artifact_contract.h"

#include <stdio.h>
#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

const char *umi_test_platform_artifact_kind_text(
    UmiTestPlatformArtifactKind kind)
{
    switch (kind) {
        case UMI_TEST_PLATFORM_ARTIFACT_GENERIC: return "generic";
        case UMI_TEST_PLATFORM_ARTIFACT_FAILURE: return "failure";
        case UMI_TEST_PLATFORM_ARTIFACT_LOG: return "log";
        case UMI_TEST_PLATFORM_ARTIFACT_COVERAGE: return "coverage";
        case UMI_TEST_PLATFORM_ARTIFACT_BENCHMARK: return "benchmark";
        default: return "unknown";
    }
}

UmiStatus umi_test_platform_artifact_validate(
    const UmiTestPlatformArtifactContract *contract,
    char *out_message,
    size_t capacity)
{
    const char *message = "Artifact contract is valid.";
    UmiStatus status = UMI_STATUS_OK;
    if (contract == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (contract->kind < UMI_TEST_PLATFORM_ARTIFACT_GENERIC ||
        contract->kind > UMI_TEST_PLATFORM_ARTIFACT_BENCHMARK) {
        message = "Artifact kind is outside the supported contract.";
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (contract->name[0] == '\0' ||
               contract->producer[0] == '\0' ||
               contract->format[0] == '\0' ||
               contract->uri[0] == '\0') {
        message = "Artifact name, producer, format and URI are required.";
        status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (out_message != NULL && capacity > 0U) {
        if (snprintf(out_message, capacity, "%s", message) < 0) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
    }
    return status;
}

UmiStatus umi_test_platform_artifact_to_attachment(
    const UmiTestPlatformArtifactContract *contract,
    const char *attachment_id,
    const char *result_id,
    UmiTestPlatformAttachmentSnapshot *out_attachment)
{
    UmiStatus status;
    if (attachment_id == NULL || attachment_id[0] == '\0' ||
        result_id == NULL || result_id[0] == '\0' ||
        out_attachment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_test_platform_artifact_validate(contract, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_attachment, 0, sizeof(*out_attachment));
    out_attachment->struct_size = (uint32_t)sizeof(*out_attachment);
    out_attachment->api_version = UMI_TEST_PLATFORM_ATTACHMENT_API_VERSION;
    copy_text(out_attachment->id, sizeof(out_attachment->id), attachment_id);
    copy_text(out_attachment->result_id, sizeof(out_attachment->result_id),
              result_id);
    copy_text(out_attachment->name, sizeof(out_attachment->name),
              contract->name);
    copy_text(out_attachment->kind, sizeof(out_attachment->kind),
              umi_test_platform_artifact_kind_text(contract->kind));
    copy_text(out_attachment->producer, sizeof(out_attachment->producer),
              contract->producer);
    copy_text(out_attachment->uri, sizeof(out_attachment->uri), contract->uri);
    copy_text(out_attachment->mime_type, sizeof(out_attachment->mime_type),
              contract->mime_type);
    copy_text(out_attachment->schema_uri, sizeof(out_attachment->schema_uri),
              contract->schema_uri);
    copy_text(out_attachment->checksum, sizeof(out_attachment->checksum),
              contract->checksum);
    out_attachment->size_bytes = contract->size_bytes;
    return UMI_STATUS_OK;
}
