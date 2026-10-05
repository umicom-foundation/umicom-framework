/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_process_stream_native.c
 * PURPOSE: Check real native language children without invoking a language server, shell command or network.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/language_runtime/process_stream.h"
#include "umicom/platform/filesystem.h"
#include <stdint.h>
#ifdef _WIN32
#include <wchar.h>
#else
#include <time.h>
#endif

static const char unicodeText[] = "caf\xc3\xa9-\xd9\x85\xd9\x84\xd9\x81-\xe6\x96\x87-\xf0\x9f\x93\x81";

static size_t Collect(UmiLanguageRuntimeProcessStream *stream, unsigned char *bytes, size_t capacity)
{
    size_t used = 0U;
    for (unsigned i = 0U; i < 200U; ++i)
    {
        unsigned char part[257];
        size_t count = 99U;
        UmiStatus status = umi_language_runtime_process_stream_read(stream, part, sizeof(part), 50U, &count);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
        CHECK(count <= capacity - used);
        memcpy(bytes + used, part, count);
        used += count;
        /* Drain stdout after exit too: its last bytes can outlive the child. */
        if (status == UMI_STATUS_NOT_FOUND)
        {
            if (!umi_language_runtime_process_stream_is_running(stream))
                return used;
            /* EOF can arrive just before the process becomes waitable. Yield
             * here so a fast empty pipe does not exhaust the polling budget. */
#ifdef _WIN32
            Sleep(5U);
#else
            struct timespec delay = {0, 5000000L};
            (void)nanosleep(&delay, NULL);
#endif
        }
    }
    CHECK(0);
    return 0U;
}

static void Ready(UmiLanguageRuntimeProcessStream *stream)
{
    char bytes[6];
    size_t used = 0U;
    for (unsigned i = 0U; used < sizeof(bytes) && i < 100U; ++i)
    {
        size_t count = 0U;
        UmiStatus status =
            umi_language_runtime_process_stream_read(stream, bytes + used, sizeof(bytes) - used, 50U, &count);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
        used += count;
    }
    CHECK(used == sizeof(bytes) && memcmp(bytes, "ready\n", sizeof(bytes)) == 0);
}

static void AppendHex(char *expected, size_t capacity, size_t *used, const char *text)
{
    const char digits[] = "0123456789abcdef";
    for (const unsigned char *p = (const unsigned char *)text; *p != 0U; ++p)
    {
        CHECK(*used + 3U < capacity);
        expected[(*used)++] = digits[*p >> 4U];
        expected[(*used)++] = digits[*p & 15U];
    }
    CHECK(*used + 2U < capacity);
    expected[(*used)++] = '\n';
    expected[*used] = '\0';
}

