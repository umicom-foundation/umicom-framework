/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/test_channel.c
 * PURPOSE: Verify inherited and restricted environments, explicit stdin closure and closed host descriptors.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#define _POSIX_C_SOURCE 200809L
#include "fixture.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
static void SetFixtureEnvironment(void)
{
#ifdef _WIN32
    (void)SetEnvironmentVariableW(L"UMICOM_CONSOLE_FIXTURE_VALUE", L"parent value");
#else
    (void)setenv("UMICOM_CONSOLE_FIXTURE_VALUE", "parent value", 1);
#endif
}
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    const char *known[] = {"inherited",    "restricted",   "override", "duplicate",
                           "invalid-name", "invalid-utf8", "eof",      "closed-stdio"};
    bool valid = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (strcmp(mode, known[i]) == 0)
            valid = true;
    if (!valid)
        return 2;
    int failed = 0;
    UmiProcessChannel *channel = NULL;
    char directory[UMI_PATH_CAPACITY], output[8192] = {0};
    CHECK(umi_path_parent(argv[2], directory, sizeof directory) == UMI_STATUS_OK);
    const char *producer = strcmp(mode, "eof") == 0 ? "echo" : "environment";
    UmiProcessChannelRequest request = {argv[2], &producer, 1U, directory};
    UmiEnvironmentVariable environment[] = {{"UMICOM_CONSOLE_FIXTURE_VALUE", "override \xc3\xa9"},
                                            {"umicom_console_fixture_value", "ambiguous"}};
    SetFixtureEnvironment();
    if (strcmp(mode, "duplicate") == 0)
    {
        CHECK(UmiProcessChannelOpenProgram(&request, environment, 2U, &channel) ==
                  UMI_STATUS_ALREADY_EXISTS &&
              channel == NULL);
        goto cleanup;
    }
    if (strcmp(mode, "invalid-name") == 0)
    {
        environment[0].name = "bad=name";
        CHECK(UmiProcessChannelOpenProgram(&request, environment, 1U, &channel) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              channel == NULL);
        goto cleanup;
    }
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        environment[0].value = "\xc0\xaf";
        CHECK(UmiProcessChannelOpenProgram(&request, environment, 1U, &channel) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              channel == NULL);
        goto cleanup;
    }
    if (strcmp(mode, "closed-stdio") == 0)
    {
#ifdef _WIN32
        /* Windows standard handle isolation is exercised by normal GUI launch;
         * this case specifically covers Linux descriptors reused below three. */
        failed = 77;
        goto cleanup;
#else
        (void)close(STDIN_FILENO);
        (void)close(STDOUT_FILENO);
        (void)close(STDERR_FILENO);
#endif
    }
    UmiStatus status = strcmp(mode, "restricted") == 0
                           ? UmiProcessChannelOpen(&request, &channel)
                           : UmiProcessChannelOpenProgram(
                                 &request, strcmp(mode, "override") == 0 ? environment : NULL,
                                 strcmp(mode, "override") == 0 ? 1U : 0U, &channel);
    CHECK(status == UMI_STATUS_OK && channel != NULL);
    if (strcmp(mode, "eof") == 0)
        CHECK(UmiProcessChannelWrite(channel, "hello\n", 6U, 1000U) == UMI_STATUS_OK);
    CHECK(UmiProcessChannelCloseInput(channel) == UMI_STATUS_OK);
    CHECK(UmiProcessChannelCloseInput(channel) == UMI_STATUS_OK);
    CHECK(UmiProcessChannelWrite(channel, "x", 1U, 10U) == UMI_STATUS_INVALID_STATE);
    size_t used = 0U;
    bool ended = false;
    for (unsigned i = 0U; i < 2000U; ++i)
    {
        char bytes[512];
        size_t count = 0U;
        status = UmiProcessChannelRead(channel, bytes, sizeof bytes, &count, 5U);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_TIMEOUT);
        CHECK(count < sizeof output - used);
        memcpy(output + used, bytes, count);
        used += count;
        output[used] = '\0';
        UmiProcessChannelSnapshot snapshot;
        CHECK(UmiProcessChannelPoll(channel, &snapshot) == UMI_STATUS_OK);
        if (!snapshot.running && count == 0U)
        {
            CHECK(snapshot.exitCode == 0);
            ended = true;
            break;
        }
        if (status == UMI_STATUS_OK && count == 0U)
            umi_thread_sleep_ms(5U);
    }
    CHECK(ended);
    if (strcmp(mode, "eof") == 0)
        CHECK(strstr(output, "hello\n") != NULL && strstr(output, "EOF") != NULL);
    else if (strcmp(mode, "restricted") == 0)
        CHECK(strstr(output, "value=(absent)") != NULL);
    else if (strcmp(mode, "override") == 0)
        CHECK(strstr(output, "value=override \xc3\xa9") != NULL);
    else
        CHECK(strstr(output, "value=parent value") != NULL);
cleanup:
    UmiProcessChannelDestroy(channel);
    return failed;
}
