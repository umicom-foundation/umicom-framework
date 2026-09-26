/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/setup_centre/setup.h
 * Purpose: Reviewed, native installation of an explicit offline suite payload.
 * Architecture: Framework owns manifests, selection, integrity and file writes.
 * GUI adapters project this contract. No installer executes a package script.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SETUP_CENTRE_SETUP_H
#define UMICOM_SETUP_CENTRE_SETUP_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {

#endif
#define UMI_SETUP_PATH_CAPACITY 4096U
#define UMI_SETUP_RELATIVE_CAPACITY 768U
#define UMI_SETUP_MAX_APPS 64U
#define UMI_SETUP_MAX_FILES 32768U
#define UMI_SETUP_MAX_FILE_BYTES UINT64_C(17179869184)
#define UMI_SETUP_MAX_TOTAL_BYTES UINT64_C(137438953472)
#define UMI_SETUP_MANIFEST_LIMIT (32U * 1024U * 1024U)
#define UMI_SETUP_CATALOGUE "suite.umi"
#define UMI_SETUP_RECEIPT "umicom-install.umi"
    /** A title is UTF-8. id is an ASCII token. entry is a relative payload path.
     * Pointers returned by the accessors remain borrowed from their bundle. */

    typedef struct UmiSetupApplication {

        char id[80];

        char title[192];

        char entry[UMI_SETUP_RELATIVE_CAPACITY];

    }
    UmiSetupApplication;

    typedef struct UmiSetupReport {

        UmiStatus status;

        int systemError;

        int outputCreated;

        int completed;

        size_t filesCompleted;

        uint64_t bytesCompleted;

        uint64_t bytesPlanned;

        char detail[320];

    }
    UmiSetupReport;

    typedef struct UmiSetupBundle UmiSetupBundle;

    /** Called synchronously on the working thread. Return nonzero to request a
     * stop between chunks. Callbacks must not mutate/destroy the current bundle.
     * Never manipulate GUI widgets here; copy observations to the GUI thread. */

    typedef int (*UmiSetupProgress)(const UmiSetupReport *report, void *context);

    /** Opens data only. The catalogue is not executable CMake or a script.
     * No filesystem writes, program execution or automatic network lookup. */

    UmiStatus UmiSetupBundleOpen(const char *root, UmiSetupBundle **outBundle, UmiSetupReport *report);

    void UmiSetupBundleDestroy(UmiSetupBundle *bundle);

    size_t UmiSetupApplicationCount(const UmiSetupBundle *bundle);

    const UmiSetupApplication *UmiSetupApplicationAt(const UmiSetupBundle *bundle, size_t index);

    uint64_t UmiSetupAllApplications(const UmiSetupBundle *bundle);

    /** Hash every selected file and derive a fingerprint over catalogue bytes,
     * selected IDs and the absolute destination. This reads; it does not reserve
     * the destination. Installation refuses existing destinations (including empty).
     * A digest detects changes, not who published a catalogue. Trust the source. */

    UmiStatus UmiSetupReview(const UmiSetupBundle *bundle, uint64_t selected,
    const char *destination, char outFingerprint[65], UmiSetupProgress progress,
    void *context, UmiSetupReport *report);

    /** Recheck the reviewed catalogue/files; then create a new, private directory.
     * Every copy is hashed again. A completion receipt is written LAST. A failed
     * or cancelled install retains its new, incomplete directory for inspection;
     * it is never reported as installed, merged into, or recursively deleted.
     * No updates, elevation, service registration, registry changes or autostart.
     * Thread rule: one operation per bundle at a time. expectedFingerprint required. */

    UmiStatus UmiSetupInstall(const UmiSetupBundle *bundle, uint64_t selected,
    const char *destination, const char *expectedFingerprint,
    UmiSetupProgress progress, void *context, UmiSetupReport *report);

    /** Read-only verification against the completion receipt. User-created files
     * are not deleted or interpreted. Receipt hashes are not publisher signatures. */

    UmiStatus UmiSetupVerifyInstallation(const char *root, UmiSetupReport *report);

    /** Create a new offline payload from selected, already generated runtime
     * reports. listFile uses the documented UMICOM_SETUP_INPUT format. Runtime
     * reports are parsed as five literal data fields, NEVER executed as CMake.
     * bootstrap may be NULL; otherwise it is copied as Umicom-Setup.exe and as an
     * installed helper. Existing NSIS/runtime producers remain alternatives.
     * Reported duplicate paths must agree in spelling, owner and content. */

    UmiStatus UmiSetupPack(const char *listFile, const char *destination,
    const char *bootstrap, UmiSetupProgress progress, void *context, UmiSetupReport *report);

    /** Native current-user Windows shortcuts, in a newly created group directory.
     * root must contain a valid installation receipt. Existing links/groups are
     * never replaced. Unsupported hosts report UNAVAILABLE. Shortcut failure does
     * not invalidate an otherwise complete installation; report it separately. */

    UmiStatus UmiSetupCreateShortcuts(const char *root, const char *groupName,
    int desktop, int startMenu, UmiSetupReport *report);

    /** Portable path/schema helpers are public for other delivery consumers. */
    UmiStatus UmiSetupValidateRelative(const char *path);

    UmiStatus UmiSetupValidateAbsolute(const char *path);

    const char *UmiSetupStatusText(UmiStatus status);

    int UmiSetupMain(int argc, char **argv);

#ifdef __cplusplus
}

#endif
#endif
