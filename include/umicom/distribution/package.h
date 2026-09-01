/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/package.h
 *
 * PURPOSE:
 *   Describe installable application, extension, runtime, template and content
 *   releases independently from archive formats and marketplace providers.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/*
 * A distribution package is metadata, not an installer.  It can be inspected,
 * filtered and validated before any downloader or filesystem adapter runs.
 */
#ifndef UMICOM_DISTRIBUTION_PACKAGE_H
#define UMICOM_DISTRIBUTION_PACKAGE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DISTRIBUTION_API_VERSION 2U
#define UMI_DISTRIBUTION_ID_CAPACITY 128U
#define UMI_DISTRIBUTION_NAME_CAPACITY 256U
#define UMI_DISTRIBUTION_TEXT_CAPACITY 512U
#define UMI_DISTRIBUTION_URI_CAPACITY 512U
#define UMI_DISTRIBUTION_PACKAGE_CAPACITY 1024U

typedef enum UmiDistributionPackageKind {
    UMI_DISTRIBUTION_APPLICATION = 1,
    UMI_DISTRIBUTION_EXTENSION = 2,
    UMI_DISTRIBUTION_RUNTIME = 3,
    UMI_DISTRIBUTION_TEMPLATE = 4,
    UMI_DISTRIBUTION_CONTENT = 5
} UmiDistributionPackageKind;

typedef struct UmiDistributionPackage {
    uint32_t struct_size;
    uint32_t api_version;
    char release_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char package_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char product_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char provider_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char display_name[UMI_DISTRIBUTION_NAME_CAPACITY];
    char summary[UMI_DISTRIBUTION_TEXT_CAPACITY];
    char category[UMI_DISTRIBUTION_ID_CAPACITY];
    char licence_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char source_uri[UMI_DISTRIBUTION_URI_CAPACITY];
    UmiVersion version;
    UmiReleaseChannel channel;
    UmiDistributionPackageKind kind;
    uint32_t required_framework_abi;
    uint64_t download_size;
    int trusted;
    int compatible;
    int security_update;
    int deprecated;
    uint64_t revision;
} UmiDistributionPackage;

UmiStatus umi_distribution_package_init(
    UmiDistributionPackage *package,
    const char *release_id,
    const char *package_id,
    const char *product_id,
    const char *display_name,
    UmiVersion version,
    UmiDistributionPackageKind kind,
    UmiReleaseChannel channel);
UmiStatus umi_distribution_package_validate(
    const UmiDistributionPackage *package,
    char *out_reason,
    size_t reason_capacity);
int umi_distribution_version_compare(UmiVersion left, UmiVersion right);
const char *umi_distribution_package_kind_text(UmiDistributionPackageKind kind);

#ifdef __cplusplus
}
#endif

#endif
