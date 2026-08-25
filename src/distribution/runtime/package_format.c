/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/package_format.c
 *
 * PURPOSE:
 *   package format descriptors and platform suitability checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_format.h"

bool umi_dr_package_format_installer(UmiDrPackageFormat f){return f==UMI_DR_PACKAGE_MSI||f==UMI_DR_PACKAGE_MSIX||f==UMI_DR_PACKAGE_DEB||f==UMI_DR_PACKAGE_RPM||f==UMI_DR_PACKAGE_DMG||f==UMI_DR_PACKAGE_PKG;}bool umi_dr_package_format_supported(UmiDrPackageFormat f,UmiDrPlatform p){if(f==UMI_DR_PACKAGE_ZIP||f==UMI_DR_PACKAGE_TAR||f==UMI_DR_PACKAGE_PORTABLE)return true;if(p==UMI_DR_PLATFORM_WINDOWS)return f==UMI_DR_PACKAGE_MSI||f==UMI_DR_PACKAGE_MSIX;if(p==UMI_DR_PLATFORM_LINUX)return f==UMI_DR_PACKAGE_DEB||f==UMI_DR_PACKAGE_RPM||f==UMI_DR_PACKAGE_APPIMAGE;if(p==UMI_DR_PLATFORM_MACOS)return f==UMI_DR_PACKAGE_DMG||f==UMI_DR_PACKAGE_PKG;return false;}