static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY], copied[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(directory, root, unicodeText);
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK);
    const char *arguments[UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 1U] = {"args"};
    UmiLanguageRuntimeProcessStreamConfig config = {argv[2], arguments, 1U, directory, 0};
    UmiLanguageRuntimeProcessStream *stream = NULL;
    unsigned char output[131072];
    char expected[131072];
    size_t expectedSize = 0U;
    expected[0] = '\0';
    char *large = NULL;
    int argsCase = 0;
    if (strcmp(mode, "invalid") == 0)
    {
        stream = (UmiLanguageRuntimeProcessStream *)(uintptr_t)1U;
        CHECK(umi_language_runtime_process_stream_start(NULL, &stream) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(stream == NULL);
        CHECK(umi_language_runtime_process_stream_start(&config, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        config.program = "";
        CHECK(umi_language_runtime_process_stream_start(&config, &stream) == UMI_STATUS_INVALID_ARGUMENT &&
              stream == NULL);
        return 0;
    }
    if (strcmp(mode, "null-argument") == 0 || strcmp(mode, "too-many") == 0)
    {
        arguments[0] = NULL;
        if (strcmp(mode, "too-many") == 0)
            config.argument_count = UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS + 1U;
        CHECK(umi_language_runtime_process_stream_start(&config, &stream) == UMI_STATUS_INVALID_ARGUMENT &&
              stream == NULL);
        return 0;
    }
    if (strcmp(mode, "missing-program") == 0 || strcmp(mode, "missing-directory") == 0)
    {
        FixturePath(copied, root, "missing-path");
        if (strcmp(mode, "missing-program") == 0)
            config.program = copied;
        else
            config.working_directory = copied;
        CHECK(umi_language_runtime_process_stream_start(&config, &stream) == UMI_STATUS_IO_ERROR &&
              stream == NULL);
        return 0;
    }
    if (strcmp(mode, "invalid-utf8") == 0 || strcmp(mode, "command-limit") == 0)
    {
#ifdef _WIN32
        if (strcmp(mode, "invalid-utf8") == 0)
        {
            arguments[1] = "\xc3\x28";
            config.argument_count = 2U;
            CHECK(umi_language_runtime_process_stream_start(&config, &stream) ==
                      UMI_STATUS_INVALID_ARGUMENT &&
                  stream == NULL);
        }
        else
        {
            large = malloc(40001U);
            CHECK(large != NULL);
            memset(large, 'x', 40000U);
            large[40000U] = '\0';
            arguments[1] = large;
            config.argument_count = 2U;
            CHECK(umi_language_runtime_process_stream_start(&config, &stream) ==
                      UMI_STATUS_CAPACITY_EXCEEDED &&
                  stream == NULL);
            free(large);
        }
        return 0;
#else
        return 77;
#endif
    }
    if (strcmp(mode, "empty") == 0)
    {
        arguments[1] = "";
        config.argument_count = 2U;
        argsCase = 1;
    }
    else if (strcmp(mode, "quotes") == 0)
    {
        arguments[1] = "two words";
        arguments[2] = "say \"hello\"";
        arguments[3] = "ends with slash\\";
        arguments[4] = "\\\"quoted\\\"";
        arguments[5] = " \t ";
        arguments[6] = "";
        arguments[7] = "&&$(never-execute);|";
        config.argument_count = 8U;
        argsCase = 1;
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        arguments[1] = unicodeText;
        config.argument_count = 2U;
        argsCase = 1;
    }
    else if (strcmp(mode, "many") == 0)
    {
        config.argument_count = UMI_LANGUAGE_RUNTIME_MAX_ARGUMENTS;
        for (size_t i = 1U; i < config.argument_count; ++i)
            arguments[i] = i % 2U == 0U ? "" : unicodeText;
        argsCase = 1;
    }
    else if (strcmp(mode, "long") == 0)
    {
        large = malloc(12001U);
        CHECK(large != NULL);
        memset(large, 'x', 12000U);
        large[12000U] = '\0';
        arguments[1] = large;
        config.argument_count = 2U;
        argsCase = 1;
    }
    else if (strcmp(mode, "unicode-program") == 0)
    {
#ifdef _WIN32
        FixturePath(copied, directory, "language tool.exe");
#else
        FixturePath(copied, directory, "language tool");
#endif
        CHECK(umi_fs_copy_file(argv[2], copied) == UMI_STATUS_OK);
#ifndef _WIN32
        CHECK(chmod(copied, 0700) == 0);
#endif
        config.program = copied;
        arguments[1] = unicodeText;
        config.argument_count = 2U;
        argsCase = 1;
    }
    else if (strcmp(mode, "cwd") == 0)
    {
        arguments[0] = "cwd";
        for (char *p = directory; *p != '\0'; ++p)
            if (*p == '\\')
                *p = '/';
        AppendHex(expected, sizeof(expected), &expectedSize, directory);
    }
    else if (strcmp(mode, "environment") == 0)
    {
        arguments[0] = "environment";
#ifdef _WIN32
        CHECK(_putenv_s("UMICOM_LANGUAGE_FIXTURE_VALUE", "local test value") == 0);
#else
        CHECK(setenv("UMICOM_LANGUAGE_FIXTURE_VALUE", "local test value", 1) == 0);
#endif
        AppendHex(expected, sizeof(expected), &expectedSize, "local test value");
    }
    else if (strcmp(mode, "stderr-separate") == 0 || strcmp(mode, "stderr-merged") == 0)
    {
        arguments[0] = "streams";
        config.merge_stderr = strcmp(mode, "stderr-merged") == 0;
        const char *text = config.merge_stderr ? "diagnostic\nprotocol\n" : "protocol\n";
        expectedSize = strlen(text);
        memcpy(expected, text, expectedSize + 1U);
    }
    else if (strcmp(mode, "echo") == 0)
        arguments[0] = "echo";
    else if (strcmp(mode, "closed-input") == 0)
        arguments[0] = "closed-input";
    else if (strcmp(mode, "exit-active") == 0)
        arguments[0] = "exit-active";
    else if (strcmp(mode, "timeout") == 0 || strcmp(mode, "stop") == 0 || strcmp(mode, "destroy") == 0 ||
             strcmp(mode, "read-invalid") == 0)
        arguments[0] = "wait";
    else if (strcmp(mode, "eof") != 0)
        return 2;
    if (argsCase)
        for (size_t i = 1U; i < config.argument_count; ++i)
            AppendHex(expected, sizeof(expected), &expectedSize, arguments[i]);
    CHECK(umi_language_runtime_process_stream_start(&config, &stream) == UMI_STATUS_OK && stream != NULL);
    if (strcmp(arguments[0], "wait") == 0 || strcmp(mode, "closed-input") == 0)
    {
        Ready(stream);
        CHECK(umi_language_runtime_process_stream_is_running(stream));
        if (strcmp(mode, "closed-input") == 0)
            CHECK(umi_language_runtime_process_stream_write(stream, "x", 1U) == UMI_STATUS_IO_ERROR);
        if (strcmp(mode, "timeout") == 0)
        {
            size_t count = 9U;
            CHECK(umi_language_runtime_process_stream_read(stream, output, sizeof(output), 10U, &count) ==
                      UMI_STATUS_NOT_FOUND &&
                  count == 0U);
        }
        if (strcmp(mode, "read-invalid") == 0)
        {
            size_t count = 9U;
            CHECK(umi_language_runtime_process_stream_read(stream, NULL, 1U, 0U, &count) ==
                      UMI_STATUS_INVALID_ARGUMENT &&
                  count == 0U);
            CHECK(umi_language_runtime_process_stream_write(stream, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_language_runtime_process_stream_write(stream, NULL, 0U) == UMI_STATUS_OK);
        }
        if (strcmp(mode, "destroy") != 0)
        {
            CHECK(umi_language_runtime_process_stream_stop(stream, 20U) == UMI_STATUS_OK);
            CHECK(!umi_language_runtime_process_stream_is_running(stream));
            CHECK(umi_language_runtime_process_stream_stop(stream, 0U) == UMI_STATUS_OK);
            CHECK(umi_language_runtime_process_stream_write(stream, "x", 1U) == UMI_STATUS_IO_ERROR);
        }
    }
    else
    {
        if (strcmp(mode, "echo") == 0)
        {
            expectedSize = 32768U;
            for (size_t i = 0U; i < expectedSize; ++i)
                expected[i] = (char)(unsigned char)(i % 256U);
            CHECK(umi_language_runtime_process_stream_write(stream, expected, 12345U) == UMI_STATUS_OK);
            CHECK(umi_language_runtime_process_stream_write(stream, expected + 12345U,
                                                            expectedSize - 12345U) == UMI_STATUS_OK);
        }
        size_t size = Collect(stream, output, sizeof(output));
        CHECK(size == expectedSize && memcmp(output, expected, size) == 0);
        size_t count = 9U;
        CHECK(umi_language_runtime_process_stream_read(stream, output, sizeof(output), 0U, &count) ==
                  UMI_STATUS_NOT_FOUND &&
              count == 0U);
    }
    umi_language_runtime_process_stream_destroy(stream);
    free(large);
    return 0;
}
#include "../native_process/utf8_entry.inc"
