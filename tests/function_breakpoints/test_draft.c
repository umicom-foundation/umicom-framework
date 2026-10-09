/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/function_breakpoints/test_draft.c
 * PURPOSE: Check complete function-breakpoint input before any adapter request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/function_breakpoint_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiDebugFunctionDraft *draft = calloc(1U, sizeof *draft);
    if (draft == NULL)
        return 1;
    const char *mode = argv[1];
    strcpy(draft->session_id, "session");
    draft->revision = 1U;
    draft->count = 1U;
    strcpy(draft->entries[0].breakpoint.name, "main");
    draft->entries[0].enabled = 1;
    bool valid = false;
    if (strcmp(mode, "normal") == 0)
        valid = true;
    else if (strcmp(mode, "empty") == 0)
    {
        draft->count = 0U;
        valid = true;
    }
    else if (strcmp(mode, "disabled") == 0)
    {
        draft->entries[0].enabled = 0;
        valid = true;
    }
    else if (strcmp(mode, "conditions") == 0)
    {
        strcpy(draft->entries[0].breakpoint.condition, "count > 3");
        strcpy(draft->entries[0].breakpoint.hit_condition, "5");
        valid = true;
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        strcpy(draft->entries[0].breakpoint.name, "caf\xc3\xa9");
        valid = true;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        draft->count = 2U;
        draft->entries[1] = draft->entries[0];
    }
    else if (strcmp(mode, "count") == 0)
        draft->count = UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT + 1U;
    else if (strcmp(mode, "spaces") == 0)
        strcpy(draft->entries[0].breakpoint.name, "   ");
    else if (strcmp(mode, "name-empty") == 0)
        draft->entries[0].breakpoint.name[0] = '\0';
    else if (strcmp(mode, "name-long") == 0)
        memset(draft->entries[0].breakpoint.name, 'x', sizeof draft->entries[0].breakpoint.name);
    else if (strcmp(mode, "condition-long") == 0)
        memset(draft->entries[0].breakpoint.condition, 'x',
               sizeof draft->entries[0].breakpoint.condition);
    else if (strcmp(mode, "hit-long") == 0)
        memset(draft->entries[0].breakpoint.hit_condition, 'x',
               sizeof draft->entries[0].breakpoint.hit_condition);
    else if (strcmp(mode, "session-long") == 0)
        memset(draft->session_id, 'x', sizeof draft->session_id);
    else if (strcmp(mode, "newline") == 0)
        strcpy(draft->entries[0].breakpoint.name, "main\nother");
    else if (strcmp(mode, "utf8") == 0)
    {
        draft->entries[0].breakpoint.name[0] = (char)0xFF;
        draft->entries[0].breakpoint.name[1] = '\0';
    }
    else if (strcmp(mode, "boolean") == 0)
        draft->entries[0].enabled = 2;
    else
    {
        free(draft);
        return 2;
    }
    UmiStatus status = UmiDebugFunctionDraftValidate(draft);
    int failed = (status == UMI_STATUS_OK) != valid;
    if (failed)
        fprintf(stderr, "Unexpected status for %s: %s\n", mode, umi_status_text(status));
    free(draft);
    return failed;
}
