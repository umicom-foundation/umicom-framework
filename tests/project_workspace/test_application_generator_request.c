/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_application_generator_request.c
 *
 * PURPOSE:
 *   Implement the test application generator request behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/project/workspace/application_generator_request.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/project/workspace/application_generator_request.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiProjectWorkspaceApplicationGeneratorRequestTransferEqual(const UmiProjectWorkspaceApplicationGeneratorRequest *a, const UmiProjectWorkspaceApplicationGeneratorRequest *b)
{
    return a->base.structure_size == b->base.structure_size &&
        a->base.api_version == b->base.api_version &&
        strcmp(a->base.id, b->base.id) == 0 &&
        strcmp(a->base.name, b->base.name) == 0 &&
        strcmp(a->base.detail, b->base.detail) == 0 &&
        a->base.revision == b->base.revision &&
        a->base.flags == b->base.flags &&
        a->base.priority == b->base.priority &&
        a->base.state == b->base.state &&
        a->base.enabled == b->base.enabled &&
        a->metric == b->metric;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiProjectWorkspaceApplicationGeneratorRequestTransferTails(UmiProjectWorkspaceApplicationGeneratorRequest *value)
{
    (void)value;
    {
        size_t used = strlen(value->base.id) + 1U;
        memset(value->base.id + used, 0xa5, sizeof(value->base.id) - used);
    }
    {
        size_t used = strlen(value->base.name) + 1U;
        memset(value->base.name + used, 0xa5, sizeof(value->base.name) - used);
    }
    {
        size_t used = strlen(value->base.detail) + 1U;
        memset(value->base.detail + used, 0xa5, sizeof(value->base.detail) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiProjectWorkspaceApplicationGeneratorRequestTransferMalformed(const UmiProjectWorkspaceApplicationGeneratorRequest *sample)
{
    (void)sample;
    {
        UmiProjectWorkspaceApplicationGeneratorRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base.id, 'x', sizeof(invalid.base.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_application_generator_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_application_generator_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base.id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceApplicationGeneratorRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base.name, 'x', sizeof(invalid.base.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_application_generator_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_application_generator_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base.name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceApplicationGeneratorRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base.detail, 'x', sizeof(invalid.base.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_application_generator_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_application_generator_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base.detail was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiProjectWorkspaceApplicationGeneratorRequestTransferCases, UmiProjectWorkspaceApplicationGeneratorRequest,
    umi_project_workspace_application_generator_request_archive_encode, umi_project_workspace_application_generator_request_archive_decode,
    UmiProjectWorkspaceApplicationGeneratorRequestTransferEqual, UmiProjectWorkspaceApplicationGeneratorRequestTransferTails, UmiProjectWorkspaceApplicationGeneratorRequestTransferMalformed)

int main(void) {
    UmiProjectWorkspaceApplicationGeneratorRequest a,b;
    CHECK(umi_project_workspace_application_generator_request_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_request_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_request_set_name(&a,"Application Generator Request")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_request_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_application_generator_request_set_metric(&a,7U);
    CHECK(umi_project_workspace_application_generator_request_validate(&a)==UMI_STATUS_OK);
    if (UmiProjectWorkspaceApplicationGeneratorRequestTransferCases(&a) != 0) return 1;

    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_application_generator_request_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Application Generator Request")==0);
    return 0;
}
