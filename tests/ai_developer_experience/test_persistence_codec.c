/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_persistence_codec.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience persistence codec.
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
#include "umicom/ai_developer_experience/persistence_codec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSessionStore *store = NULL;
    const char *text = "line one\nline two\nline three\n";
    char restored[256];
    size_t chunks = 0U;
    size_t length = 0U;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_ai_developer_persistence_save_text(
        store, "test.text", text, strlen(text), &chunks) == UMI_STATUS_OK);
    assert(chunks > 0U);
    assert(umi_ai_developer_persistence_load_text(
        store, "test.text", chunks,
        restored, sizeof(restored), &length) == UMI_STATUS_OK);
    assert(length == strlen(text));
    assert(strcmp(restored, text) == 0);

    umi_session_store_destroy(store);
    return 0;
}

