/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/consumer_package_v2.h
 *
 * PURPOSE:
 *   Describe and render the CMake package contract used by SDK consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_CONSUMER_PACKAGE_V2_H
#define UMICOM_SDK_CONSUMER_PACKAGE_V2_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/sdk/export_catalogue_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SDK_V2_PATH_CAPACITY 512U

typedef struct UmiSdkConsumerPackageV2 {
    char package_name[UMI_SDK_V2_ID_CAPACITY];
    char namespace_name[UMI_SDK_V2_ID_CAPACITY];
    char minimum_version[64];
    char config_directory[UMI_SDK_V2_PATH_CAPACITY];
    int include_examples;
    int include_toolchain_file;
} UmiSdkConsumerPackageV2;

UmiStatus umi_sdk_consumer_package_v2_init(UmiSdkConsumerPackageV2 *package,
                                            const char *package_name,
                                            const char *namespace_name,
                                            const char *minimum_version,
                                            const char *config_directory);
UmiStatus umi_sdk_consumer_package_v2_validate(
    const UmiSdkConsumerPackageV2 *package);
UmiStatus umi_sdk_consumer_package_v2_render_cmake(
    const UmiSdkConsumerPackageV2 *package,
    const UmiSdkExportCatalogueV2 *catalogue,
    char *output,
    size_t output_capacity);

#ifdef __cplusplus
}
#endif
#endif
