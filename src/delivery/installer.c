/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/installer.c
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
#include "umicom/delivery/installer.h"
#include "../base/value_archive_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise installer plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_installer_plan_init(UmiInstallerPlan *plan,
                                      const char *product_name,
                                      const char *vendor,
                                      const char *version,
                                      const char *install_directory,
                                      const char *entrypoint,
                                      UmiInstallerPlatform platform,
                                      UmiInstallScope scope)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || product_name == NULL || vendor == NULL ||
        version == NULL || install_directory == NULL || entrypoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(plan, 0, sizeof(*plan));
    status = umi_delivery_copy_text(plan->product_name,
                                    sizeof(plan->product_name), product_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->vendor, sizeof(plan->vendor), vendor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->version, sizeof(plan->version), version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->install_directory,
                                    sizeof(plan->install_directory),
                                    install_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(plan->entrypoint,
                                    sizeof(plan->entrypoint), entrypoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    plan->platform = platform;
    plan->scope = scope;
    plan->create_start_menu_shortcut = platform == UMI_INSTALLER_WINDOWS;
    plan->include_uninstaller = scope != UMI_INSTALL_SCOPE_PORTABLE;
    plan->require_signature = scope != UMI_INSTALL_SCOPE_PORTABLE;
    return UMI_STATUS_OK;
}

/*
 * Provide the installer plan set shortcuts operation used by this module and its client
 * applications.
 */
void umi_installer_plan_set_shortcuts(UmiInstallerPlan *plan,
                                          int start_menu,
                                          int desktop)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    plan->create_start_menu_shortcut = start_menu != 0;
    plan->create_desktop_shortcut = desktop != 0;
}

/* Check that installer plan satisfies its contract before another service relies on it. */
UmiStatus umi_installer_plan_validate(const UmiInstallerPlan *plan)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->product_name, '\0', sizeof(plan->product_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->vendor, '\0', sizeof(plan->vendor)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->version, '\0', sizeof(plan->version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->install_directory, '\0', sizeof(plan->install_directory)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(plan->entrypoint, '\0', sizeof(plan->entrypoint)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->product_name[0] == '\0' || plan->vendor[0] == '\0' ||
        plan->version[0] == '\0' || plan->install_directory[0] == '\0' ||
        plan->entrypoint[0] == '\0' ||
        (plan->platform != UMI_INSTALLER_WINDOWS &&
         plan->platform != UMI_INSTALLER_LINUX) ||
        plan->scope < UMI_INSTALL_SCOPE_USER ||
        plan->scope > UMI_INSTALL_SCOPE_PORTABLE) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->scope != UMI_INSTALL_SCOPE_PORTABLE &&
        !plan->include_uninstaller) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/*
 * Provide the installer plan generator operation used by this module and its client
 * applications.
 */
const char *umi_installer_plan_generator(const UmiInstallerPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return "unknown";
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->scope == UMI_INSTALL_SCOPE_PORTABLE) return "ZIP";
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->platform == UMI_INSTALLER_WINDOWS) return "NSIS";
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->platform == UMI_INSTALLER_LINUX) return "TGZ";
    return "unknown";
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiInstallerPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x00851353b52b6d06);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstallerPlan *)0)->product_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstallerPlan *)0)->vendor)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstallerPlan *)0)->version)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstallerPlan *)0)->install_directory)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiInstallerPlan *)0)->entrypoint)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiInstallerPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiInstallerPlan *)0)->product_name) - 1U +
        8U + sizeof(((UmiInstallerPlan *)0)->vendor) - 1U +
        8U + sizeof(((UmiInstallerPlan *)0)->version) - 1U +
        8U + sizeof(((UmiInstallerPlan *)0)->install_directory) - 1U +
        8U + sizeof(((UmiInstallerPlan *)0)->entrypoint) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiInstallerPlanArchiveWrite(UmiArchiveWriter *writer, const UmiInstallerPlan *value)
{
    UmiArchiveWriteText(writer, value->product_name, sizeof(value->product_name));
    UmiArchiveWriteText(writer, value->vendor, sizeof(value->vendor));
    UmiArchiveWriteText(writer, value->version, sizeof(value->version));
    UmiArchiveWriteText(writer, value->install_directory, sizeof(value->install_directory));
    UmiArchiveWriteText(writer, value->entrypoint, sizeof(value->entrypoint));
    UmiArchiveWriteSigned(writer, (int64_t)value->platform);
    UmiArchiveWriteSigned(writer, (int64_t)value->scope);
    UmiArchiveWriteSigned(writer, (int64_t)value->create_start_menu_shortcut);
    UmiArchiveWriteSigned(writer, (int64_t)value->create_desktop_shortcut);
    UmiArchiveWriteSigned(writer, (int64_t)value->include_uninstaller);
    UmiArchiveWriteSigned(writer, (int64_t)value->require_signature);
}
static void UmiInstallerPlanArchiveRead(UmiArchiveReader *reader, UmiInstallerPlan *value)
{
    UmiArchiveReadText(reader, value->product_name, sizeof(value->product_name));
    UmiArchiveReadText(reader, value->vendor, sizeof(value->vendor));
    UmiArchiveReadText(reader, value->version, sizeof(value->version));
    UmiArchiveReadText(reader, value->install_directory, sizeof(value->install_directory));
    UmiArchiveReadText(reader, value->entrypoint, sizeof(value->entrypoint));
    value->platform = (UmiInstallerPlatform)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->scope = (UmiInstallScope)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->create_start_menu_shortcut = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->create_desktop_shortcut = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->include_uninstaller = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->require_signature = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiInstallerPlanArchiveValidate(const UmiInstallerPlan *value)
{
    return umi_installer_plan_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_installer_plan_archive_encode, umi_installer_plan_archive_decode,
    UmiInstallerPlan, UmiInstallerPlanArchiveSchema, UmiInstallerPlanArchiveBound, UmiInstallerPlanArchiveWrite, UmiInstallerPlanArchiveRead, UmiInstallerPlanArchiveValidate)
