/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_job_identity.c
 *
 * PURPOSE:
 *   Verify immutable workspace identity and failure-atomic path outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/job.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#define CHANGE_PARENT() _chdir("..")
#else
#include <unistd.h>
#define CHANGE_PARENT() chdir("..")
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); exit(1); } } while (0)
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    const char *cases[] = {"arguments","copy","small-buffer","exact-buffer","same-root",
        "normalized-root","different-root","invalid-root","relative-drift"};
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(name, cases[selected])) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    char root[1024], alternate[2048], output[1024], saved[1024];
    int written = snprintf(root, sizeof root, "%s/workspace", argv[2]);
    CHECK(written > 0 && (size_t)written < sizeof root);
    UmiCodeGuardJob *job = NULL;
    CHECK(UmiCodeGuardJobCreate(!strcmp(name,"relative-drift") ? "workspace" : root,
        NULL, &job) == UMI_STATUS_OK);
    memset(output, 0x5a, sizeof output);
    memcpy(saved, output, sizeof output);
    int matches = 73;
    if (!strcmp(name, "arguments")) {
        CHECK(UmiCodeGuardJobCopyRoot(NULL, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobCopyRoot(job, NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(output, saved, sizeof output));
        CHECK(UmiCodeGuardJobMatchRoot(NULL, root, &matches) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobMatchRoot(job, root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(matches == 73);
    } else if (!strcmp(name, "small-buffer")) {
        CHECK(UmiCodeGuardJobCopyRoot(job, output, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiCodeGuardJobCopyRoot(job, output, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(output, saved, sizeof output));
    } else if (!strcmp(name, "copy") || !strcmp(name, "exact-buffer")) {
        CHECK(UmiCodeGuardJobCopyRoot(job, output, sizeof output) == UMI_STATUS_OK);
        CHECK(umi_path_equal(output, root) && umi_path_is_absolute(output));
        if (!strcmp(name, "copy")) {
            /* Caller changes cannot mutate the job input or later copies. */
            output[0] = '!';
            CHECK(UmiCodeGuardJobCopyRoot(job, output, sizeof output) == UMI_STATUS_OK);
            CHECK(umi_path_equal(output, root));
        }
        size_t required = strlen(output) + 1U;
        memset(output, 0x5a, sizeof output);
        CHECK(UmiCodeGuardJobCopyRoot(job, output, required - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!memcmp(output, saved, sizeof output));
        CHECK(UmiCodeGuardJobCopyRoot(job, output, required) == UMI_STATUS_OK);
        CHECK(umi_path_equal(output, root));
        CHECK(!memcmp(output + required, saved + required, sizeof output - required));
    } else if (!strcmp(name, "invalid-root")) {
        CHECK(UmiCodeGuardJobMatchRoot(job, NULL, &matches) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(matches == 73);
        CHECK(UmiCodeGuardJobMatchRoot(job, "", &matches) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(matches == 73);
        memset(alternate, 'a', sizeof alternate);
        CHECK(UmiCodeGuardJobMatchRoot(job, alternate, &matches) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(matches == 73);
    } else {
        const char *candidate = root;
        if (!strcmp(name, "normalized-root")) {
            written = snprintf(alternate, sizeof alternate, "%s/./sub/../", root);
            CHECK(written > 0 && (size_t)written < sizeof alternate);
            candidate = alternate;
        } else if (!strcmp(name, "different-root")) {
            written = snprintf(alternate, sizeof alternate, "%s/other", argv[2]);
            CHECK(written > 0 && (size_t)written < sizeof alternate);
            candidate = alternate;
        } else if (!strcmp(name, "relative-drift")) {
            CHECK(CHANGE_PARENT() == 0);
            candidate = "workspace";
        }
        CHECK(UmiCodeGuardJobMatchRoot(job, candidate, &matches) == UMI_STATUS_OK);
        CHECK(matches == ((!strcmp(name,"different-root") || !strcmp(name,"relative-drift")) ? 0 : 1));
    }
    CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_OK && job == NULL);
    return 0;
}
