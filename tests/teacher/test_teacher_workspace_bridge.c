/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_teacher_workspace_bridge.c
 *
 * PURPOSE:
 *   Implement the test teacher workspace bridge behavior for
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
#include "umicom/teacher/teacher_workspace_bridge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/teacher/teacher_workspace_bridge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTeacherTeacherWorkspaceBridgeTransferEqual(const UmiTeacherTeacherWorkspaceBridge *a, const UmiTeacherTeacherWorkspaceBridge *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        a->language == b->language &&
        a->level == b->level &&
        a->weight == b->weight &&
        a->required_score == b->required_score &&
        a->revision == b->revision &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTeacherTeacherWorkspaceBridgeTransferTails(UmiTeacherTeacherWorkspaceBridge *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTeacherTeacherWorkspaceBridgeTransferMalformed(const UmiTeacherTeacherWorkspaceBridge *sample)
{
    (void)sample;
    {
        UmiTeacherTeacherWorkspaceBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_teacher_teacher_workspace_bridge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_teacher_teacher_workspace_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTeacherTeacherWorkspaceBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_teacher_teacher_workspace_bridge_validate(&invalid) != UMI_STATUS_OK) ||
            umi_teacher_teacher_workspace_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTeacherTeacherWorkspaceBridgeTransferCases, UmiTeacherTeacherWorkspaceBridge,
    umi_teacher_teacher_workspace_bridge_archive_encode, umi_teacher_teacher_workspace_bridge_archive_decode,
    UmiTeacherTeacherWorkspaceBridgeTransferEqual, UmiTeacherTeacherWorkspaceBridgeTransferTails, UmiTeacherTeacherWorkspaceBridgeTransferMalformed)

int main(void) {
    UmiTeacherTeacherWorkspaceBridge value;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_teacher_workspace_bridge_configure(&value, "item", "Item", UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER, 10U, 70U) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_teacher_workspace_bridge_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiTeacherTeacherWorkspaceBridgeTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_teacher_workspace_bridge_priority(&value, 60U) != 70U) return 3;
    return 0;
}
