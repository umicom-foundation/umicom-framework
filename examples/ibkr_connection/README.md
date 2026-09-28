# Paper and Live connection laboratory

The primary implementation and CLI are C23. Configure this directory with CMake and Ninja, build, then run CTest. Tests use local inert fixtures only. `umicom-broker-profile-example` does not open a socket. `umicom-broker-connect connect` is the explicit real endpoint-opening command.

See `../../docs/learning/paper-live-connections.html` for complete beginner instructions and `../../docs/development/ibkr-connection-contract.md` for ownership, protocol and limits. GTK4 >= 4.10 is optional. No official vendor SDK is required or bundled. Current real-TWS compatibility is not yet qualified.

The focused SDK compiles the actual canonical status and broker mapper sources as a dependency subset, not the full Framework trading platform. Install it into a separate laboratory prefix. The `client` directory tests consuming that installed SDK without making a network connection.
