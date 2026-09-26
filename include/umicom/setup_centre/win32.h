/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native bootstrap presentation. Installation logic remains in setup_centre. */

#ifndef UMICOM_SETUP_CENTRE_WIN32_H
#define UMICOM_SETUP_CENTRE_WIN32_H
#ifdef __cplusplus
extern "C" {

#endif
#ifdef _WIN32
    int UmiSetupWin32Run(void *instance,int showCommand);
    /** Hidden construction check: no release is loaded and no worker starts. */
    int UmiSetupWin32Check(void *instance);

#endif
#ifdef __cplusplus
}

#endif
#endif
