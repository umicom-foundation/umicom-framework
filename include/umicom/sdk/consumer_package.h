/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/consumer_package.h
 *
 * PURPOSE:
 *   Describe and render the CMake package contract used by SDK consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_CONSUMER_PACKAGE_H
#define UMICOM_SDK_CONSUMER_PACKAGE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/sdk/export_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SDK_PACKAGE_PATH_CAPACITY 512U

typedef struct UmiSdkConsumerPackage {
    char package_name[UMI_SDK_EXPORT_ID_CAPACITY];
    char namespace_name[UMI_SDK_EXPORT_ID_CAPACITY];
    char minimum_version[64];
    char config_directory[UMI_SDK_PACKAGE_PATH_CAPACITY];
    int include_examples;
    int include_toolchain_file;
} UmiSdkConsumerPackage;

UmiStatus umi_sdk_consumer_package_init(UmiSdkConsumerPackage *package,
                                            const char *package_name,
                                            const char *namespace_name,
                                            const char *minimum_version,
                                            const char *config_directory);
UmiStatus umi_sdk_consumer_package_validate(
    const UmiSdkConsumerPackage *package);
UmiStatus umi_sdk_consumer_package_render_cmake(
    const UmiSdkConsumerPackage *package,
    const UmiSdkExportCatalogue *catalogue,
    char *output,
    size_t output_capacity);

#ifdef __cplusplus
}
#endif
#endif
