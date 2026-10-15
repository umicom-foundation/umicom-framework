/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/installed_metadata.h
 * PURPOSE: Read installed SDK identity and layout as bounded data, without evaluating CMake.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_SDK_INSTALLED_METADATA_H
#define UMICOM_SDK_INSTALLED_METADATA_H
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/platform/path.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SDK_METADATA_BYTE_LIMIT 16384U
#define UMI_SDK_METADATA_RELATIVE_PATH "share/umicom/framework/sdk.properties"

/** Owned metadata describes the selected installation, never the inspecting
 * process. It is not authenticated and does not prove binary compatibility. */
typedef struct UmiSdkInstalledMetadata {
    UmiVersion framework_version;
    uint32_t framework_abi;
    char include_directory[UMI_PATH_CAPACITY];
    char library_directory[UMI_PATH_CAPACITY];
    char base_library[UMI_PATH_CAPACITY];
} UmiSdkInstalledMetadata;

/** Parse the bounded key=value installation record, accepting LF or CRLF.
 * All six keys must occur exactly once. Unknown keys, controls, numeric overflow,
 * absolute paths and parent traversal are rejected. Outputs clear on failure;
 * input and output must not overlap. No filesystem or CMake operation occurs. */
UmiStatus UmiSdkInstalledMetadataParse(const void *bytes, size_t length,
    UmiSdkInstalledMetadata *out_metadata);

/** Read the metadata at the fixed SDK-relative path in an explicit absolute
 * prefix. Uses Framework's bounded regular-file reader. Parents are trusted;
 * this is not a sandbox against concurrent filesystem changes. Clear on failure. */
UmiStatus UmiSdkInstalledMetadataLoad(const char *prefix, UmiSdkInstalledMetadata *out_metadata);
#ifdef __cplusplus
}
#endif
#endif
