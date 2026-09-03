/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/ai/ai_chat.inc
 *
 * PURPOSE:
 *   Define the umicom.ai.chat reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.ai.chat", "ai", "AI Chat", "umicom.ai",
         "Provider-neutral conversations, tools, citations and approvals.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "assistant model chat generation"),
