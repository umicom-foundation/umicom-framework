/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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
#include "umicom/project/workspace/types.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/project/workspace/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiProjectWorkspaceNamedStateTransferEqual(const UmiProjectWorkspaceNamedState *a, const UmiProjectWorkspaceNamedState *b)
{
    return a->structure_size == b->structure_size &&
        a->api_version == b->api_version &&
        strcmp(a->id, b->id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->detail, b->detail) == 0 &&
        a->revision == b->revision &&
        a->flags == b->flags &&
        a->priority == b->priority &&
        a->state == b->state &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiProjectWorkspaceNamedStateTransferTails(UmiProjectWorkspaceNamedState *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiProjectWorkspaceNamedStateTransferMalformed(const UmiProjectWorkspaceNamedState *sample)
{
    (void)sample;
    {
        UmiProjectWorkspaceNamedState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_named_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_named_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceNamedState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_named_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_named_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceNamedState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.detail, 'x', sizeof(invalid.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_named_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_named_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated detail was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiProjectWorkspaceNamedStateTransferCases, UmiProjectWorkspaceNamedState,
    umi_project_workspace_named_state_archive_encode, umi_project_workspace_named_state_archive_decode,
    UmiProjectWorkspaceNamedStateTransferEqual, UmiProjectWorkspaceNamedStateTransferTails, UmiProjectWorkspaceNamedStateTransferMalformed)

int main(void) {
    UmiProjectWorkspaceNamedState v,w;
    CHECK(umi_project_workspace_named_state_init(&v,"x")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_init(&w,"x")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_set_name(&v,"Workspace")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_named_state_validate(&v)==UMI_STATUS_OK);
    if (UmiProjectWorkspaceNamedStateTransferCases(&v) != 0) return 1;

    CHECK(umi_project_workspace_named_state_same_identity(&v,&w));
    CHECK(strcmp(umi_project_workspace_state_text(v.state),"ready")==0);
    return 0;
}
