#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSdkRuntimePlatform.cmake
#
# PURPOSE:
#   Register installed Framework SDK/runtime discovery, compatibility and
#   clean-machine validation.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
set(UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_sdk_runtime STATIC
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/types.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/installation_layout.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/component.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/component_catalogue.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/binary_manifest.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/dependency_manifest.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_manifest.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/abi_requirement.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/version_contract.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/compatibility.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/search_path.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/loader_plan.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/runtime_resolver.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/sdk_profile.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/sdk_catalogue.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/consumer_plan.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/install_probe.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_validator.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/export_target.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/cmake_package.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/runtime_environment.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/deployment_layout.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/clean_machine.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/binary_evidence.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/dependency_evidence.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/loader_evidence.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_evidence.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/runtime_policy.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/platform_profile.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/diagnostic.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/metrics.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/health.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/command.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/event.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/query.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/service.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/controller.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/runtime_file.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/runtime_file_catalogue.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/architecture.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/compiler_runtime.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/ucrt_runtime.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/resource_root.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/plugin_root.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/adapter_root.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/header_root.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/library_root.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/cmake_target_map.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_root.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/relocation.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/runtime_bundle.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/sdk_bundle.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/deployment_probe.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/consumer_evidence.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_search.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/loader_search.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/environment_probe.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/path_normalisation.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/install_receipt.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/repair_plan.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/uninstall_plan.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/upgrade_plan.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/rollback_plan.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_channel.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/package_generation.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/dependency_graph.c"
    "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/src/sdk_runtime/compatibility_matrix.c"
 )
add_library(Umicom::sdk_runtime ALIAS umicom_sdk_runtime)
set_target_properties(umicom_sdk_runtime PROPERTIES EXPORT_NAME sdk_runtime)
target_include_directories(umicom_sdk_runtime PUBLIC
    $<BUILD_INTERFACE:${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(umicom_sdk_runtime PUBLIC Umicom::base Umicom::test_runtime)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_sdk_runtime)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_sdk_runtime)
