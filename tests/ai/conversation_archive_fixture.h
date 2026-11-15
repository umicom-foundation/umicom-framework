/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/conversation_archive_fixture.h
 *
 * PURPOSE:
 *   Construct valid heap-backed conversation records for archive admission and ownership regressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_CONVERSATION_ARCHIVE_FIXTURE_H
#define UMICOM_TEST_CONVERSATION_ARCHIVE_FIXTURE_H
#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/ai/conversation_archive.h"

static UmiAiConversationRecord *archive_record_fixture(const char *id)
{
    UmiAiConversationRecord *record = calloc(1U, sizeof *record);
    UmiAiMessage message;
    assert(record != NULL);
    assert(umi_ai_conversation_record_init(record, id, "provider.local", "model.chat",
        "workspace.fixture", "A saved conversation", 1U) == UMI_STATUS_OK);
    assert(umi_ai_message_set(&message, UMI_AI_ROLE_USER, "user", "Preserve this message") == UMI_STATUS_OK);
    assert(umi_ai_conversation_add(&record->conversation, &message) == UMI_STATUS_OK);
    return record;
}
#endif
