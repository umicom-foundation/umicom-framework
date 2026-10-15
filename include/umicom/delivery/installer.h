/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/installer.h
 *
 * PURPOSE:
 *   Describe Windows and Linux installer-generation contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_INSTALLER_H
#define UMICOM_DELIVERY_INSTALLER_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named installer platform values accepted by this public contract.
 */
typedef enum UmiInstallerPlatform {
    UMI_INSTALLER_WINDOWS = 1,
    UMI_INSTALLER_LINUX = 2
} UmiInstallerPlatform;

/**
 * List the named install scope values accepted by this public contract.
 */
typedef enum UmiInstallScope {
    UMI_INSTALL_SCOPE_USER = 1,
    UMI_INSTALL_SCOPE_MACHINE = 2,
    UMI_INSTALL_SCOPE_PORTABLE = 3
} UmiInstallScope;

/**
 * Represent the installer plan data shared with callers of this public contract.
 */
typedef struct UmiInstallerPlan {
    char product_name[UMI_DELIVERY_ID_CAPACITY];
    char vendor[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char install_directory[UMI_DELIVERY_PATH_CAPACITY];
    char entrypoint[UMI_DELIVERY_PATH_CAPACITY];
    UmiInstallerPlatform platform;
    UmiInstallScope scope;
    int create_start_menu_shortcut;
    int create_desktop_shortcut;
    int include_uninstaller;
    int require_signature;
} UmiInstallerPlan;

/**
 * Initialise installer plan from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_installer_plan_init(UmiInstallerPlan *plan,
                                      const char *product_name,
                                      const char *vendor,
                                      const char *version,
                                      const char *install_directory,
                                      const char *entrypoint,
                                      UmiInstallerPlatform platform,
                                      UmiInstallScope scope);
/**
 * Provide the installer plan set shortcuts operation used by this module and its client
 * applications.
 */
void umi_installer_plan_set_shortcuts(UmiInstallerPlan *plan,
                                          int start_menu,
                                          int desktop);
/**
 * Check that installer plan satisfies its contract before another service relies on it.
 */
UmiStatus umi_installer_plan_validate(const UmiInstallerPlan *plan);
/**
 * Provide the installer plan generator operation used by this module and its client
 * applications.
 */
const char *umi_installer_plan_generator(const UmiInstallerPlan *plan);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_installer_plan_archive_encode(const UmiInstallerPlan *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_installer_plan_archive_decode(const void *bytes, size_t byte_count,
    UmiInstallerPlan *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_installer_plan_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiInstallerPlan *expected, UmiInstallerPlan *value);


#ifdef __cplusplus
}
#endif
#endif