endif()
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::sdk_runtime)
endif()
if(BUILD_TESTING)
    function(umicom_add_sdk_runtime_test target test_name source)
        add_executable("${target}" "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::sdk_runtime)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES LABELS "framework;sdk-runtime")
    endfunction()
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-installation-layout-test framework.sdk_runtime.installation_layout tests/sdk_runtime/test_installation_layout.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-component-test framework.sdk_runtime.component tests/sdk_runtime/test_component.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-component-catalogue-test framework.sdk_runtime.component_catalogue tests/sdk_runtime/test_component_catalogue.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-binary-manifest-test framework.sdk_runtime.binary_manifest tests/sdk_runtime/test_binary_manifest.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-dependency-manifest-test framework.sdk_runtime.dependency_manifest tests/sdk_runtime/test_dependency_manifest.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-manifest-test framework.sdk_runtime.package_manifest tests/sdk_runtime/test_package_manifest.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-abi-requirement-test framework.sdk_runtime.abi_requirement tests/sdk_runtime/test_abi_requirement.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-version-contract-test framework.sdk_runtime.version_contract tests/sdk_runtime/test_version_contract.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-compatibility-test framework.sdk_runtime.compatibility tests/sdk_runtime/test_compatibility.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-search-path-test framework.sdk_runtime.search_path tests/sdk_runtime/test_search_path.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-loader-plan-test framework.sdk_runtime.loader_plan tests/sdk_runtime/test_loader_plan.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-runtime-resolver-test framework.sdk_runtime.runtime_resolver tests/sdk_runtime/test_runtime_resolver.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-sdk-profile-test framework.sdk_runtime.sdk_profile tests/sdk_runtime/test_sdk_profile.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-sdk-catalogue-test framework.sdk_runtime.sdk_catalogue tests/sdk_runtime/test_sdk_catalogue.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-consumer-plan-test framework.sdk_runtime.consumer_plan tests/sdk_runtime/test_consumer_plan.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-install-probe-test framework.sdk_runtime.install_probe tests/sdk_runtime/test_install_probe.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-validator-test framework.sdk_runtime.package_validator tests/sdk_runtime/test_package_validator.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-export-target-test framework.sdk_runtime.export_target tests/sdk_runtime/test_export_target.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-cmake-package-test framework.sdk_runtime.cmake_package tests/sdk_runtime/test_cmake_package.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-runtime-environment-test framework.sdk_runtime.runtime_environment tests/sdk_runtime/test_runtime_environment.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-deployment-layout-test framework.sdk_runtime.deployment_layout tests/sdk_runtime/test_deployment_layout.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-clean-machine-test framework.sdk_runtime.clean_machine tests/sdk_runtime/test_clean_machine.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-binary-evidence-test framework.sdk_runtime.binary_evidence tests/sdk_runtime/test_binary_evidence.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-dependency-evidence-test framework.sdk_runtime.dependency_evidence tests/sdk_runtime/test_dependency_evidence.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-loader-evidence-test framework.sdk_runtime.loader_evidence tests/sdk_runtime/test_loader_evidence.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-evidence-test framework.sdk_runtime.package_evidence tests/sdk_runtime/test_package_evidence.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-runtime-policy-test framework.sdk_runtime.runtime_policy tests/sdk_runtime/test_runtime_policy.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-platform-profile-test framework.sdk_runtime.platform_profile tests/sdk_runtime/test_platform_profile.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-diagnostic-test framework.sdk_runtime.diagnostic tests/sdk_runtime/test_diagnostic.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-metrics-test framework.sdk_runtime.metrics tests/sdk_runtime/test_metrics.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-health-test framework.sdk_runtime.health tests/sdk_runtime/test_health.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-command-test framework.sdk_runtime.command tests/sdk_runtime/test_command.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-event-test framework.sdk_runtime.event tests/sdk_runtime/test_event.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-query-test framework.sdk_runtime.query tests/sdk_runtime/test_query.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-service-test framework.sdk_runtime.service tests/sdk_runtime/test_service.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-controller-test framework.sdk_runtime.controller tests/sdk_runtime/test_controller.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-runtime-file-test framework.sdk_runtime.runtime_file tests/sdk_runtime/test_runtime_file.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-runtime-file-catalogue-test framework.sdk_runtime.runtime_file_catalogue tests/sdk_runtime/test_runtime_file_catalogue.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-architecture-test framework.sdk_runtime.architecture tests/sdk_runtime/test_architecture.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-compiler-runtime-test framework.sdk_runtime.compiler_runtime tests/sdk_runtime/test_compiler_runtime.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-ucrt-runtime-test framework.sdk_runtime.ucrt_runtime tests/sdk_runtime/test_ucrt_runtime.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-resource-root-test framework.sdk_runtime.resource_root tests/sdk_runtime/test_resource_root.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-plugin-root-test framework.sdk_runtime.plugin_root tests/sdk_runtime/test_plugin_root.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-adapter-root-test framework.sdk_runtime.adapter_root tests/sdk_runtime/test_adapter_root.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-header-root-test framework.sdk_runtime.header_root tests/sdk_runtime/test_header_root.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-library-root-test framework.sdk_runtime.library_root tests/sdk_runtime/test_library_root.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-cmake-target-map-test framework.sdk_runtime.cmake_target_map tests/sdk_runtime/test_cmake_target_map.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-root-test framework.sdk_runtime.package_root tests/sdk_runtime/test_package_root.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-relocation-test framework.sdk_runtime.relocation tests/sdk_runtime/test_relocation.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-runtime-bundle-test framework.sdk_runtime.runtime_bundle tests/sdk_runtime/test_runtime_bundle.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-sdk-bundle-test framework.sdk_runtime.sdk_bundle tests/sdk_runtime/test_sdk_bundle.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-deployment-probe-test framework.sdk_runtime.deployment_probe tests/sdk_runtime/test_deployment_probe.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-consumer-evidence-test framework.sdk_runtime.consumer_evidence tests/sdk_runtime/test_consumer_evidence.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-search-test framework.sdk_runtime.package_search tests/sdk_runtime/test_package_search.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-loader-search-test framework.sdk_runtime.loader_search tests/sdk_runtime/test_loader_search.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-environment-probe-test framework.sdk_runtime.environment_probe tests/sdk_runtime/test_environment_probe.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-path-normalisation-test framework.sdk_runtime.path_normalisation tests/sdk_runtime/test_path_normalisation.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-install-receipt-test framework.sdk_runtime.install_receipt tests/sdk_runtime/test_install_receipt.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-repair-plan-test framework.sdk_runtime.repair_plan tests/sdk_runtime/test_repair_plan.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-uninstall-plan-test framework.sdk_runtime.uninstall_plan tests/sdk_runtime/test_uninstall_plan.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-upgrade-plan-test framework.sdk_runtime.upgrade_plan tests/sdk_runtime/test_upgrade_plan.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-rollback-plan-test framework.sdk_runtime.rollback_plan tests/sdk_runtime/test_rollback_plan.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-channel-test framework.sdk_runtime.package_channel tests/sdk_runtime/test_package_channel.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-package-generation-test framework.sdk_runtime.package_generation tests/sdk_runtime/test_package_generation.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-dependency-graph-test framework.sdk_runtime.dependency_graph tests/sdk_runtime/test_dependency_graph.c)
    umicom_add_sdk_runtime_test(umicom-sdk-runtime-compatibility-matrix-test framework.sdk_runtime.compatibility_matrix tests/sdk_runtime/test_compatibility_matrix.c)
endif()
install(TARGETS umicom_sdk_runtime EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "${UMICOM_SDK_RUNTIME_FRAMEWORK_ROOT}/include/umicom/sdk_runtime"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom")
message(STATUS "Umicom installed SDK and runtime platform enabled")
