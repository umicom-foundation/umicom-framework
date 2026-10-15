/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/scaffold/application_resources.c
 * PURPOSE: Locate shared application templates for native command and graphical hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_module.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

#ifndef UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT
#define UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT ""
#endif
#ifndef UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR
#define UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR "share/umicom/templates/application_module"
#endif

/* Keep resource discovery in Framework so an installed IDE and the command line
 * agree about the same shipped templates. The plan still owns a byte snapshot;
 * finding this directory does not authorise writes or execute template content. */
UmiStatus umi_application_scaffold_template_root(char *output, size_t capacity)
{
    char image[UMI_PATH_CAPACITY], bin[UMI_PATH_CAPACITY];
    char prefix[UMI_PATH_CAPACITY], candidate[UMI_PATH_CAPACITY];
    if (output == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_fs_executable_path(image, sizeof image);
    if (status == UMI_STATUS_OK) status = umi_path_parent(image, bin, sizeof bin);
    if (status == UMI_STATUS_OK) status = umi_path_parent(bin, prefix, sizeof prefix);
    if (status == UMI_STATUS_OK) {
        const char *installed = UMICOM_APPLICATION_MODULE_TEMPLATE_INSTALL_DIR;
        status = umi_path_is_absolute(installed)
            ? umi_path_copy(candidate, sizeof candidate, installed)
            : umi_path_join(prefix, installed, candidate, sizeof candidate);
        if (status == UMI_STATUS_OK && umi_fs_is_directory(candidate))
            return umi_path_copy(output, capacity, candidate);
    }
    const char *fallback = UMICOM_APPLICATION_MODULE_TEMPLATE_ROOT;
    if (!umi_path_is_absolute(fallback) || !umi_fs_is_directory(fallback))
        return UMI_STATUS_NOT_FOUND;
    return umi_path_copy(output, capacity, fallback);
}
