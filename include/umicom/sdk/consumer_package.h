/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/consumer_package.h
 *
 * PURPOSE:
 *   Describe and render the CMake package contract used by SDK consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the sdk consumer package data shared with callers of this public contract.
 */
typedef struct UmiSdkConsumerPackage {
    char package_name[UMI_SDK_EXPORT_ID_CAPACITY];
    char namespace_name[UMI_SDK_EXPORT_ID_CAPACITY];
    char minimum_version[64];
    char config_directory[UMI_SDK_PACKAGE_PATH_CAPACITY];
    int include_examples;
    int include_toolchain_file;
} UmiSdkConsumerPackage;

/**
 * Initialise sdk consumer package from caller-provided values so later operations receive
 * a known state.
 */
/** Publish a complete inline package only after all input copies succeed.
 * Failure, including allocation failure, leaves every destination byte unchanged.
 * Input strings must be readable and NUL-terminated; they may borrow old fields.
 * Defaults retain enabled example and toolchain-file inclusion. Serialize access.
 */
UmiStatus umi_sdk_consumer_package_init(UmiSdkConsumerPackage *package,
                                            const char *package_name,
                                            const char *namespace_name,
                                            const char *minimum_version,
                                            const char *config_directory);
/**
 * Check that sdk consumer package satisfies its contract before another service relies on
 * it.
 */
/** Validate nonempty, NUL-terminated text within every fixed field capacity.
 * This structural check does not validate CMake argument syntax.
 */
UmiStatus umi_sdk_consumer_package_validate(
    const UmiSdkConsumerPackage *package);
/**
 * Provide the sdk consumer package render cmake operation used by this module and its
 * client applications.
 */
/** Measure complete generated text including its terminating NUL, without
 * allocation or output generation. Refusal leaves out_bytes unchanged.
 *
 * Emitted package/target names use ASCII letters, digits and underscore at each
 * initial segment, followed by those characters, dot, plus or hyphen. Target
 * segments may be separated by a double colon; empty segments are refused.
 * Link-command keywords (PRIVATE, PUBLIC, INTERFACE, LINK_PRIVATE, LINK_PUBLIC,
 * LINK_INTERFACE_LIBRARIES, debug, optimized, general) are not target arguments.
 * Version is one to four nonempty decimal components separated by single dots.
 * No whitespace, lists, variables, generator expressions, paths or flags are
 * accepted in these emitted fields. Other model fields are structurally checked
 * but not emitted here. This is a deliberate literal subset, not a CMake parser.
 *
 * Inputs must remain stable, and out_bytes must not overlap them. Valid names
 * do not prove installed target presence or make package scripts trustworthy.
 */
UmiStatus umi_sdk_consumer_package_cmake_size(const UmiSdkConsumerPackage *package,
    const UmiSdkExportCatalogue *catalogue, size_t *out_bytes);

/** Render complete text or preserve every existing output byte on failure.
 * Uses the size query's literal-argument policy. Invalid fields return
 * INVALID_ARGUMENT, insufficient output returns CAPACITY_EXCEEDED, and temporary
 * allocation refusal returns OUT_OF_MEMORY. Output is separate caller-owned
 * storage. Bytes after the accepted terminator are unchanged. No file is written
 * and no CMake command, compiler, library loader or application is executed.
 * Review and trust the selected package before executing its configuration.
 */
UmiStatus umi_sdk_consumer_package_render_cmake(
    const UmiSdkConsumerPackage *package,
    const UmiSdkExportCatalogue *catalogue,
    char *output,
    size_t output_capacity);

#ifdef __cplusplus
}
#endif
#endif
