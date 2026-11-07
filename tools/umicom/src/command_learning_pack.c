/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_learning_pack.c
 *
 * PURPOSE:
 *   Verify local lesson files or prepare a new learning-pack manifest through Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"
#include "umicom/teacher/learning_pack_authoring.h"
#include "umicom/platform/rooted_files.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static void Help(void)
{
    (void)puts("umicom learning-pack verify ROOT MANIFEST HOST_VERSION\n"
        "umicom learning-pack prepare ROOT MANIFEST NEW_MANIFEST HOST_VERSION\n"
        "ROOT is an absolute directory; manifest names are relative to it.\n"
        "HOST_VERSION is the reader application's major.minor.patch version.\n"
        "verify checks every declared file (up to 1 MiB each); it writes nothing.\n"
        "prepare increments the revision and creates NEW_MANIFEST exclusively.\n"
        "Review content before prepare. Hashes detect changes, not authorship.\n"
        "No lesson is executed, downloaded or installed by either command.");
}
static int HostVersion(const char *text, UmiVersion *out)
{
    uint16_t parts[3];
    for (size_t part = 0U; part < 3U; ++part) {
        unsigned int value = 0U, digits = 0U;
        while (*text >= '0' && *text <= '9') {
            if (++digits > 5U) return 0;
            value = value * 10U + (unsigned int)(*text - '0');
            if (value > UINT16_MAX) return 0;
            ++text;
        }
        if (digits == 0U) return 0;
        parts[part] = (uint16_t)value;
        if (part < 2U) { if (*text != '.') return 0; ++text; }
    }
    if (*text != '\0') return 0;
    *out = (UmiVersion){parts[0], parts[1], parts[2]};
    return 1;
}
int umi_cli_command_learning_pack(int argc, char **argv)
{
    if (argc == 1 && strcmp(argv[0], "--help") == 0) { Help(); return 0; }
    int prepare = argc == 5 && strcmp(argv[0], "prepare") == 0;
    int verify = argc == 4 && strcmp(argv[0], "verify") == 0;
    if (!prepare && !verify) { Help(); return 2; }
    UmiVersion host;
    if (!HostVersion(argv[argc - 1], &host)) {
        (void)fputs("HOST_VERSION must contain three integers in 0..65535.\n", stderr);
        return 2;
    }
    UmiLearningPack *pack = NULL;
    UmiStatus status = UmiLearningPackLoad(argv[1], argv[2], host, &pack);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Cannot admit learning pack: %s\n", umi_status_text(status));
        return 1;
    }
    UmiLearningPackInfo info;
    status = UmiLearningPackCopyInfo(pack, &info);
    if (status == UMI_STATUS_OK && verify) {
        /* Verify the exact bytes the Studio reader would receive, including
         * binary examples. No content or credentials are printed. */
        for (size_t i = 0U; i < info.resource_count && status == UMI_STATUS_OK; ++i) {
            UmiLearningPackResource item;
            unsigned char *bytes = NULL;
            size_t length = 0U;
            status = UmiLearningPackResourceAt(pack, i, &item);
            if (status == UMI_STATUS_OK) {
                status = UmiLearningPackReadResource(pack, item.id,
                    UMI_LEARNING_AUTHORING_RESOURCE_BYTES, &bytes, &length);
                if (status != UMI_STATUS_OK)
                    (void)fprintf(stderr, "Resource %s: %s\n", item.id, umi_status_text(status));
            }
            UmiLearningPackFreeBytes(bytes);
        }
        if (status == UMI_STATUS_OK)
            (void)printf("Verified %zu resources; pack %s revision %" PRIu64 ".\n",
                info.resource_count, info.id, info.revision);
    } else if (status == UMI_STATUS_OK) {
        unsigned char *json = NULL;
        size_t length = 0U;
        if (info.revision >= (uint64_t)INT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else status = UmiLearningPackPrepareRevision(pack, info.revision + 1U,
            UMI_LEARNING_AUTHORING_RESOURCE_BYTES, &json, &length);
        if (status == UMI_STATUS_OK) {
            UmiRootedFileCreation creation;
            /* Publication stays explicit and exclusive. In particular an
             * existing manifest is never replaced as a side effect of review. */
            status = UmiRootedFileCreate(argv[1], argv[3], json, length, &creation);
            if (status != UMI_STATUS_OK && creation.created)
                (void)fprintf(stderr, "Incomplete new manifest retained (%zu bytes). "
                    "Inspect it before choosing a different output name.\n", creation.bytes_written);
            if (status == UMI_STATUS_OK)
                (void)printf("Prepared %s revision %" PRIu64 " with %zu resources. "
                    "Review the new manifest before installing it.\n",
                    info.id, info.revision + 1U, info.resource_count);
        }
        UmiLearningPackFreeBytes(json);
    }
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK)
        (void)fprintf(stderr, "Learning-pack operation failed: %s\n", umi_status_text(status));
    if (ferror(stdout) != 0 || ferror(stderr) != 0) return 1;
    return status == UMI_STATUS_OK ? 0 : 1;
}
