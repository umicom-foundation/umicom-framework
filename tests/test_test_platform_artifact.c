/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_artifact.c
 *
 * PURPOSE:
 *   Verify tool-neutral coverage and benchmark attachment contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestPlatformArtifactContract contract;
    UmiTestPlatformAttachmentSnapshot attachment;
    char message[128];
    (void)memset(&contract, 0, sizeof(contract));
    contract.kind = UMI_TEST_PLATFORM_ARTIFACT_COVERAGE;
    (void)strcpy(contract.name, "Framework coverage");
    (void)strcpy(contract.producer, "llvm-cov");
    (void)strcpy(contract.format, "cobertura");
    (void)strcpy(contract.uri, "artifacts/coverage.xml");
    (void)strcpy(contract.mime_type, "application/xml");
    (void)strcpy(contract.schema_uri,
                 "https://cobertura.github.io/xml/coverage");
    (void)strcpy(contract.checksum, "sha256:0123456789");
    contract.size_bytes = 4096U;
    assert(umi_test_platform_artifact_validate(&contract, message,
                                               sizeof(message)) ==
           UMI_STATUS_OK);
    assert(umi_test_platform_artifact_to_attachment(
               &contract, "attachment.coverage", "result.framework",
               &attachment) == UMI_STATUS_OK);
    assert(strcmp(attachment.kind, "coverage") == 0);
    assert(strcmp(attachment.producer, "llvm-cov") == 0);
    assert(attachment.size_bytes == 4096U);
    contract.producer[0] = '\0';
    assert(umi_test_platform_artifact_validate(&contract, message,
                                               sizeof(message)) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
