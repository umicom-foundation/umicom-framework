# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomInstalledInventoryConsumer LANGUAGES C)
find_package(UmicomReleaseBaseline CONFIG REQUIRED)
add_executable(inventory-consumer main.c)
target_link_libraries(inventory-consumer PRIVATE Umicom::distribution)
set_target_properties(inventory-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
enable_testing()
add_test(NAME installed.inventory.consumer COMMAND inventory-consumer)

# Keep the original installed consumer and independently link the policy API.
add_executable(policy-consumer policy_consumer.c)
target_link_libraries(policy-consumer PRIVATE Umicom::distribution)
set_target_properties(policy-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
add_test(NAME installed.policy.consumer COMMAND policy-consumer)
