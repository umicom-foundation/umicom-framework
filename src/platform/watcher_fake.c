/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/watcher_fake.c
 *
 * PURPOSE:
 *   Implement deterministic watcher-event injection for contract tests and
 *   headless products without touching the operating-system filesystem.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "include/watcher_internal.h"

UmiStatus umi_watcher_fake_emit(UmiWatcher *watcher,
                                UmiWatchEventKind kind,
                                const char *path,
                                const char *previous_path,
                                int directory)
{
    if (watcher == NULL || watcher->config.backend != UMI_WATCHER_FAKE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_watcher_emit(watcher,
                            kind,
                            path,
                            previous_path,
                            directory);
}
