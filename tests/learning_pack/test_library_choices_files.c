/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_library_choices_files.c
 *
 * PURPOSE:
 *   Verify explicit settings creation and fresh manifest admission on remembered roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "library_choices_fixture.h"
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); result = 1; goto cleanup; \
} } while (0)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningPack *pack = NULL, *opened = NULL;
    UmiLearningLibraryChoices *choices = NULL, *loaded = NULL;
    UmiRootedFileCreation creation;
    unsigned char random[16], *content = NULL;
    size_t length = 0U;
    char token[33], filename[80];
    CHECK(UmiSecureRandomBytes(random, sizeof(random)) == UMI_STATUS_OK);
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0U; i < sizeof(random); ++i) {
        token[2U*i] = hex[random[i] >> 4U]; token[2U*i+1U] = hex[random[i] & 15U];
    }
    token[32] = '\0';
    snprintf(filename, sizeof(filename), "choices-%s.json", token);
    CHECK(UmiLearningLibraryChoicesCreate(&choices) == UMI_STATUS_OK);
    CHECK(ChoiceFixturePack(root, "framework.learning", "framework", "en-GB", 3U, 1, &pack) == UMI_STATUS_OK);
    CHECK(UmiLearningLibraryChoicesRemember(choices, pack, "learning-pack.json") == UMI_STATUS_OK);
    UmiLearningPackDestroy(pack); pack = NULL;
    if (strcmp(name, "missing-settings") == 0) {
        CHECK(UmiLearningLibraryChoicesLoad(root, filename, &loaded) == UMI_STATUS_NOT_FOUND);
        int exists = 1; CHECK(UmiRootedFileExists(root, filename, &exists) == UMI_STATUS_OK && !exists);
        CHECK(loaded == NULL); goto cleanup;
    }
    if (strcmp(name, "refused-save") == 0) {
        CHECK(UmiLearningLibraryChoicesSaveNew(choices, root, "../outside.json", &creation) != UMI_STATUS_OK);
        CHECK(!creation.created && !creation.complete && creation.bytes_written == 0U);
        goto cleanup;
    }
    CHECK(UmiLearningLibraryChoicesSaveNew(choices, root, filename, &creation) == UMI_STATUS_OK);
    CHECK(creation.created && creation.complete && creation.bytes_written != 0U);
    if (strcmp(name, "existing-save") == 0) {
        CHECK(UmiLearningLibraryChoicesForget(choices, "framework.learning") == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesSaveNew(choices, root, filename, &creation) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(!creation.created && !creation.complete);
        CHECK(UmiLearningLibraryChoicesLoad(root, filename, &loaded) == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesCount(loaded) == 1U);
        goto cleanup;
    }
    CHECK(UmiLearningLibraryChoicesLoad(root, filename, &loaded) == UMI_STATUS_OK);
    UmiLearningLibraryChoicesDestroy(choices); choices = NULL;
    if (strcmp(name, "missing-manifest") == 0) {
        /* Remove only this test's own manifest, never a remembered user folder. */
        CHECK(UmiRootedFileRemove(root, "learning-pack.json") == UMI_STATUS_OK);
        CHECK(UmiLearningLibraryChoicesReopen(loaded, 0U, (UmiVersion){0,9,0}, &opened) == UMI_STATUS_NOT_FOUND);
        CHECK(opened == NULL); goto cleanup;
    }
    if (strcmp(name, "identity") == 0 || strcmp(name, "product") == 0 ||
        strcmp(name, "locale") == 0 || strcmp(name, "revision") == 0 || strcmp(name, "locale-case") == 0) {
        CHECK(ChoiceFixturePack(root,
            strcmp(name, "identity") == 0 ? "another" : "framework.learning",
            strcmp(name, "product") == 0 ? "studio" : "framework",
            strcmp(name, "locale") == 0 ? "fr" : strcmp(name, "locale-case") == 0 ? "EN-gb" : "en-GB",
            strcmp(name, "revision") == 0 ? 4U : 3U, 1, &pack) == UMI_STATUS_OK);
    }
    UmiStatus expected = (strcmp(name, "identity") == 0 || strcmp(name, "product") == 0 ||
        strcmp(name, "locale") == 0 || strcmp(name, "revision") == 0) ? UMI_STATUS_INVALID_STATE :
        strcmp(name, "incompatible") == 0 ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    CHECK(UmiLearningLibraryChoicesReopen(loaded, 0U,
        strcmp(name, "incompatible") == 0 ? (UmiVersion){1,0,0} : (UmiVersion){0,9,0}, &opened) == expected);
    if (expected != UMI_STATUS_OK) { CHECK(opened == NULL); goto cleanup; }
    if (strcmp(name, "changed-resource") == 0)
        CHECK(UmiRootedFileWrite(root, "guide.txt", "changed", 7U) == UMI_STATUS_OK);
    CHECK(UmiLearningPackReadResource(opened, "guide", 1024U, &content, &length) ==
        (strcmp(name, "changed-resource") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
    if (strcmp(name, "changed-resource") == 0) CHECK(content == NULL && length == 0U);
    else CHECK(length == strlen("Read the admitted library resource."));
cleanup:
    UmiLearningPackFreeBytes(content);
    UmiLearningPackDestroy(pack); UmiLearningPackDestroy(opened);
    UmiLearningLibraryChoicesDestroy(choices); UmiLearningLibraryChoicesDestroy(loaded);
    return result;
}
