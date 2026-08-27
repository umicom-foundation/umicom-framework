/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/types.h
 *
 * PURPOSE:
 *   Define bounded Model Context Protocol records without replacing existing AI tool, approval or policy contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_TYPES_H
#define UMICOM_AI_MCP_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_MCP_TEXT_CAPACITY 32768U
#define UMI_AI_MCP_SCHEMA_CAPACITY 8192U
#define UMI_AI_MCP_MAX_SERVERS 16U
#define UMI_AI_MCP_MAX_TOOLS UMI_AI_MAX_TOOLS
#define UMI_AI_MCP_MAX_RESOURCES 64U
#define UMI_AI_MCP_MAX_PROMPTS 32U
#define UMI_AI_MCP_MAX_TOOL_BINDINGS UMI_AI_MAX_TOOLS

typedef enum UmiAiMcpSessionState {
    UMI_AI_MCP_SESSION_DISCONNECTED = 0,
    UMI_AI_MCP_SESSION_CONNECTED = 1,
    UMI_AI_MCP_SESSION_INITIALIZED = 2,
    UMI_AI_MCP_SESSION_READY = 3,
    UMI_AI_MCP_SESSION_FAILED = 4
} UmiAiMcpSessionState;

typedef enum UmiAiMcpTrust {
    UMI_AI_MCP_TRUST_UNTRUSTED = 0,
    UMI_AI_MCP_TRUST_LOCAL = 1,
    UMI_AI_MCP_TRUST_VERIFIED = 2
} UmiAiMcpTrust;

enum {
    UMI_AI_MCP_CAPABILITY_TOOLS = 1U << 0,
    UMI_AI_MCP_CAPABILITY_RESOURCES = 1U << 1,
    UMI_AI_MCP_CAPABILITY_PROMPTS = 1U << 2,
    UMI_AI_MCP_CAPABILITY_LOGGING = 1U << 3
};

enum {
    UMI_AI_MCP_TOOL_FLAG_READ_ONLY = 1U << 0,
    UMI_AI_MCP_TOOL_FLAG_SIDE_EFFECT = 1U << 1,
    UMI_AI_MCP_TOOL_FLAG_NETWORK = 1U << 2,
    UMI_AI_MCP_TOOL_FLAG_FILESYSTEM = 1U << 3
};

typedef struct UmiAiMcpServerDescriptor {
    char server_id[UMI_AI_ID_CAPACITY];
    char display_name[UMI_AI_SMALL_TEXT_CAPACITY];
    char endpoint[UMI_AI_SMALL_TEXT_CAPACITY];
    char permission_prefix[UMI_AI_ID_CAPACITY];
    UmiAiMcpTrust trust;
    uint32_t expected_capabilities;
    int enabled;
} UmiAiMcpServerDescriptor;

typedef struct UmiAiMcpServerCapabilities {
    uint32_t flags;
    char protocol_version[64];
    char server_name[UMI_AI_SMALL_TEXT_CAPACITY];
    char server_version[128];
} UmiAiMcpServerCapabilities;

typedef struct UmiAiMcpToolDescriptor {
    char name[UMI_AI_ID_CAPACITY];
    char description[UMI_AI_SMALL_TEXT_CAPACITY];
    char input_schema[UMI_AI_MCP_SCHEMA_CAPACITY];
    char permission[UMI_AI_ID_CAPACITY];
    uint32_t flags;
} UmiAiMcpToolDescriptor;

typedef struct UmiAiMcpResourceDescriptor {
    char uri[UMI_AI_TEXT_CAPACITY];
    char name[UMI_AI_SMALL_TEXT_CAPACITY];
    char description[UMI_AI_SMALL_TEXT_CAPACITY];
    char mime_type[128];
} UmiAiMcpResourceDescriptor;

typedef struct UmiAiMcpPromptDescriptor {
    char name[UMI_AI_ID_CAPACITY];
    char description[UMI_AI_SMALL_TEXT_CAPACITY];
    char arguments_schema[UMI_AI_MCP_SCHEMA_CAPACITY];
} UmiAiMcpPromptDescriptor;

#ifdef __cplusplus
}
#endif
#endif
