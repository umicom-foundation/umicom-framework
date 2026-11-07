/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_authoring_cli.c
 *
 * PURPOSE:
 *   Exercise the real native learning-pack command against isolated files, without a GUI or compiler launch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "authoring_fixture.h"
int umi_cli_command_learning_pack(int argc, char **argv);
#define CHECK(x) do { if (!(x)) { (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
static int Check(const char *name, char *root)
{
    int result = 0;
    unsigned char *original = NULL, *current = NULL;
    size_t original_size = 0U, current_size = 0U;
    UmiLearningPack *pack = NULL;
    UmiLearningPackInfo info;
    char *verify[] = {"verify", root, "learning-pack.json", "0.9.0"};
    char *prepare[] = {"prepare", root, "learning-pack.json", "candidate.json", "0.9.0"};
    CHECK(AuthoringFixture(root) == UMI_STATUS_OK);
    UmiStatus removed = UmiRootedFileRemove(root, "candidate.json");
    CHECK(removed == UMI_STATUS_OK || removed == UMI_STATUS_NOT_FOUND);
    CHECK(UmiRootedFileRead(root, "learning-pack.json", 4096U, &original, &original_size) == UMI_STATUS_OK);
    if (strcmp(name, "help") == 0) {
        char *help[] = {"--help"};
        CHECK(umi_cli_command_learning_pack(1, help) == 0);
    } else if (strcmp(name, "arguments") == 0) {
        CHECK(umi_cli_command_learning_pack(0, NULL) == 2);
        CHECK(umi_cli_command_learning_pack(3, verify) == 2);
        const char *bad[] = {"", "0.9", "0.9.0.1", "+0.9.0", "0.-9.0", "65536.9.0", "999999999.0.0", "0.9.0tail"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            verify[3] = (char *)bad[i];
            CHECK(umi_cli_command_learning_pack(4, verify) == 2);
        }
    } else if (strcmp(name, "incompatible") == 0) {
        verify[3] = "2.0.0";
        CHECK(umi_cli_command_learning_pack(4, verify) == 1);
    } else if (strcmp(name, "relative-root") == 0) {
        verify[1] = ".";
        CHECK(umi_cli_command_learning_pack(4, verify) == 1);
    } else if (strcmp(name, "missing") == 0) {
        CHECK(UmiRootedFileRemove(root, "image.svg") == UMI_STATUS_OK);
        CHECK(umi_cli_command_learning_pack(4, verify) == 1);
        CHECK(umi_cli_command_learning_pack(5, prepare) == 1);
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "candidate.json", &exists) == UMI_STATUS_OK && !exists);
    } else if (strcmp(name, "verify") == 0) {
        CHECK(umi_cli_command_learning_pack(4, verify) == 0);
    } else if (strcmp(name, "existing-output") == 0) {
        CHECK(UmiRootedFileWrite(root, "candidate.json", "retain", 6U) == UMI_STATUS_OK);
        CHECK(umi_cli_command_learning_pack(5, prepare) == 1);
        CHECK(UmiRootedFileRead(root, "candidate.json", 4096U, &current, &current_size) == UMI_STATUS_OK);
        CHECK(current_size == 6U && memcmp(current, "retain", 6U) == 0);
        UmiRootedFileFree(current); current = NULL;
    } else if (strcmp(name, "same-output") == 0) {
        prepare[3] = "learning-pack.json";
        CHECK(umi_cli_command_learning_pack(5, prepare) == 1);
    } else if (strcmp(name, "traversal-output") == 0) {
        prepare[3] = "../candidate.json";
        CHECK(umi_cli_command_learning_pack(5, prepare) == 1);
    } else {
        CHECK(strcmp(name, "revision") == 0);
        CHECK(UmiRootedFileWrite(root, "lesson.md", "changed", 7U) == UMI_STATUS_OK);
        CHECK(umi_cli_command_learning_pack(4, verify) == 1);
        CHECK(umi_cli_command_learning_pack(5, prepare) == 0);
        UmiVersion host = {0U, 9U, 0U};
        CHECK(UmiLearningPackLoad(root, "candidate.json", host, &pack) == UMI_STATUS_OK);
        CHECK(UmiLearningPackCopyInfo(pack, &info) == UMI_STATUS_OK && info.revision == 3U);
        verify[2] = "candidate.json";
        CHECK(umi_cli_command_learning_pack(4, verify) == 0);
    }
    CHECK(UmiRootedFileRead(root, "learning-pack.json", 4096U, &current, &current_size) == UMI_STATUS_OK);
    CHECK(original_size == current_size && memcmp(original, current, current_size) == 0);
cleanup:
    UmiRootedFileFree(original); UmiRootedFileFree(current); UmiLearningPackDestroy(pack);
    return result;
}
int main(int argc, char **argv)
{
    return argc == 3 ? Check(argv[1], argv[2]) : 2;
}
