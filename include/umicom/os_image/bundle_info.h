/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Public projection for runtime consumers. Image parsing remains in os_image. */
#ifndef UMICOM_OS_IMAGE_BUNDLE_INFO_H
#define UMICOM_OS_IMAGE_BUNDLE_INFO_H
#include "umicom/os_image/image.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiOsImageBundleInfo {
        UmiOsImageArch architecture;
        char sourceId[65],manifestHash[65];
        char kernel[UMI_OS_IMAGE_PATH],rootfs[UMI_OS_IMAGE_PATH];
    }
    UmiOsImageBundleInfo;
    UmiStatus UmiOsImageDescribe(const char *root,UmiOsImageBundleInfo *outInfo,     UmiOsImageReport *report);
#ifdef __cplusplus
}
#endif
#endif
