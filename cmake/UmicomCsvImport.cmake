# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Extend the established base owner so all installed consumers use one parser.
target_sources(umicom_base PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/base/csv_reader.c")
if(BUILD_TESTING)
    umicom_report_export_test(csv-reader test_csv_reader.c Umicom::base
        records quotes multiline empty trailing bom ownership malformed utf8 controls
        columns field-limit byte-limit output invalid)
endif()
