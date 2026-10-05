/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_debug_bridge.c
 *
 * PURPOSE:
 *   Implement deterministic remote debug bridge validation and identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_debug_bridge.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise terminal remote remote debug bridge from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_debug_bridge_init(UmiTerminalRemoteRemoteDebugBridge *value,const char *left_id,const char *right_id) { /* Use the stable identifier comparison to choose the matching record or policy. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Use the stable identifier comparison to choose the matching record or policy. */ if(left_id) (void)umi_terminal_remote_copy_text(value->left_id,sizeof(value->left_id),left_id); /* Use the stable identifier comparison to choose the matching record or policy. */ if(right_id) (void)umi_terminal_remote_copy_text(value->right_id,sizeof(value->right_id),right_id); value->revision=1U; value->enabled=true; }
/*
 * Check that terminal remote remote debug bridge satisfies its contract before another
 * service relies on it.
 */
bool umi_terminal_remote_remote_debug_bridge_valid(const UmiTerminalRemoteRemoteDebugBridge *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->left_id, '\0', sizeof(value->left_id)) == NULL) return 0;
    if (memchr(value->right_id, '\0', sizeof(value->right_id)) == NULL) return 0;
 return value && value->enabled && value->left_id[0]!='\0' && value->right_id[0]!='\0' && strcmp(value->left_id,value->right_id)!=0; }
/*
 * Provide the terminal remote remote debug bridge fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_terminal_remote_remote_debug_bridge_fingerprint(const UmiTerminalRemoteRemoteDebugBridge *value) { /* Use the stable identifier comparison to choose the matching record or policy. */ if(!umi_terminal_remote_remote_debug_bridge_valid(value)) return 0U; return umi_terminal_remote_fingerprint_text(value->left_id) ^ (umi_terminal_remote_fingerprint_text(value->right_id)<<1U); }

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTerminalRemoteRemoteDebugBridgeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7612738e8345a02b);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteRemoteDebugBridge *)0)->left_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiTerminalRemoteRemoteDebugBridge *)0)->right_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTerminalRemoteRemoteDebugBridgeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTerminalRemoteRemoteDebugBridge *)0)->left_id) - 1U +
        8U + sizeof(((UmiTerminalRemoteRemoteDebugBridge *)0)->right_id) - 1U +
        8U +
        8U;
}
static void UmiTerminalRemoteRemoteDebugBridgeArchiveWrite(UmiArchiveWriter *writer, const UmiTerminalRemoteRemoteDebugBridge *value)
{
    UmiArchiveWriteText(writer, value->left_id, sizeof(value->left_id));
    UmiArchiveWriteText(writer, value->right_id, sizeof(value->right_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiTerminalRemoteRemoteDebugBridgeArchiveRead(UmiArchiveReader *reader, UmiTerminalRemoteRemoteDebugBridge *value)
{
    UmiArchiveReadText(reader, value->left_id, sizeof(value->left_id));
    UmiArchiveReadText(reader, value->right_id, sizeof(value->right_id));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTerminalRemoteRemoteDebugBridgeArchiveValidate(const UmiTerminalRemoteRemoteDebugBridge *value)
{
    return umi_terminal_remote_remote_debug_bridge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_terminal_remote_remote_debug_bridge_archive_encode, umi_terminal_remote_remote_debug_bridge_archive_decode,
    UmiTerminalRemoteRemoteDebugBridge, UmiTerminalRemoteRemoteDebugBridgeArchiveSchema, UmiTerminalRemoteRemoteDebugBridgeArchiveBound, UmiTerminalRemoteRemoteDebugBridgeArchiveWrite, UmiTerminalRemoteRemoteDebugBridgeArchiveRead, UmiTerminalRemoteRemoteDebugBridgeArchiveValidate)
