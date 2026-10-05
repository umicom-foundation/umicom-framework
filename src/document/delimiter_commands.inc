/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/delimiter_commands.inc
 * PURPOSE: Expose shared delimiter navigation through the existing command catalogue.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/delimiter_navigation.h"
static UmiStatus DelimiterCommand(void *context, const char *argument, char *message, size_t capacity,
                                  UmiDocumentDelimiterAction action)
{
    /* These commands act on the current caret and take no external location
     * text. Reject unexpected input instead of guessing another document. */
    UmiStatus status = argument != NULL && argument[0] != '\0'
                           ? UMI_STATUS_INVALID_ARGUMENT
                           : UmiDocumentCoordinatorNavigateActiveDelimiter(context, action);
    if (status == UMI_STATUS_NOT_FOUND && message != NULL && capacity != 0U)
        (void)snprintf(
            message, capacity,
            "No complete bracket pair at this caret. Select a bracket or move inside a balanced pair.");
    else
        command_message(message, capacity, "Source bracket location selected", status);
    return status;
}
static UmiStatus MatchDelimiterCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return DelimiterCommand(context, argument, message, capacity, UMI_DOCUMENT_DELIMITER_MATCH);
}
static UmiStatus SelectDelimiterContentCommand(void *context, const char *argument, char *message,
                                               size_t capacity)
{
    return DelimiterCommand(context, argument, message, capacity, UMI_DOCUMENT_DELIMITER_CONTENTS);
}
static UmiStatus SelectDelimiterPairCommand(void *context, const char *argument, char *message,
                                            size_t capacity)
{
    return DelimiterCommand(context, argument, message, capacity, UMI_DOCUMENT_DELIMITER_PAIR);
}
