/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/directory_guard.h
 * Purpose: Share build-directory ownership between native workflow hosts.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_DIRECTORY_GUARD_H
#define UMICOM_BUILD_DIRECTORY_GUARD_H
#include "umicom/build/project_session.h"
#include "umicom/platform/file_lease.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Append the fixed .umicom-build.lock guard filename to an absolute,
 * existing build-directory selection. Trailing separators are accepted.
 * This only formats and validates syntax; it does not inspect the directory.
 * Refusals preserve output. Input/output storage must not overlap. */
UmiStatus UmiBuildDirectoryGuardPath(const char *directory, char *output, size_t capacity);
/** Acquire the same guard used by opted-in build sessions. The directory must
 * already exist. No directories, source files or build files are created;
 * only the persistent guard file may be created. Release with UmiFileLeaseRelease.
 * This coordinates participants, not direct cmake/ninja/ctest invocations.
 * See file_lease.h for filesystem, ancestor and process-lifetime limits. */
UmiStatus UmiBuildDirectoryGuardAcquire(const char *directory, UmiFileLease **outLease);

typedef struct UmiBuildDirectoryGuardSnapshot {
    uint64_t operation_id;
    bool enabled;
    bool attempted;
    bool held;
    UmiStatus acquisition_status;
    UmiStatus release_status;
    char path[UMI_PATH_CAPACITY];
} UmiBuildDirectoryGuardSnapshot;
/** Opt in on the session owner's thread while its queue is idle. Defaults to
 * disabled for compatibility. Enabled sessions acquire before the first phase
 * and hold through every configure/build/test/install/package/run phase.
 * An unavailable or busy guard stops the workflow before any executor runs.
 * Create the chosen build directory before submitting, including Configure.
 * Profiles selecting a CMake preset return UNAVAILABLE before execution: the
 * preset may choose another binary directory and is not resolved by this API.
 * Custom executors must return only after their protected work has stopped.
 * Enabling this does not retroactively protect other, unguarded sessions. */
UmiStatus UmiBuildProjectSessionSetDirectoryGuard(UmiBuildProjectSession *session, bool enabled);
/** Copy guard evidence under the session mutex. No disk I/O or waiting for
 * build completion. Status fields are meaningful when attempted is true. */
UmiStatus UmiBuildProjectSessionReadDirectoryGuard(UmiBuildProjectSession *session,
    UmiBuildDirectoryGuardSnapshot *outSnapshot);
#ifdef __cplusplus
}
#endif
#endif
