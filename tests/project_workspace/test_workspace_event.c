/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_event.c
 *
 * PURPOSE:
 *   Implement the test workspace event behavior for
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
#include "umicom/project/workspace/workspace_event.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/project/workspace/workspace_event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiProjectWorkspaceWorkspaceEventTransferEqual(const UmiProjectWorkspaceWorkspaceEvent *a, const UmiProjectWorkspaceWorkspaceEvent *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->topic, b->topic) == 0 &&
        strcmp(a->payload, b->payload) == 0 &&
        a->sequence == b->sequence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiProjectWorkspaceWorkspaceEventTransferTails(UmiProjectWorkspaceWorkspaceEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->topic) + 1U;
        memset(value->topic + used, 0xa5, sizeof(value->topic) - used);
    }
    {
        size_t used = strlen(value->payload) + 1U;
        memset(value->payload + used, 0xa5, sizeof(value->payload) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiProjectWorkspaceWorkspaceEventTransferMalformed(const UmiProjectWorkspaceWorkspaceEvent *sample)
{
    (void)sample;
    {
        UmiProjectWorkspaceWorkspaceEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_workspace_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_workspace_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceWorkspaceEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.topic, 'x', sizeof(invalid.topic));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_workspace_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_workspace_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated topic was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiProjectWorkspaceWorkspaceEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payload, 'x', sizeof(invalid.payload));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_project_workspace_workspace_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_project_workspace_workspace_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payload was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiProjectWorkspaceWorkspaceEventTransferCases, UmiProjectWorkspaceWorkspaceEvent,
    umi_project_workspace_workspace_event_archive_encode, umi_project_workspace_workspace_event_archive_decode,
    UmiProjectWorkspaceWorkspaceEventTransferEqual, UmiProjectWorkspaceWorkspaceEventTransferTails, UmiProjectWorkspaceWorkspaceEventTransferMalformed)

int main(void) {
    UmiProjectWorkspaceWorkspaceEvent v;
    CHECK(umi_project_workspace_workspace_event_init(&v,"id","workspace.changed","{}") == UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_event_validate(&v)==UMI_STATUS_OK);
    if (UmiProjectWorkspaceWorkspaceEventTransferCases(&v) != 0) return 1;

    return 0;
}
