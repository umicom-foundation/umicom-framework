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
/*
 * Read ct riscv isa into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_ct_riscv_isa_parse(const char *s, UmiCtRiscvIsa *out) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (s == NULL || out == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out, 0, sizeof(*out));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(s, "rv32", 4U) == 0) {
        out->xlen = 32U;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strncmp(s, "rv64", 4U) == 0) {
        out->xlen = 64U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_PARSE_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ct_copy(out->canonical, sizeof(out->canonical), s) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 4U; s[i] != '\0' && s[i] != '_'; ++i) {
        char c = (char)tolower((unsigned char)s[i]);
        /* Apply this branch only when its contract condition is satisfied. */
        if (c == 'g') {
            out->integer_base = true;
            out->multiply = true;
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_ATOMICS);
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_FLOAT);
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_DOUBLE);
            out->zicsr = true;
            out->zifencei = true;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'i') {
            out->integer_base = true;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'm') {
            out->multiply = true;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'a') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_ATOMICS);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'f') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_FLOAT);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'd') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_DOUBLE);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'c') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_COMPRESSED);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (c == 'v') {
            (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_VECTOR);
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(s, "zicsr") != NULL) {
        out->zicsr = true;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(s, "zifencei") != NULL) {
        out->zifencei = true;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(s, "zba") != NULL || strstr(s, "zbb") != NULL) {
        (void)umi_ct_cpu_feature_set_add(&out->features, UMI_CT_CPU_BITMANIP);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!out->integer_base) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
