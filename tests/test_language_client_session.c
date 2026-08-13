/* Umicom Framework language client session test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include "umicom/language/client_session.h"
int main(void){UmiLanguageClientSession s;assert(umi_language_client_session_init(&s,"primary","clangd","file:///work")==UMI_STATUS_OK);assert(umi_language_client_session_transition(&s,UMI_LANGUAGE_CLIENT_STARTING)==UMI_STATUS_OK);assert(umi_language_client_session_transition(&s,UMI_LANGUAGE_CLIENT_INITIALIZING)==UMI_STATUS_OK);assert(umi_language_client_session_transition(&s,UMI_LANGUAGE_CLIENT_READY)==UMI_STATUS_OK);umi_language_client_session_record_request(&s);umi_language_client_session_record_response(&s);assert(s.requests_sent==1U&&s.responses_received==1U);return 0;}
