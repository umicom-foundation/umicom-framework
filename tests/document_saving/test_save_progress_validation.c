/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_progress_validation.c
 * PURPOSE: Check copied save results and failure-message preservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/save_session.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiDocumentSaveProgress p = {0};
    p.phase = UMI_DOCUMENT_SAVE_READY; p.last_status = UMI_STATUS_OK;
    p.total = 3U; p.saved = 1U; p.unchanged = 1U; p.remaining = 1U;
    memcpy(p.display_name, "notes.c", 8U);
    char text[768] = "retained";
    if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiDocumentSaveProgressValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "partition") == 0) {
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        p.remaining = 2U;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "overflow") == 0) {
        p.total = SIZE_MAX; p.saved = SIZE_MAX; p.remaining = 0U;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
        p.unchanged = 0U; p.phase = UMI_DOCUMENT_SAVE_COMPLETE;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "ready-empty") == 0) {
        p.total = p.saved = p.unchanged = p.remaining = 0U;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text, "retained") == 0);
    } else if (strcmp(argv[1], "path-empty") == 0) {
        p.phase = UMI_DOCUMENT_SAVE_NEEDS_PATH; p.remaining = 0U; p.saved = 2U;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "unknown-status") == 0) {
        p.phase = UMI_DOCUMENT_SAVE_FAILED; p.last_status = (UmiStatus)999;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text, "retained") == 0);
        p.last_status = (UmiStatus)-1;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "host-failure") == 0) {
        p.phase = UMI_DOCUMENT_SAVE_FAILED; p.last_status = UMI_STATUS_IO_ERROR;
        p.saved = 2U; p.remaining = 0U;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "2 saved, 0 remaining, 1 already saved") != NULL);
    } else if (strcmp(argv[1], "counts-visible") == 0) {
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "1 already saved separately") != NULL);
        p.phase = UMI_DOCUMENT_SAVE_NEEDS_PATH;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "1 already saved separately") != NULL);
        p.phase = UMI_DOCUMENT_SAVE_FAILED; p.last_status = UMI_STATUS_IO_ERROR;
        CHECK(UmiDocumentSaveProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
        CHECK(strstr(text, "1 already saved") != NULL);
    } else if (strcmp(argv[1], "name") == 0) {
        memset(p.display_name, 'x', sizeof(p.display_name));
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(argv[1], "immutable") == 0) {
        UmiDocumentSaveProgress before; memcpy(&before, &p, sizeof(p));
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        CHECK(memcmp(&p, &before, sizeof(p)) == 0);
    } else if (strcmp(argv[1], "all-phases") == 0) {
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        p.phase = UMI_DOCUMENT_SAVE_NEEDS_PATH;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        p.phase = UMI_DOCUMENT_SAVE_CANCELLED; p.last_status = UMI_STATUS_CANCELLED;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        p.phase = UMI_DOCUMENT_SAVE_FAILED; p.last_status = UMI_STATUS_PERMISSION_DENIED;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
        p.phase = UMI_DOCUMENT_SAVE_COMPLETE; p.last_status = UMI_STATUS_OK;
        p.saved = 2U; p.remaining = 0U;
        CHECK(UmiDocumentSaveProgressValidate(&p) == UMI_STATUS_OK);
    } else return 2;
    return 0;
}
