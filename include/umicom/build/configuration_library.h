/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/configuration_library.h
 * PURPOSE: Keep named project build and launch configurations in the selected local Data Server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_CONFIGURATION_LIBRARY_H
#define UMICOM_BUILD_CONFIGURATION_LIBRARY_H
#include "umicom/build/profile_store.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_BUILD_CONFIGURATION_CAPACITY 32U
#define UMI_BUILD_CONFIGURATION_NAME_CAPACITY 64U
    typedef struct UmiBuildConfigurationCatalogue
    {
        uint64_t revision;
        size_t count;
        char names[UMI_BUILD_CONFIGURATION_CAPACITY][UMI_BUILD_CONFIGURATION_NAME_CAPACITY];
    } UmiBuildConfigurationCatalogue;
    /** Names contain 1..63 ASCII letters, digits, spaces, dots, dashes or underscores.
 * Leading/trailing spaces are refused; a catalogue contains each name once. Names are
 * case-sensitive on every host. No path, shell or SQL interpretation occurs. */
    UmiStatus UmiBuildConfigurationNameValidate(const char *name);
    /** Capture names for one absolute project root. An absent library yields an
 * empty catalogue with revision zero. A complete transaction gives one coherent
 * revision; failure preserves out. Memory servers are not persistent.
 * The caller retains the server and serialises calls on its owning thread. */
    UmiStatus UmiBuildConfigurationCapture(UmiDataServer *server, const char *root,
                                           UmiBuildConfigurationCatalogue *out);
    /** Load a named settings copy only if the catalogue still has expected_revision.
 * This never modifies active settings, workspace trust, documents or processes.
 * Source identity is lexical and platform-specific, as in the profile store.
 * Failure leaves out unchanged. Settings are ordinary data, not a secret vault. */
    UmiStatus UmiBuildConfigurationLoad(UmiDataServer *server, const char *root, const char *name,
                                        uint64_t expected_revision, UmiBuildProfile *out);
    /** Save a complete named configuration using the captured catalogue revision.
 * An existing name is replaced only by this explicit operation. Fields, name
 * index and next revision commit together. Stale/corrupt input is preserved.
 * The profile's source must be absolute. No active profile or trust is changed.
 * A successful call assigns out_revision; failures leave it unchanged. */
    UmiStatus UmiBuildConfigurationSave(UmiDataServer *server, const char *name,
                                        const UmiBuildProfile *profile, uint64_t expected_revision,
                                        uint64_t *out_revision);
/** Rename one saved configuration at the captured catalogue revision. Existing
 * destination names are never replaced. The profile, active settings and trust
 * remain unchanged. An identical name validates the selection and returns the
 * current revision without writing. Failure preserves out_revision. */
UmiStatus UmiBuildConfigurationRename(UmiDataServer *server, const char *root,
    const char *name, const char *replacement, uint64_t expected_revision, uint64_t *out_revision);
/** Remove a saved configuration and compact its catalogue in one transaction.
 * This changes saved library data only, never active settings or project files.
 * Hosts must obtain explicit user confirmation for the selected name. Unknown
 * record fields, malformed profiles and stale revisions refuse the transaction.
 * Failure preserves stored records and out_revision. */
UmiStatus UmiBuildConfigurationRemove(UmiDataServer *server, const char *root,
    const char *name, uint64_t expected_revision, uint64_t *out_revision);
#ifdef __cplusplus
}
#endif
#endif
