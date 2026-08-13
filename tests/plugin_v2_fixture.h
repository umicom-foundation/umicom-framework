/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/plugin_v2_fixture.h
 *
 * PURPOSE:
 *   Share deterministic Extension SDK v2 fixtures across focused tests.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_TESTS_PLUGIN_V2_FIXTURE_H
#define UMICOM_TESTS_PLUGIN_V2_FIXTURE_H

#include <stdio.h>
#include <string.h>
#include "umicom/umicom.h"

static inline UmiVersion plugin_test_version(unsigned int major,
                                             unsigned int minor,
                                             unsigned int patch)
{
    UmiVersion version;
    version.major = (uint16_t)major;
    version.minor = (uint16_t)minor;
    version.patch = (uint16_t)patch;
    return version;
}

static inline UmiPluginManifest plugin_test_manifest(const char *id,
                                                     const char *name)
{
    UmiPluginManifest manifest;
    (void)memset(&manifest, 0, sizeof(manifest));
    (void)snprintf(manifest.plugin_id, sizeof(manifest.plugin_id), "%s", id);
    (void)snprintf(manifest.display_name, sizeof(manifest.display_name), "%s", name);
    (void)snprintf(manifest.library_path, sizeof(manifest.library_path),
                   "%s.dll", id);
    manifest.version = plugin_test_version(1U, 2U, 0U);
    manifest.required_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    return manifest;
}

#endif
