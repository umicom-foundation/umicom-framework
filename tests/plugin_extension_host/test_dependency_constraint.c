/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_dependency_constraint.c
 *
 * PURPOSE:
 *   Exercise describe one extension dependency version/capability constraint.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/dependency_constraint.h"
int main(void)
{
    UmiPluginExtensionHostDependencyConstraint value; umi_plugin_extension_host_dependency_constraint_init(&value);
    if (umi_plugin_extension_host_dependency_constraint_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_dependency_constraint_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_dependency_constraint_fingerprint(&value) == 0U) return 3;
    return 0;
}
