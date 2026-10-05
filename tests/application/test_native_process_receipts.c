/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_native_process_receipts.c
 * PURPOSE: Exercise native launch evidence with a copied inert fixture, never a product.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/application_processes.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <gio/gio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #expression);                                              \
            result = 1;                                                                                      \
            goto done;                                                                                       \
        }                                                                                                    \
    } while (0)
typedef struct Observation
{
    UmiGtk4ApplicationProcesses *owner;
    size_t calls;
    bool release_on_start;
    bool exited;
    bool check_reentry;
    UmiStatus reentry;
    size_t exits;
} Observation;
static void observe(const UmiApplicationLaunchReceipt *receipt, void *data)
{
    Observation *state = data;
    ++state->calls;
    state->exited = receipt->state == UMI_APPLICATION_LAUNCH_RECEIPT_EXITED;
    if (state->exited)
        ++state->exits;
    if (state->check_reentry)
    {
        uint64_t nested = 0U;
        state->reentry = UmiGtk4ApplicationProcessesStart(state->owner, receipt->application_id,
                                                          receipt->executable_path, true, &nested);
    }
    if (state->release_on_start && receipt->state == UMI_APPLICATION_LAUNCH_RECEIPT_RUNNING)
    {
        UmiGtk4ApplicationProcessesDestroy(state->owner);
        state->owner = NULL;
    }
}
int main(int argc, char **argv)
{
    Observation state = {0};
    UmiApplicationLaunchReceipt receipt;
    char *directory = NULL, *path = NULL, *marker = NULL, *failure = NULL, *finished = NULL;
    char *outer_directory = NULL;
    GFile *source = NULL, *target = NULL;
    GError *error = NULL;
    uint64_t id = 0U, second = 0U;
    int result = 0;
    const char *test = argc > 1 ? argv[1] : "";
    CHECK(argc == 3);
    CHECK(UmiGtk4ApplicationProcessesCreate(&state.owner) == UMI_STATUS_OK);
    CHECK(UmiGtk4ApplicationProcessesObserve(state.owner, observe, &state) == UMI_STATUS_OK);
    if (strcmp(test, "relative") == 0 || strcmp(test, "unknown") == 0 || strcmp(test, "empty") == 0)
    {
        CHECK(UmiGtk4ApplicationProcessesStart(
                  state.owner, strcmp(test, "unknown") == 0 ? "org.other.tool" : "org.umicom.media-studio",
                  strcmp(test, "empty") == 0 ? "" : "umicom-media-studio", false,
                  &id) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(id == 0U && state.calls == 0U);
        goto done;
    }
    directory = g_dir_make_tmp("umicom-launch-receipt-XXXXXX", &error);
    CHECK(directory != NULL);
    if (strcmp(test, "unicode-path") == 0)
    {
        outer_directory = directory;
        directory = g_build_filename(outer_directory, "caf\xc3\xa9 space", NULL);
        CHECK(g_mkdir(directory, 0700) == 0);
    }

#ifdef G_OS_WIN32
    path = g_build_filename(directory, "umicom-media-studio.exe", NULL);
#else
    path = g_build_filename(directory, "umicom-media-studio", NULL);
#endif
    marker = g_build_filename(directory, "receipt-fixture", NULL);
    failure = g_build_filename(directory, "receipt-fail", NULL);
    finished = g_build_filename(directory, "receipt-finished", NULL);
    if (strcmp(test, "missing") == 0)
    {
        CHECK(UmiGtk4ApplicationProcessesStart(state.owner, "org.umicom.media-studio", path, false, &id) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(id == 0U && state.calls == 0U);
        goto done;
    }
    if (strcmp(test, "wrong-name") == 0)
    {
        CHECK(UmiGtk4ApplicationProcessesStart(state.owner, "org.umicom.bank", path, false, &id) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(id == 0U && state.calls == 0U);
        goto done;
    }
    CHECK(g_file_set_contents(marker, "fixture", -1, &error));
    if (strcmp(test, "spawn-failure") == 0)
    {
        CHECK(g_file_set_contents(path, "not an executable", -1, &error));
        CHECK(g_chmod(path, 0700) == 0);
        CHECK(UmiGtk4ApplicationProcessesStart(state.owner, "org.umicom.media-studio", path, false, &id) !=
              UMI_STATUS_OK);
        CHECK(id != 0U && state.calls == 1U);
        CHECK(UmiGtk4ApplicationProcessesLatest(state.owner, "org.umicom.media-studio", &receipt) ==
              UMI_STATUS_OK);
        CHECK(receipt.state == UMI_APPLICATION_LAUNCH_RECEIPT_REJECTED);
        goto done;
    }
    source = g_file_new_for_path(argv[2]);
    target = g_file_new_for_path(path);
    CHECK(g_file_copy(source, target, G_FILE_COPY_NONE, NULL, NULL, NULL, &error));
    CHECK(g_chmod(path, 0700) == 0);
    if (strcmp(test, "exit-failure") == 0)
        CHECK(g_file_set_contents(failure, "failure", -1, &error));
    state.release_on_start = strcmp(test, "destroy-observer") == 0;
    state.check_reentry = strcmp(test, "reentry") == 0;
    CHECK(UmiGtk4ApplicationProcessesStart(state.owner, "org.umicom.media-studio", path, false, &id) ==
          UMI_STATUS_OK);
    CHECK(id != 0U && state.calls == 1U);
    if (strcmp(test, "new-window") == 0)
    {
        CHECK(UmiGtk4ApplicationProcessesStart(state.owner, "org.umicom.media-studio", path, true, &second) ==
              UMI_STATUS_OK);
        CHECK(second > id && state.calls == 2U);
    }

    if (strcmp(test, "duplicate") == 0)
    {
        CHECK(UmiGtk4ApplicationProcessesStart(state.owner, "org.umicom.media-studio", path, false,
                                               &second) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(second == 0U);
    }
    if (strcmp(test, "destroy-active") == 0)
    {
        UmiGtk4ApplicationProcessesDestroy(state.owner);
        state.owner = NULL;
    }
    /* Drain the owner context for a bounded period; observer destruction must
     * prevent subsequent callbacks while allowing the child to finish. */
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (g_get_monotonic_time() < deadline)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        if (state.owner != NULL && state.exits == (strcmp(test, "new-window") == 0 ? 2U : 1U))
            break;
        if (state.owner == NULL && g_file_test(finished, G_FILE_TEST_IS_REGULAR))
        {
            for (size_t tick = 0U; tick < 100U; ++tick)
            {
                g_usleep(1000U);
                while (g_main_context_iteration(NULL, FALSE))
                {
                }
            }
            break;
        }
        g_usleep(1000U);
    }
    CHECK(g_file_test(finished, G_FILE_TEST_IS_REGULAR));
    if (state.owner == NULL)
        CHECK(state.calls == 1U);
    else
    {
        CHECK(state.exited && state.calls == (strcmp(test, "new-window") == 0 ? 4U : 2U));
        if (state.check_reentry)
            CHECK(state.reentry == UMI_STATUS_BUSY);
        CHECK(UmiGtk4ApplicationProcessesLatest(state.owner, "org.umicom.media-studio", &receipt) ==
              UMI_STATUS_OK);
        CHECK(receipt.exit_code == (strcmp(test, "exit-failure") == 0 ? 7 : 0));
        CHECK(strcmp(receipt.executable_path, path) == 0);
    }
done:
    UmiGtk4ApplicationProcessesDestroy(state.owner);
    g_clear_object(&source);
    g_clear_object(&target);
    g_clear_error(&error);
    /* Every name belongs to this newly created fixture directory. No recursive
     * removal or product-directory mutation is used during cleanup. */
    if (path != NULL)
        (void)g_remove(path);
    if (marker != NULL)
        (void)g_remove(marker);
    if (failure != NULL)
        (void)g_remove(failure);
    if (finished != NULL)
        (void)g_remove(finished);
    if (directory != NULL)
        (void)g_rmdir(directory);
    if (outer_directory != NULL)
        (void)g_rmdir(outer_directory);
    g_free(outer_directory);
    g_free(path);
    g_free(marker);
    g_free(failure);
    g_free(finished);
    g_free(directory);
    return result;
}
