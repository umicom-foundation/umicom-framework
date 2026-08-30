/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/cross_target/riscv_isa.c
 *
 * PURPOSE:
 *   Parse RISC-V ISA strings into XLEN and stable capability masks without depending on compiler command lines.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/cross_target/riscv_isa.h"

#include <ctype.h>
#include <string.h>
UmiStatus umi_ct_riscv_isa_parse(const char *s, UmiCtRiscvIsa *out) {
    size_t i;
    if (s == NULL || out == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out, 0, sizeof(*out));
    if (strncmp(s, "rv32", 4U) == 0) {
        out->xlen = 32U;
    } else if (strncmp(s, "rv64", 4U) == 0) {
        out->xlen = 64U;
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }
    if (umi_ct_copy(out->canonical, sizeof(out->canonical), s) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (i = 4U; s[i] != '\0' && s[i] != '_'; ++i) {
        char c = (char)tolower((unsigned char)s[i]);
        if (c == 'g') {
            out->integer_base = true;
            out->multiply = true;
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_ATOMICS);
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_FLOAT);
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_DOUBLE);
            out->zicsr = true;
            out->zifencei = true;
        } else if (c == 'i') {
            out->integer_base = true;
        } else if (c == 'm') {
            out->multiply = true;
        } else if (c == 'a') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_ATOMICS);
        } else if (c == 'f') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_FLOAT);
        } else if (c == 'd') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_DOUBLE);
        } else if (c == 'c') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_COMPRESSED);
        } else if (c == 'v') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_VECTOR);
        }
    }
    if (strstr(s, "zicsr") != NULL) {
        out->zicsr = true;
    }
    if (strstr(s, "zifencei") != NULL) {
        out->zifencei = true;
    }
    if (strstr(s, "zba") != NULL || strstr(s, "zbb") != NULL) {
        (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_BITMANIP);
    }
    if (!out->integer_base) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
