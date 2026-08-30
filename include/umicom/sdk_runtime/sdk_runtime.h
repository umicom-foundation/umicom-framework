/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/sdk_runtime.h
 *
 * PURPOSE:
 *   Expose installed SDK/runtime discovery, validation and clean-machine contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SDK_RUNTIME_H
#define UMICOM_SDK_RUNTIME_SDK_RUNTIME_H

#include "umicom/sdk_runtime/types.h"
#include "umicom/sdk_runtime/installation_layout.h"
#include "umicom/sdk_runtime/component.h"
#include "umicom/sdk_runtime/component_catalogue.h"
#include "umicom/sdk_runtime/binary_manifest.h"
#include "umicom/sdk_runtime/dependency_manifest.h"
#include "umicom/sdk_runtime/package_manifest.h"
#include "umicom/sdk_runtime/abi_requirement.h"
#include "umicom/sdk_runtime/version_contract.h"
#include "umicom/sdk_runtime/compatibility.h"
#include "umicom/sdk_runtime/search_path.h"
#include "umicom/sdk_runtime/loader_plan.h"
#include "umicom/sdk_runtime/runtime_resolver.h"
#include "umicom/sdk_runtime/sdk_profile.h"
#include "umicom/sdk_runtime/sdk_catalogue.h"
#include "umicom/sdk_runtime/consumer_plan.h"
#include "umicom/sdk_runtime/install_probe.h"
#include "umicom/sdk_runtime/package_validator.h"
#include "umicom/sdk_runtime/export_target.h"
#include "umicom/sdk_runtime/cmake_package.h"
#include "umicom/sdk_runtime/runtime_environment.h"
#include "umicom/sdk_runtime/deployment_layout.h"
#include "umicom/sdk_runtime/clean_machine.h"
#include "umicom/sdk_runtime/binary_evidence.h"
#include "umicom/sdk_runtime/dependency_evidence.h"
#include "umicom/sdk_runtime/loader_evidence.h"
#include "umicom/sdk_runtime/package_evidence.h"
#include "umicom/sdk_runtime/runtime_policy.h"
#include "umicom/sdk_runtime/platform_profile.h"
#include "umicom/sdk_runtime/diagnostic.h"
#include "umicom/sdk_runtime/metrics.h"
#include "umicom/sdk_runtime/health.h"
#include "umicom/sdk_runtime/command.h"
#include "umicom/sdk_runtime/event.h"
#include "umicom/sdk_runtime/query.h"
#include "umicom/sdk_runtime/service.h"
#include "umicom/sdk_runtime/controller.h"

#include "umicom/sdk_runtime/runtime_file.h"
#include "umicom/sdk_runtime/runtime_file_catalogue.h"
#include "umicom/sdk_runtime/architecture.h"
#include "umicom/sdk_runtime/compiler_runtime.h"
#include "umicom/sdk_runtime/ucrt_runtime.h"
#include "umicom/sdk_runtime/resource_root.h"
#include "umicom/sdk_runtime/plugin_root.h"
#include "umicom/sdk_runtime/adapter_root.h"
#include "umicom/sdk_runtime/header_root.h"
#include "umicom/sdk_runtime/library_root.h"
#include "umicom/sdk_runtime/cmake_target_map.h"
#include "umicom/sdk_runtime/package_root.h"
#include "umicom/sdk_runtime/relocation.h"
#include "umicom/sdk_runtime/runtime_bundle.h"
#include "umicom/sdk_runtime/sdk_bundle.h"
#include "umicom/sdk_runtime/deployment_probe.h"
#include "umicom/sdk_runtime/consumer_evidence.h"
#include "umicom/sdk_runtime/package_search.h"
#include "umicom/sdk_runtime/loader_search.h"
#include "umicom/sdk_runtime/environment_probe.h"
#include "umicom/sdk_runtime/path_normalisation.h"
#include "umicom/sdk_runtime/install_receipt.h"
#include "umicom/sdk_runtime/repair_plan.h"
#include "umicom/sdk_runtime/uninstall_plan.h"
#include "umicom/sdk_runtime/upgrade_plan.h"
#include "umicom/sdk_runtime/rollback_plan.h"
#include "umicom/sdk_runtime/package_channel.h"
#include "umicom/sdk_runtime/package_generation.h"
#include "umicom/sdk_runtime/dependency_graph.h"
#include "umicom/sdk_runtime/compatibility_matrix.h"

#endif
