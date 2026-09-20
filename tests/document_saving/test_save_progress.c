/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_progress.c
 * PURPOSE: Verify readable Save All reports without changing their source state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/save_session.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiDocumentSaveProgress p = {0};
    p.phase = UMI_DOCUMENT_SAVE_READY; p.last_status = UMI_STATUS_OK;
    p.total = 3U; p.saved = 1U; p.remaining = 2U;
    memcpy(p.display_name, "notes.h", 8U);
    char text[768] = "unchanged";
    if (strcmp(argv[1], "invalid") == 0) {
        CHECK(UmiDocumentSaveProgressFormat(NULL, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveProgressFormat(&p, NULL, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveProgressFormat(&p, text, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text, "unchanged") == 0);
    } else if (strcmp(argv[1], "name") == 0) {
        memset(p.display_name, 'x', sizeof(p.display_name));
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text, "unchanged") == 0);
    } else if (strcmp(argv[1], "counts") == 0) {
        p.total = SIZE_MAX; p.saved = SIZE_MAX; p.unchanged = 1U; p.remaining = 0U;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        p.unchanged = 0U; p.remaining = 1U;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text, "unchanged") == 0);
    } else if (strcmp(argv[1], "phase") == 0) {
        p.phase = (UmiDocumentSavePhase)99;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        p.phase = UMI_DOCUMENT_SAVE_CANCELLED;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        p.phase = UMI_DOCUMENT_SAVE_COMPLETE;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        p.phase = UMI_DOCUMENT_SAVE_FAILED;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text, "unchanged") == 0);
    } else if (strcmp(argv[1], "capacity") == 0) {
        char complete[768];
        CHECK(UmiDocumentSaveProgressFormat(&p, complete, sizeof(complete)) == UMI_STATUS_OK);
        size_t bytes = strlen(complete);
        CHECK(UmiDocumentSaveProgressFormat(&p, text, bytes) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text, "unchanged") == 0);
        CHECK(UmiDocumentSaveProgressFormat(&p, text, bytes + 1U) == UMI_STATUS_OK);
        CHECK(strcmp(text, complete) == 0);
    } else if (strcmp(argv[1], "phases") == 0) {
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "1 saved, 2 remaining") != NULL);
        p.phase = UMI_DOCUMENT_SAVE_NEEDS_PATH;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "notes.h") != NULL);
        p.phase = UMI_DOCUMENT_SAVE_CANCELLED; p.last_status = UMI_STATUS_CANCELLED;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "cancelled") != NULL && strstr(text, "2 remaining") != NULL);
        p.phase = UMI_DOCUMENT_SAVE_FAILED; p.last_status = UMI_STATUS_IO_ERROR;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "stopped") != NULL && strstr(text, "notes.h") != NULL);
        p.phase = UMI_DOCUMENT_SAVE_COMPLETE; p.last_status = UMI_STATUS_OK;
        p.unchanged = 2U; p.remaining = 0U;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "1 saved, 2 already saved") != NULL);
    } else if (strcmp(argv[1], "immutable") == 0) {
        UmiDocumentSaveProgress before = p;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(memcmp(&p, &before, sizeof(p)) == 0);
    } else return 2;
    return 0;
}
