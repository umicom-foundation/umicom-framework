/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/installation_inspection.h
 * PURPOSE: Observe the files named by a selected SDK installation receipt.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_SDK_INSTALLATION_INSPECTION_H
#define UMICOM_SDK_INSTALLATION_INSPECTION_H
#include "umicom/sdk/installed_metadata.h"
#ifdef __cplusplus
extern "C" {
#endif
/** File-presence evidence remains distinct from compiling, linking or running a
 * consumer. Paths are owned normalized copies beneath the requested prefix. */
typedef struct UmiSdkInstallationObservation {
    UmiSdkInstalledMetadata metadata;
    char prefix[UMI_PATH_CAPACITY];
    char package_directory[UMI_PATH_CAPACITY];
    char public_header[UMI_PATH_CAPACITY];
    char base_library[UMI_PATH_CAPACITY];
    int package_config;
    int package_targets;
    int package_version;
    int header_present;
    int base_library_present;
    int complete;
} UmiSdkInstallationObservation;

/** Inspect only the explicit absolute SDK prefix; never search other locations.
 * A missing or unreadable receipt fails and clears output. With valid metadata,
 * return OK and separate presence flags, even if files are missing. Presence is
 * not authentication, ABI proof, component completeness or consumer qualification.
 * Run on a worker and select a trusted installation; no package code is executed. */
UmiStatus UmiSdkInstallationInspect(const char *prefix, UmiSdkInstallationObservation *out_observation);
#ifdef __cplusplus
}
#endif
#endif
