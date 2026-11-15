/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_shortcut_reference.c
 *
 * PURPOSE:
 *   Verify bounded reference sizing, UTF-8, escaping, order and failure atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_workbench/shortcut_reference.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto done; } } while (0)
int main(int argc, char **argv)
{
    const char *cases[] = {"empty", "row", "availability", "unicode", "escape", "query", "short",
        "exact", "invalid-pointer", "invalid-utf8", "unterminated", "empty-field", "invalid-state",
        "bounds", "large", "order", "late-invalid"};
    if (argc != 2) return 2;
    const char *mode = argv[1]; int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(mode, cases[i])) known = 1;
    if (!known) return 2;
    UmiShortcutReferenceEntry rows[2] = {
        {"file.save", "Save", "Ctrl+S", 1}, {"file.close", "Close", "Ctrl+W", 0}};
    UmiShortcutReferenceEntry *large = NULL;
    char *text = NULL; size_t required = 91U, capacity = 0U;
    char sentinel[8] = "retain"; char before[8]; memcpy(before, sentinel, sizeof before);
    if (!strcmp(mode, "empty")) {
        CHECK(UmiShortcutReferenceFormat(NULL, 0U, NULL, 0U, &required) == UMI_STATUS_OK);
        text = malloc(required + 1U); CHECK(text != NULL);
        CHECK(UmiShortcutReferenceFormat(NULL, 0U, text, required + 1U, &required) == UMI_STATUS_OK);
        CHECK(strstr(text, "No keyboard bindings were supplied.") != NULL); goto done;
    }
    if (!strcmp(mode, "invalid-pointer")) {
        CHECK(UmiShortcutReferenceFormat(NULL, 1U, sentinel, sizeof sentinel, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiShortcutReferenceFormat(rows, 1U, NULL, 1U, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiShortcutReferenceFormat(rows, 1U, sentinel, sizeof sentinel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 91U && !memcmp(sentinel, before, sizeof before)); goto done;
    }
    if (!strcmp(mode, "bounds")) {
        CHECK(UmiShortcutReferenceFormat(rows, UMI_SHORTCUT_REFERENCE_LIMIT + 1U,
            sentinel, sizeof sentinel, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(required == 91U && !memcmp(sentinel, before, sizeof before)); goto done;
    }
    if (!strcmp(mode, "invalid-utf8")) { rows[0].caption[0] = (char)0xff; rows[0].caption[1] = '\0'; }
    if (!strcmp(mode, "unterminated")) memset(rows[0].command, 'x', sizeof rows[0].command);
    if (!strcmp(mode, "empty-field")) rows[0].shortcut[0] = '\0';
    if (!strcmp(mode, "invalid-state")) rows[0].available = 2;
    if (!strcmp(mode, "late-invalid")) rows[1].caption[0] = '\0';
    if (!strcmp(mode, "invalid-utf8") || !strcmp(mode, "unterminated") ||
        !strcmp(mode, "empty-field") || !strcmp(mode, "invalid-state") || !strcmp(mode, "late-invalid")) {
        CHECK(UmiShortcutReferenceFormat(rows, 2U, sentinel, sizeof sentinel, &required) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(required == 91U && !memcmp(sentinel, before, sizeof before)); goto done;
    }
    if (!strcmp(mode, "order")) { UmiShortcutReferenceEntry swap = rows[0]; rows[0] = rows[1]; rows[1] = swap; }
    if (!strcmp(mode, "row")) { rows[1] = rows[0]; strcpy(rows[1].shortcut, "Alt+S"); }
    if (!strcmp(mode, "unicode")) strcpy(rows[0].caption, "Enregistrer \xc3\xa9");
    if (!strcmp(mode, "escape")) strcpy(rows[0].caption, "Save\n\t\\\x7f");
    size_t count = 2U;
    const UmiShortcutReferenceEntry *source = rows;
    if (!strcmp(mode, "large")) {
        count = UMI_SHORTCUT_REFERENCE_LIMIT;
        large = calloc(count, sizeof *large); CHECK(large != NULL);
        for (size_t i = 0U; i < count; ++i) {
            memset(large[i].command, 1, sizeof large[i].command - 1U);
            memset(large[i].caption, 2, sizeof large[i].caption - 1U);
            memset(large[i].shortcut, 3, sizeof large[i].shortcut - 1U);
            large[i].available = 1;
        }
        source = large;
    }
    CHECK(UmiShortcutReferenceFormat(source, count, NULL, 0U, &required) == UMI_STATUS_OK);
    CHECK(required > 0U && required < 1024U * 1024U);
    if (!strcmp(mode, "query")) goto done;
    if (!strcmp(mode, "short")) {
        size_t query = required; required = 91U;
        CHECK(UmiShortcutReferenceFormat(source, count, sentinel, sizeof sentinel, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(required == query && !memcmp(sentinel, before, sizeof before)); goto done;
    }
    capacity = required + 1U;
    text = malloc(capacity + 1U); CHECK(text != NULL); memset(text, 'Q', capacity + 1U);
    if (!strcmp(mode, "exact")) {
        CHECK(UmiShortcutReferenceFormat(source, count, text, capacity - 1U, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (size_t i = 0U; i <= capacity; ++i) CHECK(text[i] == 'Q');
    }
    CHECK(UmiShortcutReferenceFormat(source, count, text, capacity, &required) == UMI_STATUS_OK);
    CHECK(strlen(text) == required && text[capacity] == 'Q');
    if (!strcmp(mode, "unicode")) CHECK(strstr(text, "Enregistrer \xc3\xa9") != NULL);
    else if (!strcmp(mode, "escape")) CHECK(strstr(text, "Save\\x0A\\x09\\x5C\\x7F") != NULL);
    else if (!strcmp(mode, "availability")) CHECK(strstr(text, "Save [available]") && strstr(text, "Close [unavailable]"));
    else if (strcmp(mode, "large") != 0) {
        CHECK(strstr(text, "Ctrl+S  Save") != NULL && strstr(text, "file.save") != NULL);
        if (!strcmp(mode, "order")) CHECK(strstr(text, "file.close") < strstr(text, "file.save"));
        else if (!strcmp(mode, "row")) CHECK(strstr(text, "Alt+S  Save") != NULL);
        else CHECK(strstr(text, "file.save") < strstr(text, "file.close"));
    }
done:
    free(text); free(large); return failed;
}
