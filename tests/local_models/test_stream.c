/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_stream.c
 *
 * PURPOSE:
 *   Verify chunk bounds, cancellation, file limits and final-leaf policy using isolated native fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/input_stream.h"
#include "../build_log/fixture.h"
typedef struct ReadState { uint64_t received; size_t calls; bool cancel; } ReadState;
static UmiStatus Observe(const unsigned char *bytes, size_t size, void *context)
{
    ReadState *state = context;
    CHECK(size > 0U && size <= 65536U);
    for (size_t i = 0U; i < size; ++i)
        CHECK(bytes[i] == (unsigned char)((state->received + i) % 251U));
    ++state->calls;
    state->received += size;
    return state->cancel ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "stream.bin");
    ReadState state = {0}; uint64_t observed = 99U;
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiInputFileVisit("relative", 10U, Observe, &state, &observed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(observed == 0U);
        CHECK(UmiInputFileVisit(path, 10U, NULL, &state, &observed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiInputFileVisit(path, 10U, Observe, &state, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiInputFileVisit(root, 10U, Observe, &state, &observed) != UMI_STATUS_OK);
        CHECK(UmiInputFileVisit(path, 10U, Observe, &state, &observed) == UMI_STATUS_NOT_FOUND);
        CHECK(state.calls == 0U); return 0;
    }
    if (strcmp(name, "fifo") == 0) {
#ifdef _WIN32
        (void)fprintf(stderr, "SKIP: POSIX FIFO fixture is not a Windows named pipe.\n"); return 77;
#else
        CHECK(mkfifo(path, 0600) == 0);
        CHECK(UmiInputFileVisit(path, 10U, Observe, &state, &observed) != UMI_STATUS_OK);
        CHECK(state.calls == 0U); return 0;
#endif
    }
    size_t length = strcmp(name, "empty") == 0 ? 0U : 196613U;
    unsigned char *bytes = malloc(length + 1U); CHECK(bytes != NULL);
    for (size_t i = 0U; i < length; ++i) bytes[i] = (unsigned char)(i % 251U);
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, bytes, length) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file); free(bytes);
    if (strcmp(name, "symlink") == 0) {
        char alias[UMI_PATH_CAPACITY]; FixturePath(alias, root, "alias.bin");
#ifdef _WIN32
        if (!FixtureSymbolicLink(alias, path, 0)) return 77;
#else
        CHECK(symlink(path, alias) == 0);
#endif
        CHECK(UmiInputFileVisit(alias, length, Observe, &state, &observed) != UMI_STATUS_OK);
        CHECK(state.calls == 0U); return 0;
    }
    if (strcmp(name, "limit") == 0) {
        CHECK(UmiInputFileVisit(path, length - 1U, Observe, &state, &observed) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(observed == 0U && state.calls == 0U); return 0;
    }
    if (strcmp(name, "cancel") == 0) {
        state.cancel = true;
        CHECK(UmiInputFileVisit(path, length, Observe, &state, &observed) == UMI_STATUS_CANCELLED);
        CHECK(observed == 0U && state.calls == 1U); return 0;
    }
    if (strcmp(name, "chunks") == 0 || strcmp(name, "empty") == 0) {
        CHECK(UmiInputFileVisit(path, length, Observe, &state, &observed) == UMI_STATUS_OK);
        CHECK(observed == length && state.received == length);
        CHECK(state.calls == (length == 0U ? 0U : 4U)); return 0;
    }
    return 2;
}
