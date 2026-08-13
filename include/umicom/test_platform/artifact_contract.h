/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/artifact_contract.h
 *
 * PURPOSE:
 *   Describe coverage, benchmark, failure and log artifacts through a portable
 *   contract that deliberately does not require one coverage or benchmark tool.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_ARTIFACT_CONTRACT_H
#define UMICOM_TEST_PLATFORM_ARTIFACT_CONTRACT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/attachment.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiTestPlatformArtifactKind {
    UMI_TEST_PLATFORM_ARTIFACT_GENERIC = 0,
    UMI_TEST_PLATFORM_ARTIFACT_FAILURE = 1,
    UMI_TEST_PLATFORM_ARTIFACT_LOG = 2,
    UMI_TEST_PLATFORM_ARTIFACT_COVERAGE = 3,
    UMI_TEST_PLATFORM_ARTIFACT_BENCHMARK = 4
} UmiTestPlatformArtifactKind;

typedef struct UmiTestPlatformArtifactContract {
    /* Producer and format identify tools without linking Framework to them. */
    UmiTestPlatformArtifactKind kind;
    char name[256];
    char producer[128];
    char format[128];
    char uri[1024];
    char mime_type[128];
    char schema_uri[1024];
    char checksum[128];
    uint64_t size_bytes;
} UmiTestPlatformArtifactContract;

const char *umi_test_platform_artifact_kind_text(
    UmiTestPlatformArtifactKind kind
);
UmiStatus umi_test_platform_artifact_validate(
    const UmiTestPlatformArtifactContract *contract,
    char *out_message,
    size_t capacity
);
UmiStatus umi_test_platform_artifact_to_attachment(
    const UmiTestPlatformArtifactContract *contract,
    const char *attachment_id,
    const char *result_id,
    UmiTestPlatformAttachmentSnapshot *out_attachment
);

#ifdef __cplusplus
}
#endif
#endif
