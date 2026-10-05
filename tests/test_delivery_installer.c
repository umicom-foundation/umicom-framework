/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_installer.c
 *
 * PURPOSE:
 *   Verify Windows, Linux and portable installer-generation contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/delivery/delivery.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/installer.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiInstallerPlanTransferEqual(const UmiInstallerPlan *a, const UmiInstallerPlan *b)
{
    return strcmp(a->product_name, b->product_name) == 0 &&
        strcmp(a->vendor, b->vendor) == 0 &&
        strcmp(a->version, b->version) == 0 &&
        strcmp(a->install_directory, b->install_directory) == 0 &&
        strcmp(a->entrypoint, b->entrypoint) == 0 &&
        a->platform == b->platform &&
        a->scope == b->scope &&
        a->create_start_menu_shortcut == b->create_start_menu_shortcut &&
        a->create_desktop_shortcut == b->create_desktop_shortcut &&
        a->include_uninstaller == b->include_uninstaller &&
        a->require_signature == b->require_signature;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiInstallerPlanTransferTails(UmiInstallerPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->product_name) + 1U;
        memset(value->product_name + used, 0xa5, sizeof(value->product_name) - used);
    }
    {
        size_t used = strlen(value->vendor) + 1U;
        memset(value->vendor + used, 0xa5, sizeof(value->vendor) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->install_directory) + 1U;
        memset(value->install_directory + used, 0xa5, sizeof(value->install_directory) - used);
    }
    {
        size_t used = strlen(value->entrypoint) + 1U;
        memset(value->entrypoint + used, 0xa5, sizeof(value->entrypoint) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiInstallerPlanTransferMalformed(const UmiInstallerPlan *sample)
{
    (void)sample;
    {
        UmiInstallerPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.product_name, 'x', sizeof(invalid.product_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_installer_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_installer_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated product_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstallerPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.vendor, 'x', sizeof(invalid.vendor));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_installer_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_installer_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated vendor was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstallerPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.version, 'x', sizeof(invalid.version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_installer_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_installer_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated version was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstallerPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.install_directory, 'x', sizeof(invalid.install_directory));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_installer_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_installer_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated install_directory was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiInstallerPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entrypoint, 'x', sizeof(invalid.entrypoint));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_installer_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_installer_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entrypoint was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiInstallerPlanTransferCases, UmiInstallerPlan,
    umi_installer_plan_archive_encode, umi_installer_plan_archive_decode,
    UmiInstallerPlanTransferEqual, UmiInstallerPlanTransferTails, UmiInstallerPlanTransferMalformed)

int main(void)
{
    UmiInstallerPlan windows;
    UmiInstallerPlan linux;
    assert(umi_installer_plan_init(
               &windows, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "Umicom Studio", "bin/umicom-studio-ide.exe",
               UMI_INSTALLER_WINDOWS, UMI_INSTALL_SCOPE_USER) ==
           UMI_STATUS_OK);
    assert(umi_installer_plan_validate(&windows) == UMI_STATUS_OK);
    if (UmiInstallerPlanTransferCases(&windows) != 0) return 1;

    assert(strcmp(umi_installer_plan_generator(&windows), "NSIS") == 0);
    assert(umi_installer_plan_init(
               &linux, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "/opt/umicom-studio", "bin/umicom-studio-ide",
               UMI_INSTALLER_LINUX, UMI_INSTALL_SCOPE_PORTABLE) ==
           UMI_STATUS_OK);
    assert(strcmp(umi_installer_plan_generator(&linux), "ZIP") == 0);
    return 0;
}
