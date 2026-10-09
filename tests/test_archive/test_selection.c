/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_selection.c
 * PURPOSE: Exercise portable selection fingerprints with independent expected evidence and invalid
 * inputs. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/selection_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                             \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiCtestJobRequest requests[2] = {0};
    strcpy(requests[0].test_id, "discovery.first");
    strcpy(requests[0].name, "sum | caf\xc3\xa9");
    strcpy(requests[0].build_directory, "/project/build");
    strcpy(requests[0].configuration, "Debug");
    requests[0].enabled = 1;
    requests[0].timeout_ms = 1234;
    requests[1] = requests[0];
    strcpy(requests[1].test_id, "discovery.second");
    strcpy(requests[1].name, "other");
    UmiCtestJobPlanSnapshot plan = {1, 2, true};
    char before[65], after[65];
    CHECK(UmiTestSelectionDigest(&plan, requests, before) == UMI_STATUS_OK);
    if (strcmp(mode, "known") == 0)
        CHECK(strcmp(before, "372941a5f89a8ca518f497356f8e49fe0997abca2b5390b22f44d50cf6199a8a") ==
              0);
    else if (strcmp(mode, "rediscovered") == 0)
    {
        strcpy(requests[0].test_id, "another.discovery.id");
        CHECK(UmiTestSelectionDigest(&plan, requests, after) == UMI_STATUS_OK);
        CHECK(strcmp(before, after) == 0);
    }
    else if (strcmp(mode, "padding") == 0)
    {
        /* Bytes after each string terminator are not part of the logical value. */
        memset(requests[0].name + strlen(requests[0].name) + 1U, 'x',
               sizeof(requests[0].name) - strlen(requests[0].name) - 1U);
        CHECK(UmiTestSelectionDigest(&plan, requests, after) == UMI_STATUS_OK);
        CHECK(strcmp(before, after) == 0);
    }
    else if (strcmp(mode, "order") == 0)
    {
        plan.request_count = 2;
        CHECK(UmiTestSelectionDigest(&plan, requests, before) == UMI_STATUS_OK);
        UmiCtestJobRequest swap = requests[0];
        requests[0] = requests[1];
        requests[1] = swap;
        CHECK(UmiTestSelectionDigest(&plan, requests, after) == UMI_STATUS_OK);
        CHECK(strcmp(before, after) != 0);
    }
    else if (strcmp(mode, "framing") == 0)
    {
        strcpy(requests[0].name, "ab");
        strcpy(requests[0].build_directory, "c");
        CHECK(UmiTestSelectionDigest(&plan, requests, before) == UMI_STATUS_OK);
        strcpy(requests[0].name, "a");
        strcpy(requests[0].build_directory, "bc");
        CHECK(UmiTestSelectionDigest(&plan, requests, after) == UMI_STATUS_OK);
        CHECK(strcmp(before, after) != 0);
    }
    else if (strcmp(mode, "invalid") == 0 || strcmp(mode, "bounds") == 0 ||
             strcmp(mode, "unterminated") == 0)
    {
        memset(after, 'z', sizeof(after));
        char sentinel[65];
        memcpy(sentinel, after, sizeof(after));
        if (strcmp(mode, "invalid") == 0)
        {
            CHECK(UmiTestSelectionDigest(NULL, requests, after) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiTestSelectionDigest(&plan, NULL, after) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiTestSelectionDigest(&plan, requests, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            requests[0].enabled = -1;
        }
        else if (strcmp(mode, "bounds") == 0)
            plan.repeat_count = UINT32_MAX;
        else
            memset(requests[0].configuration, 'x', sizeof(requests[0].configuration));
        CHECK(UmiTestSelectionDigest(&plan, requests, after) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(sentinel, after, sizeof(after)) == 0);
    }
    else
    {
        if (strcmp(mode, "name") == 0)
            strcpy(requests[0].name, "renamed");
        else if (strcmp(mode, "root") == 0)
            strcpy(requests[0].build_directory, "/other/build");
        else if (strcmp(mode, "configuration") == 0)
            strcpy(requests[0].configuration, "Release");
        else if (strcmp(mode, "enabled") == 0)
            requests[0].enabled = 0;
        else if (strcmp(mode, "timeout") == 0)
            ++requests[0].timeout_ms;
        else if (strcmp(mode, "repeat") == 0)
            ++plan.repeat_count;
        else if (strcmp(mode, "stop") == 0)
            plan.stop_on_failure = false;
        else if (strcmp(mode, "count") == 0)
            plan.request_count = 2;
        else
            CHECK(false);
        CHECK(UmiTestSelectionDigest(&plan, requests, after) == UMI_STATUS_OK);
        CHECK(strcmp(before, after) != 0);
    }
    return 0;
}
