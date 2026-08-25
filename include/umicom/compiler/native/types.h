/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/types.h
 *
 * PURPOSE:
 *   Define stable constants, enums and utility helpers for the native Umicom compiler pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_TYPES_H
#define UMICOM_COMPILER_NATIVE_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NATIVE_COMPILER_API_VERSION 1U
#define UMI_NC_ID_CAPACITY 96U
#define UMI_NC_NAME_CAPACITY 128U
#define UMI_NC_TEXT_CAPACITY 512U
#define UMI_NC_PATH_CAPACITY 512U
#define UMI_NC_TOKEN_TEXT_CAPACITY 128U
#define UMI_NC_MAX_TOKENS 1024U
#define UMI_NC_MAX_AST_NODES 512U
#define UMI_NC_MAX_SYMBOLS 256U
#define UMI_NC_MAX_SCOPES 64U
#define UMI_NC_MAX_IR_INSTRUCTIONS 64U
#define UMI_NC_MAX_IR_BLOCKS 32U
#define UMI_NC_MAX_IR_FUNCTIONS 8U
#define UMI_NC_MAX_MACHINE_INSTRUCTIONS 64U
#define UMI_NC_MAX_MACHINE_BLOCKS 32U

typedef enum UmiNativeTokenKind { UMI_NC_TOKEN_EOF=0, UMI_NC_TOKEN_IDENTIFIER=1, UMI_NC_TOKEN_KEYWORD=2, UMI_NC_TOKEN_NUMBER=3, UMI_NC_TOKEN_STRING=4, UMI_NC_TOKEN_CHARACTER=5, UMI_NC_TOKEN_PUNCTUATION=6, UMI_NC_TOKEN_COMMENT=7, UMI_NC_TOKEN_ERROR=8 } UmiNativeTokenKind;
typedef enum UmiNativeAstKind { UMI_NC_AST_INVALID=0, UMI_NC_AST_TRANSLATION_UNIT=1, UMI_NC_AST_FUNCTION=2, UMI_NC_AST_DECLARATION=3, UMI_NC_AST_COMPOUND=4, UMI_NC_AST_RETURN=5, UMI_NC_AST_BINARY=6, UMI_NC_AST_UNARY=7, UMI_NC_AST_CALL=8, UMI_NC_AST_IDENTIFIER=9, UMI_NC_AST_LITERAL=10 } UmiNativeAstKind;
typedef enum UmiNativeTypeKind { UMI_NC_TYPE_INVALID=0, UMI_NC_TYPE_VOID=1, UMI_NC_TYPE_BOOL=2, UMI_NC_TYPE_CHAR=3, UMI_NC_TYPE_INT=4, UMI_NC_TYPE_UINT=5, UMI_NC_TYPE_LONG=6, UMI_NC_TYPE_ULONG=7, UMI_NC_TYPE_FLOAT=8, UMI_NC_TYPE_DOUBLE=9, UMI_NC_TYPE_POINTER=10, UMI_NC_TYPE_ARRAY=11, UMI_NC_TYPE_FUNCTION=12, UMI_NC_TYPE_STRUCT=13, UMI_NC_TYPE_UNION=14, UMI_NC_TYPE_ENUM=15 } UmiNativeTypeKind;
typedef enum UmiNativeIrTypeKind { UMI_NC_IR_VOID=0, UMI_NC_IR_I1=1, UMI_NC_IR_I8=2, UMI_NC_IR_I16=3, UMI_NC_IR_I32=4, UMI_NC_IR_I64=5, UMI_NC_IR_F32=6, UMI_NC_IR_F64=7, UMI_NC_IR_PTR=8 } UmiNativeIrTypeKind;
typedef enum UmiNativeIrOpcode { UMI_NC_IR_NOP=0, UMI_NC_IR_CONST=1, UMI_NC_IR_ADD=2, UMI_NC_IR_SUB=3, UMI_NC_IR_MUL=4, UMI_NC_IR_DIV=5, UMI_NC_IR_MOD=6, UMI_NC_IR_AND=7, UMI_NC_IR_OR=8, UMI_NC_IR_XOR=9, UMI_NC_IR_SHL=10, UMI_NC_IR_SHR=11, UMI_NC_IR_CMP_EQ=12, UMI_NC_IR_CMP_NE=13, UMI_NC_IR_CMP_LT=14, UMI_NC_IR_CMP_LE=15, UMI_NC_IR_CMP_GT=16, UMI_NC_IR_CMP_GE=17, UMI_NC_IR_LOAD=18, UMI_NC_IR_STORE=19, UMI_NC_IR_ALLOCA=20, UMI_NC_IR_PHI=21, UMI_NC_IR_CALL=22, UMI_NC_IR_BR=23, UMI_NC_IR_CBR=24, UMI_NC_IR_RET=25, UMI_NC_IR_COPY=26 } UmiNativeIrOpcode;
typedef enum UmiNativeMachineArch { UMI_NC_ARCH_UNKNOWN=0, UMI_NC_ARCH_X86_64=1, UMI_NC_ARCH_RISCV64=2 } UmiNativeMachineArch;
typedef enum UmiNativeObjectFormat { UMI_NC_OBJECT_UNKNOWN=0, UMI_NC_OBJECT_ELF=1, UMI_NC_OBJECT_COFF=2, UMI_NC_OBJECT_MACHO=3 } UmiNativeObjectFormat;
typedef enum UmiNativeCompileStage { UMI_NC_STAGE_SOURCE=0, UMI_NC_STAGE_LEX=1, UMI_NC_STAGE_PREPROCESS=2, UMI_NC_STAGE_PARSE=3, UMI_NC_STAGE_SEMANTIC=4, UMI_NC_STAGE_IR=5, UMI_NC_STAGE_OPTIMIZE=6, UMI_NC_STAGE_SELECT=7, UMI_NC_STAGE_ASSEMBLY=8, UMI_NC_STAGE_OBJECT=9, UMI_NC_STAGE_LINK=10, UMI_NC_STAGE_COMPLETE=11 } UmiNativeCompileStage;

typedef struct UmiNativeSourceSpan { uint32_t file_id; size_t offset; size_t length; uint32_t line; uint32_t column; } UmiNativeSourceSpan;
UmiStatus umi_nc_copy_text(char *destination,size_t capacity,const char *source);
uint64_t umi_nc_hash_bytes(const void *data,size_t size);
uint64_t umi_nc_hash_text(const char *text);
const char *umi_nc_token_kind_name(UmiNativeTokenKind kind);
const char *umi_nc_stage_name(UmiNativeCompileStage stage);
#ifdef __cplusplus
}
#endif
#endif
