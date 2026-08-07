#ifndef UMICOM_UI_CONTRACTS_H
#define UMICOM_UI_CONTRACTS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiPanelKind {
    UMI_PANEL_PROJECT = 1,
    UMI_PANEL_EDITOR = 2,
    UMI_PANEL_OUTPUT = 3,
    UMI_PANEL_PROBLEMS = 4,
    UMI_PANEL_CHAT = 5
} UmiPanelKind;

typedef struct UmiWorkbenchState {
    const char *title;
    const char *status_text;
    const char *active_document;
    const char *output_text;
    int workspace_trusted;
} UmiWorkbenchState;

#ifdef __cplusplus
}
#endif

#endif
