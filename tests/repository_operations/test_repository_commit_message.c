/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_commit_message.c
 *
 * PURPOSE:
 *   Verify local staged-path classification produces conservative, predictable
 *   commit messages without reading source contents or contacting a service.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/commit_message.h"

#include <stdlib.h>
#include <string.h>

/* Exercise each supported classification and confirm mixed changes never
 * claim to be documentation-only, test-only or build-only. */
int main(void)
{
    UmiRepositoryCommitMessage message;

    if (umi_repository_commit_message_generate(
            "framework", "docs/guide.md\nREADME.md\n", &message) !=
            UMI_STATUS_OK ||
        message.kind != UMI_REPOSITORY_COMMIT_MESSAGE_DOCUMENTATION ||
        message.changed_path_count != 2U ||
        strcmp(message.message,
               "docs(framework): update documentation") != 0) {
        return EXIT_FAILURE;
    }
    if (umi_repository_commit_message_generate(
            "studio", "tests/test_ui.c\ntests/test_workspace.c\n",
            &message) != UMI_STATUS_OK ||
        message.kind != UMI_REPOSITORY_COMMIT_MESSAGE_TEST ||
        strcmp(message.message,
               "test(studio): strengthen test coverage") != 0) {
        return EXIT_FAILURE;
    }
    if (umi_repository_commit_message_generate(
            "framework", "cmake/Platform.cmake\nCMakeLists.txt\n",
            &message) != UMI_STATUS_OK ||
        message.kind != UMI_REPOSITORY_COMMIT_MESSAGE_BUILD ||
        strcmp(message.message,
               "build(framework): update build configuration") != 0) {
        return EXIT_FAILURE;
    }
    if (umi_repository_commit_message_generate(
            "studio", "src/app/ui.c\ntests/test_ui.c\n", &message) !=
            UMI_STATUS_OK ||
        message.kind != UMI_REPOSITORY_COMMIT_MESSAGE_FEATURE ||
        strcmp(message.message,
               "feat(studio): update implementation") != 0) {
        return EXIT_FAILURE;
    }
    if (umi_repository_commit_message_generate(
            "framework", "", &message) != UMI_STATUS_NOT_FOUND) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
