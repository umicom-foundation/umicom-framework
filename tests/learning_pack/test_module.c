/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_module.c
 *
 * PURPOSE:
 *   Verify module identity, checked bytes and independent reading ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "module_fixture.h"
#include "umicom/platform/path.h"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); result = 1; goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1], *root = argv[2];
    int result = 0;
    UmiLearningShelf *shelf = NULL, *candidate = NULL, *second = NULL;
    UmiLearningReader *reader = NULL, *again = NULL;
    UmiVersion host = {0U, 9U, 0U};
    UmiLearningReaderSnapshot snapshot;
    char output[UMI_PATH_CAPACITY] = "unchanged", executable[UMI_PATH_CAPACITY];
    CHECK(ModuleFixture(root) == UMI_STATUS_OK);
    if (strcmp(name, "root") == 0) {
        CHECK(UmiLearningModuleRoot("relative/app.exe", "guide", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "unchanged") == 0);
        CHECK(umi_path_join(root, "studio.exe", executable, sizeof(executable)) == UMI_STATUS_OK);
        CHECK(UmiLearningModuleRoot(executable, "../guide", output, sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningModuleRoot(executable, "guide", output, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        CHECK(UmiLearningModuleRoot(executable, "guide", output, sizeof(output)) == UMI_STATUS_OK);
        char modules[UMI_PATH_CAPACITY], expected[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(root, "learning-modules", modules, sizeof(modules)) == UMI_STATUS_OK);
        CHECK(umi_path_join(modules, "guide", expected, sizeof(expected)) == UMI_STATUS_OK);
        CHECK(strcmp(output, expected) == 0);
        goto cleanup;
    }
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    const char *pack = strcmp(name, "wrong-id") == 0 ? "other.fixture" : "module.fixture";
    const char *product = strcmp(name, "wrong-product") == 0 ? "studio" : "framework";
    const char *entry = strcmp(name, "missing-entry") == 0 ? "absent" : "lesson";
    if (strcmp(name, "tampered") == 0)
        CHECK(UmiRootedFileWrite(root, "lesson.html", "changed", 7U) == UMI_STATUS_OK);
    UmiStatus status = UmiLearningModuleOpen(shelf, root, pack, product, entry, host,
        strcmp(name, "bounded") == 0 ? 1U : 4096U, &candidate, &reader);
    CHECK(UmiLearningShelfCount(shelf) == 0U);
    if (strcmp(name, "wrong-id") == 0 || strcmp(name, "wrong-product") == 0 || strcmp(name, "tampered") == 0) {
        CHECK(status == UMI_STATUS_INVALID_STATE && candidate == NULL && reader == NULL);
        goto cleanup;
    }
    if (strcmp(name, "missing-entry") == 0 || strcmp(name, "bounded") == 0) {
        CHECK(status == (strcmp(name, "bounded") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_NOT_FOUND));
        CHECK(candidate == NULL && reader == NULL);
        goto cleanup;
    }
    CHECK(status == UMI_STATUS_OK && UmiLearningShelfCount(candidate) == 1U);
    if (strcmp(name, "reuse") == 0) {
        /* Reopening an admitted identity does not replace it with a changed manifest. */
        CHECK(UmiRootedFileWrite(root, "learning-pack.json", "invalid", 7U) == UMI_STATUS_OK);
        CHECK(UmiLearningModuleOpen(candidate, root, pack, product, entry, host, 4096U,
            &second, &again) == UMI_STATUS_OK);
        CHECK(UmiLearningShelfCount(candidate) == 1U && UmiLearningShelfCount(second) == 1U);
    } else CHECK(strcmp(name, "lifetime") == 0);
    UmiLearningShelfDestroy(candidate); candidate = NULL;
    UmiLearningShelfDestroy(shelf); shelf = NULL;
    CHECK(UmiLearningReaderCopySnapshot(reader, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.has_document && snapshot.history_count == 1U && strcmp(snapshot.pack.id, pack) == 0);
    size_t size = 0U;
    CHECK(UmiLearningReaderCopyContent(reader, output, sizeof(output), &size) == UMI_STATUS_OK);
    CHECK(size == sizeof(module_text) - 1U && strcmp(output, module_text) == 0);
cleanup:
    UmiLearningReaderDestroy(reader);
    UmiLearningReaderDestroy(again);
    UmiLearningShelfDestroy(shelf);
    UmiLearningShelfDestroy(candidate);
    UmiLearningShelfDestroy(second);
    return result;
}
