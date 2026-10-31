#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomIbkrNewsProviders.cmake
#
# PURPOSE:
#   Register bounded news-provider discovery and regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(_umi_news_root "${CMAKE_CURRENT_LIST_DIR}/..")
target_sources(umicom_ibkr_connection PRIVATE "${_umi_news_root}/src/ibkr_connection/news_providers.c")
install(FILES "${_umi_news_root}/include/umicom/broker_connectivity/news_providers.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${_umi_news_root}/include/umicom/broker_connectivity/news_providers.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT FrameworkSDK EXCLUDE_FROM_ALL)
if(BUILD_TESTING)
    add_executable(umicom-ibkr-news-providers-test "${_umi_news_root}/tests/ibkr_connection/test_news_providers.c")
    target_include_directories(umicom-ibkr-news-providers-test PRIVATE "${_umi_news_root}/src/ibkr_connection")
    target_link_libraries(umicom-ibkr-news-providers-test PRIVATE Umicom::ibkr_connection)
    umicom_ibkr_target(umicom-ibkr-news-providers-test)
    foreach(_case IN ITEMS wire empty malformed timeout disconnect unsolicited)
        add_test(NAME framework.ibkr_news_providers.${_case} COMMAND umicom-ibkr-news-providers-test ${_case})
    endforeach()
    if(COMMAND umicom_register_validation_targets)
        umicom_register_validation_targets(umicom-ibkr-news-providers-test)
    endif()
endif()
unset(_umi_news_root)
