/* Umicom Framework | Compiler target triples | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_TARGET_H
#define UMICOM_COMPILER_TARGET_H
#include "umicom/compiler/common.h"
typedef struct UmiCompilerTarget {
    char triple[UMI_COMPILER_NAME_CAPACITY];
    char architecture[64U];
    char vendor[64U];
    char operating_system[64U];
    char environment[64U];
    UmiCompilerObjectFormat object_format;
    uint32_t pointer_width;
    bool little_endian;
} UmiCompilerTarget;
UmiStatus umi_compiler_target_parse(const char *triple,UmiCompilerTarget *out_target);
UmiStatus umi_compiler_target_host(UmiCompilerTarget *out_target);
bool umi_compiler_target_compatible(const UmiCompilerTarget *left,const UmiCompilerTarget *right);
const char *umi_compiler_object_format_name(UmiCompilerObjectFormat format);
#endif
