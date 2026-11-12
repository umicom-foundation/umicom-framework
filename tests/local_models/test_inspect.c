/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_inspect.c
 *
 * PURPOSE:
 *   Check streaming GGUF identity, Unicode paths and rejection of malformed or mismatched files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include "umicom/base/sha256.h"
#include "../build_log/fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, "caf\xc3\xa9 model.gguf");
    size_t length = strcmp(name, "multichunk") == 0 ? 131077U : 128U;
    unsigned char *bytes = calloc(length, 1U); CHECK(bytes != NULL);
    memcpy(bytes, "GGUF", 4U); bytes[4] = 3U; bytes[8] = 1U;
    if (strcmp(name, "bad-magic") == 0) bytes[0] = 'X';
    if (strcmp(name, "bad-version") == 0) bytes[4] = 99U;
    if (strcmp(name, "bad-count") == 0) memset(bytes + 8, 255, 8U);
    if (strcmp(name, "truncated") == 0) length = 23U;
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, length) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file); file = NULL;
    char digest[65]; CHECK(UmiSha256Buffer(bytes, length, digest) == UMI_STATUS_OK);
    free(bytes);
    UmiModelInspection inspected;
    if (strcmp(name, "valid") == 0 || strcmp(name, "multichunk") == 0) {
        CHECK(UmiModelInspect(path, digest, NULL, &inspected) == UMI_STATUS_OK);
        CHECK(inspected.bytes == length && inspected.gguf_version == 3U && inspected.tensors == 1U);
        CHECK(strcmp(inspected.sha256, digest) == 0);
        CHECK(UmiModelInspect(path, inspected.sha256, NULL, &inspected) == UMI_STATUS_OK);
        CHECK(strcmp(inspected.sha256, digest) == 0);
        /* Synthetic header fixture only: this does not represent loadable weights. */
        return 0;
    }
    if (strcmp(name, "wrong-hash") == 0) {
        digest[0] = digest[0] == 'a' ? 'b' : 'a';
        CHECK(UmiModelInspect(path, digest, NULL, &inspected) == UMI_STATUS_INVALID_STATE);
        CHECK(inspected.bytes == 0U && inspected.sha256[0] == '\0'); return 0;
    }
    if (strcmp(name, "cancelled") == 0) {
        UmiCancellationToken *cancel = NULL;
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        CHECK(UmiModelInspect(path, digest, cancel, &inspected) == UMI_STATUS_CANCELLED);
        umi_cancellation_token_destroy(cancel); return 0;
    }
    if (strcmp(name, "no-overwrite") == 0) {
        CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiModelInspect(path, digest, NULL, &inspected) == UMI_STATUS_OK); return 0;
    }
    if (strcmp(name, "bad-magic") == 0 || strcmp(name, "bad-version") == 0 ||
        strcmp(name, "bad-count") == 0 || strcmp(name, "truncated") == 0) {
        CHECK(UmiModelInspect(path, digest, NULL, &inspected) == UMI_STATUS_PARSE_ERROR);
        CHECK(inspected.bytes == 0U); return 0;
    }
    return 2;
}
