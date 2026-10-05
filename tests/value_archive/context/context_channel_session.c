/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_session.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/session.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextSessionTransferEqual(const UmiContextSession *a, const UmiContextSession *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->user_id, b->user_id) == 0 &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->active_channel_id, b->active_channel_id) == 0 &&
        strcmp(a->active_context_id, b->active_context_id) == 0 &&
        a->last_sequence == b->last_sequence &&
        a->clean_shutdown == b->clean_shutdown &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextSessionTransferTails(UmiContextSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->user_id) + 1U;
        memset(value->user_id + used, 0xa5, sizeof(value->user_id) - used);
    }
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->active_channel_id) + 1U;
        memset(value->active_channel_id + used, 0xa5, sizeof(value->active_channel_id) - used);
    }
    {
        size_t used = strlen(value->active_context_id) + 1U;
        memset(value->active_context_id + used, 0xa5, sizeof(value->active_context_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextSessionTransferMalformed(const UmiContextSession *sample)
{
    (void)sample;
    {
        UmiContextSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.user_id, 'x', sizeof(invalid.user_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated user_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_channel_id, 'x', sizeof(invalid.active_channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_context_id, 'x', sizeof(invalid.active_context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_context_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextSessionTransferCases, UmiContextSession,
    umi_context_session_archive_encode, umi_context_session_archive_decode,
    UmiContextSessionTransferEqual, UmiContextSessionTransferTails, UmiContextSessionTransferMalformed)

int main(void)
{
    UmiContextSession value;
    umi_context_session_init(&value);
    value.session_id[0] = 's';
    value.user_id[0] = 's';
    value.workspace_id[0] = 's';
    value.active_channel_id[0] = 's';
    value.active_context_id[0] = 's';
    value.last_sequence = (uint64_t)17U;
    value.clean_shutdown = true;
    value.revision = (uint64_t)17U;
    if (umi_context_session_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextSessionTransferCases(&value) != 0) return 1;

    return 0;
}
