/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/application_processes_gtk4.c
 * PURPOSE: Bridge shared launch receipts to asynchronous native process exit evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/application_processes.h"
#include "umicom/application/native_discovery.h"
#include "umicom/application/portfolio.h"
#include <gio/gio.h>
#include <string.h>

struct UmiGtk4ApplicationProcesses
{
    UmiApplicationLaunchReceipts *receipts;
    UmiGtk4ApplicationProcessObserver observer;
    void *context;
    size_t references;
    bool closed;
    unsigned notification_depth;
};
typedef struct ApplicationWait
{
    UmiGtk4ApplicationProcesses *owner;
    GSubprocess *process;
    uint64_t receipt_id;
} ApplicationWait;

/* A wait owns state, never a UI reference. This differs deliberately from a
 * build job: closing its launching window must not kill unsaved work in the
 * independently opened application. Extend lifecycle policy in this owner. */
static void release(UmiGtk4ApplicationProcesses *owner)
{
    if (--owner->references != 0U)
        return;
    UmiApplicationLaunchReceiptsDestroy(owner->receipts);
    g_free(owner);
}

static void notify(UmiGtk4ApplicationProcesses *owner, uint64_t id)
{
    UmiApplicationLaunchReceipt receipt;
    if (owner->closed || owner->observer == NULL ||
        UmiApplicationLaunchReceiptsFind(owner->receipts, id, &receipt) != UMI_STATUS_OK)
        return;
    ++owner->references;
    ++owner->notification_depth;
    owner->observer(&receipt, owner->context);
    --owner->notification_depth;
    release(owner);
}

/* Finish on the creating main context. A failed observation preserves an
 * uncertain active record instead of falsely claiming that the child exited. */
static void process_finished(GObject *source, GAsyncResult *result, gpointer data)
{
    ApplicationWait *wait = data;
    UmiGtk4ApplicationProcesses *owner = wait->owner;
    GError *error = NULL;
    if (g_subprocess_wait_finish(G_SUBPROCESS(source), result, &error))
    {
        int code =
            g_subprocess_get_if_exited(wait->process) ? g_subprocess_get_exit_status(wait->process) : -1;
        (void)UmiApplicationLaunchReceiptsExited(owner->receipts, wait->receipt_id, code);
    }
    else
    {
        (void)UmiApplicationLaunchReceiptsUncertain(owner->receipts, wait->receipt_id, UMI_STATUS_IO_ERROR);
    }
    g_clear_error(&error);
    notify(owner, wait->receipt_id);
    g_object_unref(wait->process);
    g_free(wait);
    release(owner);
}

