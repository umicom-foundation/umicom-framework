/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/build_configurations.h
 * PURPOSE: Bind named project settings to the existing native Build Settings form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_BUILD_CONFIGURATIONS_H
#define UMICOM_UI_GTK4_BUILD_CONFIGURATIONS_H
#include "umicom/build/configuration_library.h"
#include "umicom/ui/gtk4/developer_dialog.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* Callbacks run synchronously on GTK's owning thread and borrow their inputs.
 * The host rechecks workspace identity and job ownership on every call. The
 * form owns copied results, but the host retains context until dialog destroy. */
    typedef struct UmiGtk4BuildConfigurationCallbacks
    {
        UmiStatus (*capture)(const char *root, UmiBuildConfigurationCatalogue *out, void *context);
        UmiStatus (*load)(const char *root, const char *name, uint64_t revision,
                          UmiBuildProfile *out, void *context);
        UmiStatus (*save)(const char *name, const UmiBuildProfile *profile, uint64_t revision,
                          uint64_t *out_revision, void *context);
        void *context;
    } UmiGtk4BuildConfigurationCallbacks;
    /** Attach controls once to a visible Build Settings form. Nothing is loaded or
 * saved until the user clicks an action. Loading copies settings into the form
 * for review; only the existing Apply Settings action can activate them.
 * Saving requires explicit replacement consent for an existing name and never
 * stores workspace trust. Destroy the dialog before releasing callback context. */
    UmiStatus
    UmiGtk4BuildSettingsBindConfigurations(UmiGtk4DeveloperDialog *dialog,
                                           const UmiGtk4BuildConfigurationCallbacks *callbacks);
typedef struct UmiGtk4BuildConfigurationLifecycleCallbacks
{
    UmiStatus (*rename)(const char *root, const char *name, const char *replacement,
        uint64_t revision, uint64_t *out_revision, void *context);
    UmiStatus (*remove)(const char *root, const char *name, uint64_t revision,
        uint64_t *out_revision, void *context);
    void *context;
} UmiGtk4BuildConfigurationLifecycleCallbacks;
/** Add optional rename and removal controls after BindConfigurations succeeds.
 * The host keeps context alive until dialog destruction, just as for the base
 * callbacks. Removal requires explicit confirmation for the current selection;
 * selection or catalogue changes clear that confirmation. Form edits and the
 * currently applied project settings are retained. GTK thread only, once.
 */
UmiStatus UmiGtk4BuildSettingsBindConfigurationLifecycle(UmiGtk4DeveloperDialog *dialog,
    const UmiGtk4BuildConfigurationLifecycleCallbacks *callbacks);
#ifdef __cplusplus
}
#endif
#endif
