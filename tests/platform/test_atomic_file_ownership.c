/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_atomic_file_ownership.c
 * PURPOSE: Verify UTF-8 atomic replacement and ownership of temporary files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../support/assertions.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define TEST_PID ((unsigned long)_getpid())
#else
#include <unistd.h>
#define TEST_PID ((unsigned long)getpid())
#endif

typedef struct Writer {
    char path[UMI_PATH_CAPACITY];
    char payload[4096];
} Writer;
static int WriteRepeatedly(void *data)
{
    Writer *writer = data;
    for (unsigned i = 0U; i < 16U; ++i)
        if (umi_atomic_file_write(writer->path, writer->payload, sizeof(writer->payload)) != UMI_STATUS_OK)
            return 1;
    return 0;
}
static void CheckBytes(const char *path, const void *expected, size_t count)
{
    unsigned char *bytes = NULL;
    size_t length = 0U;
    assert(umi_fs_read_bytes(path, &bytes, &length) == UMI_STATUS_OK);
    assert(length == count && (count == 0U || memcmp(bytes, expected, count) == 0));
    umi_fs_free_bytes(bytes);
}
int main(int argc, char **argv)
{
    assert(argc == 2);
    const char *mode = argv[1];
    char temporary_root[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], name[128];
    char destination[UMI_PATH_CAPACITY], collision[UMI_PATH_CAPACITY];
    assert(umi_fs_temp_directory(temporary_root, sizeof(temporary_root)) == UMI_STATUS_OK);
    int written = snprintf(name, sizeof(name), "umicom-atomic-%s-%lu", mode, TEST_PID);
    assert(written > 0 && (size_t)written < sizeof(name));
    assert(umi_path_join(temporary_root, name, root, sizeof(root)) == UMI_STATUS_OK);
    assert(!umi_fs_exists(root));
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_path_join(root, "caf\xc3\xa9-\xf0\x9f\x8c\x90.bin", destination, sizeof(destination)) == UMI_STATUS_OK);
    fprintf(stdout, "Atomic fixture: %s\n", root);
    if (strcmp(mode, "collision") == 0 || strcmp(mode, "collision-limit") == 0) {
        unsigned count = strcmp(mode, "collision-limit") == 0 ? 64U : 1U;
        assert(umi_fs_write_text(destination, "original") == UMI_STATUS_OK);
        for (unsigned i = 1U; i <= count; ++i) {
            written = snprintf(collision, sizeof(collision), "%s.umi-tmp-%lu-%u", destination, TEST_PID, i);
            assert(written > 0 && (size_t)written < sizeof(collision));
            assert(umi_fs_write_text(collision, "keep existing") == UMI_STATUS_OK);
        }
        UmiStatus expected = count == 64U ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_OK;
        assert(umi_atomic_file_write_text(destination, "replacement") == expected);
        CheckBytes(destination, count == 64U ? "original" : "replacement", count == 64U ? 8U : 11U);
        for (unsigned i = 1U; i <= count; ++i) {
            written = snprintf(collision, sizeof(collision), "%s.umi-tmp-%lu-%u", destination, TEST_PID, i);
            assert(written > 0 && (size_t)written < sizeof(collision));
            CheckBytes(collision, "keep existing", 13U);
        }
    } else if (strcmp(mode, "concurrent") == 0) {
        Writer writers[4] = {0};
        UmiThread *threads[4] = {0};
        for (size_t i = 0U; i < 4U; ++i) {
            written = snprintf(writers[i].path, sizeof(writers[i].path), "%s.worker-%zu", destination, i);
            assert(written > 0 && (size_t)written < sizeof(writers[i].path));
            memset(writers[i].payload, (int)('A' + (int)i), sizeof(writers[i].payload));
            assert(umi_thread_start(WriteRepeatedly, &writers[i], &threads[i]) == UMI_STATUS_OK);
        }
        for (size_t i = 0U; i < 4U; ++i) {
            int code = -1;
            assert(umi_thread_join(threads[i], &code) == UMI_STATUS_OK && code == 0);
            umi_thread_destroy(threads[i]);
            CheckBytes(writers[i].path, writers[i].payload, sizeof(writers[i].payload));
        }
    } else if (strcmp(mode, "directory") == 0) {
        assert(umi_fs_make_directories(destination) == UMI_STATUS_OK);
        assert(umi_atomic_file_write_text(destination, "must not replace directory") != UMI_STATUS_OK);
        assert(umi_fs_is_directory(destination));
    } else if (strcmp(mode, "invalid") == 0) {
        assert(umi_fs_write_text(destination, "original") == UMI_STATUS_OK);
        assert(umi_atomic_file_write(destination, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CheckBytes(destination, "original", 8U);
    } else if (strcmp(mode, "empty") == 0) {
        assert(umi_atomic_file_write_text(destination, "previous") == UMI_STATUS_OK);
        assert(umi_atomic_file_write(destination, NULL, 0U) == UMI_STATUS_OK);
        CheckBytes(destination, NULL, 0U);
    } else {
        assert(strcmp(mode, "unicode") == 0);
        assert(umi_atomic_file_write_text(destination, "previous") == UMI_STATUS_OK);
        const unsigned char bytes[] = {0U, 0xffU, 'x', '\n'};
        assert(umi_atomic_file_write(destination, bytes, sizeof(bytes)) == UMI_STATUS_OK);
        CheckBytes(destination, bytes, sizeof(bytes));
    }
    /* This process created the verified, exclusive fixture root above. */
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