UmiStatus UmiGtk4ApplicationProcessesCreate(UmiGtk4ApplicationProcesses **out)
{
    UmiStatus status;
    UmiGtk4ApplicationProcesses *owner;
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    owner = g_try_new0(UmiGtk4ApplicationProcesses, 1);
    if (owner == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiApplicationLaunchReceiptsCreate(&owner->receipts);
    if (status != UMI_STATUS_OK)
    {
        g_free(owner);
        return status;
    }
    owner->references = 1U;
    *out = owner;
    return UMI_STATUS_OK;
}

void UmiGtk4ApplicationProcessesDestroy(UmiGtk4ApplicationProcesses *processes)
{
    if (processes == NULL || processes->closed)
        return;
    processes->closed = true;
    processes->observer = NULL;
    processes->context = NULL;
    release(processes);
}

UmiStatus UmiGtk4ApplicationProcessesObserve(UmiGtk4ApplicationProcesses *processes,
                                             UmiGtk4ApplicationProcessObserver observer, void *context)
{
    if (processes == NULL || processes->closed)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (processes->notification_depth != 0U)
        return UMI_STATUS_BUSY;
    processes->observer = observer;
    processes->context = observer != NULL ? context : NULL;
    return UMI_STATUS_OK;
}

/* Keep location validation with Framework's canonical GUI catalogue. Presence
 * is not a signature check: callers must select a directory they trust. */
static UmiStatus validate_location(const char *application_id, const char *path, char **out_directory)
{
    const char *name = umi_application_portfolio_gui_executable(application_id);
    UmiApplicationNativeDiscoveryConfig config = umi_application_native_discovery_config_default();
    char *directory, *base, *expected;
    bool present = false;
    UmiStatus status;
    if (name == NULL || path == NULL || !g_utf8_validate(path, -1, NULL) || !g_path_is_absolute(path))
        return UMI_STATUS_INVALID_ARGUMENT;
    directory = g_path_get_dirname(path);
    base = g_path_get_basename(path);
    expected = g_strconcat(name, config.executable_suffix, NULL);
    config.executable_root = directory;
    status = strcmp(base, expected) == 0
                 ? umi_application_native_discovery_probe(&config, application_id, &present)
                 : UMI_STATUS_INVALID_ARGUMENT;
    g_free(base);
    g_free(expected);
    if (status == UMI_STATUS_OK && !present)
        status = UMI_STATUS_NOT_FOUND;
    if (status != UMI_STATUS_OK)
        g_free(directory);
    else
        *out_directory = directory;
    return status;
}

/* Reject text scripts before passing a canonical GUI filename to the native
 * loader. This is a small format check, not executable signing or an atomic
 * protection against replacement in an untrusted directory. */
static UmiStatus check_native_format(const char *path)
{
    unsigned char magic[4] = {0};
    gsize count = 0U;
    GError *error = NULL;
    GFile *file = g_file_new_for_path(path);
    GFileInputStream *stream = g_file_read(file, NULL, &error);
    UmiStatus status = UMI_STATUS_OK;
    if (stream == NULL ||
        !g_input_stream_read_all(G_INPUT_STREAM(stream), magic, sizeof(magic), &count, NULL, &error))
        status = error != NULL && g_error_matches(error, G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED)
                     ? UMI_STATUS_PERMISSION_DENIED
                     : UMI_STATUS_IO_ERROR;
    else
    {
#ifdef G_OS_WIN32
        if (count != sizeof(magic) || magic[0] != 'M' || magic[1] != 'Z')
            status = UMI_STATUS_UNAVAILABLE;
#elif defined(__APPLE__)
        const unsigned char formats[][4] = {{0xcf, 0xfa, 0xed, 0xfe}, {0xce, 0xfa, 0xed, 0xfe},
                                            {0xfe, 0xed, 0xfa, 0xcf}, {0xfe, 0xed, 0xfa, 0xce},
                                            {0xca, 0xfe, 0xba, 0xbe}, {0xca, 0xfe, 0xba, 0xbf},
                                            {0xbe, 0xba, 0xfe, 0xca}, {0xbf, 0xba, 0xfe, 0xca}};
        bool known = false;
        for (size_t index = 0U; index < sizeof(formats) / sizeof(formats[0]); ++index)
            if (memcmp(magic, formats[index], sizeof(magic)) == 0)
                known = true;
        if (count != sizeof(magic) || !known)
            status = UMI_STATUS_UNAVAILABLE;
#else
        if (count != sizeof(magic) || memcmp(magic, "\177ELF", sizeof(magic)) != 0)
            status = UMI_STATUS_UNAVAILABLE;
#endif
    }
    g_clear_object(&stream);
    g_object_unref(file);
    g_clear_error(&error);
    return status;
}

UmiStatus UmiGtk4ApplicationProcessesStart(UmiGtk4ApplicationProcesses *processes, const char *application_id,
                                           const char *absolute_executable, bool explicit_new_instance,
                                           uint64_t *out_id)
{
    ApplicationWait *wait;
    GSubprocessLauncher *launcher;
    GError *error = NULL;
    const char *arguments[2] = {absolute_executable, NULL};
    char *directory = NULL;
    UmiStatus status;
    if (processes == NULL || processes->closed || application_id == NULL || absolute_executable == NULL ||
        out_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (processes->notification_depth != 0U)
        return UMI_STATUS_BUSY;
    status = validate_location(application_id, absolute_executable, &directory);
    if (status != UMI_STATUS_OK)
        return status;
    wait = g_try_new0(ApplicationWait, 1);
    if (wait == NULL)
    {
        g_free(directory);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = UmiApplicationLaunchReceiptsBegin(processes->receipts, application_id, absolute_executable,
                                               explicit_new_instance, &wait->receipt_id);
    if (status != UMI_STATUS_OK)
    {
        g_free(directory);
        g_free(wait);
        return status;
    }
    *out_id = wait->receipt_id;
    status = check_native_format(absolute_executable);
    if (status != UMI_STATUS_OK)
    {
        uint64_t id = wait->receipt_id;
        (void)UmiApplicationLaunchReceiptsRejected(processes->receipts, id, status);
        g_free(directory);
        g_free(wait);
        notify(processes, id);
        return status;
    }

    launcher = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_NONE);
    g_subprocess_launcher_set_cwd(launcher, directory);
    wait->process = g_subprocess_launcher_spawnv(launcher, arguments, &error);
    g_object_unref(launcher);
    g_free(directory);
    if (wait->process == NULL)
    {
        uint64_t id = wait->receipt_id;
        status = error != NULL && g_error_matches(error, G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED)
                     ? UMI_STATUS_PERMISSION_DENIED
                     : UMI_STATUS_UNAVAILABLE;
        (void)UmiApplicationLaunchReceiptsRejected(processes->receipts, id, status);
        g_clear_error(&error);
        g_free(wait);
        notify(processes, id);
        return status;
    }
    /* Tracking and wait storage were reserved before spawn. These transitions
     * cannot lose the child through a later allocation or a caller UI teardown. */
    (void)UmiApplicationLaunchReceiptsStarted(processes->receipts, wait->receipt_id);
    wait->owner = processes;
    ++processes->references;
    g_subprocess_wait_async(wait->process, NULL, process_finished, wait);
    notify(processes, wait->receipt_id);
    return UMI_STATUS_OK;
}

UmiStatus UmiGtk4ApplicationProcessesLatest(const UmiGtk4ApplicationProcesses *processes,
                                            const char *application_id, UmiApplicationLaunchReceipt *out)
{
    if (processes == NULL || processes->closed)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiApplicationLaunchReceiptsLatest(processes->receipts, application_id, out);
}

UmiStatus UmiGtk4ApplicationProcessesAt(const UmiGtk4ApplicationProcesses *processes, size_t index,
                                        UmiApplicationLaunchReceipt *out)
{
    if (processes == NULL || processes->closed)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiApplicationLaunchReceiptsAt(processes->receipts, index, out);
}
