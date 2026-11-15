/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_toolchain_declarations.c
 *
 * PURPOSE:
 *   Check nonempty SDK evidence and exact toolchain field differences without inferring binary compatibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk/toolchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiSdkToolchainEvidence producer = {"GNU", "14.1", "x86_64", 8U};
    UmiSdkToolchainEvidence consumer = producer;
    UmiSdkToolchainComparison compared;
    unsigned char before[sizeof compared];
    memset(&compared, 0xa5, sizeof compared);
    memcpy(before, &compared, sizeof compared);
    if (strcmp(mode, "validate-null") == 0) {
        CHECK(umi_sdk_toolchain_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strncmp(mode, "null-", 5U) == 0 || strncmp(mode, "empty-", 6U) == 0 ||
               strcmp(mode, "zero-pointer") == 0) {
        const char *invalid = strncmp(mode, "null-", 5U) == 0 ? NULL : "";
        if (strcmp(mode, "null-compiler") == 0 || strcmp(mode, "empty-compiler") == 0)
            producer.compiler_id = invalid;
        else if (strcmp(mode, "null-version") == 0 || strcmp(mode, "empty-version") == 0)
            producer.compiler_version = invalid;
        else if (strcmp(mode, "null-architecture") == 0 || strcmp(mode, "empty-architecture") == 0)
            producer.architecture = invalid;
        else { CHECK(strcmp(mode, "zero-pointer") == 0); producer.pointer_size = 0U; }
        CHECK(umi_sdk_toolchain_validate(&producer) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_sdk_toolchain_compare(&producer, &consumer, &compared) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &compared, sizeof compared) == 0);
        CHECK(umi_sdk_toolchain_compare(&consumer, &producer, &compared) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &compared, sizeof compared) == 0);
    } else if (strcmp(mode, "compare-null-output") == 0) {
        CHECK(umi_sdk_toolchain_compare(&producer, &consumer, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "compare-null-producer") == 0 || strcmp(mode, "compare-null-consumer") == 0) {
        CHECK(umi_sdk_toolchain_compare(strcmp(mode, "compare-null-producer") == 0 ? NULL : &producer,
            strcmp(mode, "compare-null-consumer") == 0 ? NULL : &consumer, &compared) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &compared, sizeof compared) == 0);
    } else {
        uint32_t expected = 0U;
        if (strcmp(mode, "different-compiler") == 0 || strcmp(mode, "compiler-case") == 0) {
            consumer.compiler_id = strcmp(mode, "compiler-case") == 0 ? "gnu" : "Clang";
            expected = UMI_SDK_TOOLCHAIN_DIFFERENT_COMPILER;
        } else if (strcmp(mode, "different-version") == 0 || strcmp(mode, "version-spelling") == 0) {
            consumer.compiler_version = strcmp(mode, "version-spelling") == 0 ? "14.1.0" : "15.0";
            expected = UMI_SDK_TOOLCHAIN_DIFFERENT_VERSION;
        } else if (strcmp(mode, "different-architecture") == 0 || strcmp(mode, "architecture-alias") == 0) {
            consumer.architecture = strcmp(mode, "architecture-alias") == 0 ? "amd64" : "aarch64";
            expected = UMI_SDK_TOOLCHAIN_DIFFERENT_ARCHITECTURE;
        } else if (strcmp(mode, "different-pointer") == 0) {
            consumer.pointer_size = 4U;
            expected = UMI_SDK_TOOLCHAIN_DIFFERENT_POINTER_SIZE;
        } else if (strcmp(mode, "all-different") == 0) {
            consumer = (UmiSdkToolchainEvidence){"Clang", "19", "riscv32", 4U};
            expected = UMI_SDK_TOOLCHAIN_DIFFERENT_COMPILER | UMI_SDK_TOOLCHAIN_DIFFERENT_VERSION |
                UMI_SDK_TOOLCHAIN_DIFFERENT_ARCHITECTURE | UMI_SDK_TOOLCHAIN_DIFFERENT_POINTER_SIZE;
        } else if (strcmp(mode, "opaque") == 0) {
            /* Unrecognized declarations are data, not discovered capabilities.
             * Exact comparison must not invent an architecture or pointer ABI. */
            producer = (UmiSdkToolchainEvidence){"Vendor C", "nightly", "Experimental CPU", 12U};
            consumer = producer;
        } else CHECK(strcmp(mode, "match") == 0 || strcmp(mode, "same-object") == 0 ||
                     strcmp(mode, "owned-result") == 0);
        CHECK(umi_sdk_toolchain_validate(&producer) == UMI_STATUS_OK);
        CHECK(umi_sdk_toolchain_validate(&consumer) == UMI_STATUS_OK);
        CHECK(umi_sdk_toolchain_compare(&producer,
            strcmp(mode, "same-object") == 0 ? &producer : &consumer, &compared) == UMI_STATUS_OK);
        CHECK(compared.differing_fields == expected && compared.declarations_match == (expected == 0U));
        if (strcmp(mode, "owned-result") == 0) {
            consumer.architecture = "changed after comparison";
            consumer.pointer_size = 4U;
            CHECK(compared.declarations_match && compared.differing_fields == 0U);
            CHECK(umi_sdk_toolchain_compare(&producer, &consumer, &compared) == UMI_STATUS_OK);
            CHECK(!compared.declarations_match && compared.differing_fields ==
                (UMI_SDK_TOOLCHAIN_DIFFERENT_ARCHITECTURE | UMI_SDK_TOOLCHAIN_DIFFERENT_POINTER_SIZE));
        }
    }
    return 0;
}
