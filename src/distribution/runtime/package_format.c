/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/package_format.c
 *
 * PURPOSE:
 *   package format descriptors and platform suitability checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_format.h"

/*
 * Provide the dr package format installer operation used by this module and its client
 * applications.
 */
bool umi_dr_package_format_installer(UmiDrPackageFormat f){return f==UMI_DR_PACKAGE_MSI||f==UMI_DR_PACKAGE_MSIX||f==UMI_DR_PACKAGE_DEB||f==UMI_DR_PACKAGE_RPM||f==UMI_DR_PACKAGE_DMG||f==UMI_DR_PACKAGE_PKG;}bool umi_dr_package_format_supported(UmiDrPackageFormat f,UmiDrPlatform p){/* Apply this operation only while the related capability or state is available. */ if(f==UMI_DR_PACKAGE_ZIP||f==UMI_DR_PACKAGE_TAR||f==UMI_DR_PACKAGE_PORTABLE)return true;/* Apply this operation only while the related capability or state is available. */ if(p==UMI_DR_PLATFORM_WINDOWS)return f==UMI_DR_PACKAGE_MSI||f==UMI_DR_PACKAGE_MSIX;/* Apply this operation only while the related capability or state is available. */ if(p==UMI_DR_PLATFORM_LINUX)return f==UMI_DR_PACKAGE_DEB||f==UMI_DR_PACKAGE_RPM||f==UMI_DR_PACKAGE_APPIMAGE;/* Apply this operation only while the related capability or state is available. */ if(p==UMI_DR_PLATFORM_MACOS)return f==UMI_DR_PACKAGE_DMG||f==UMI_DR_PACKAGE_PKG;return false;}
