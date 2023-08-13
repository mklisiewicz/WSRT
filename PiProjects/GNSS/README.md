# GNSS Software Description
GNSS directory provides a C++ program tailored for Raspberry Pi-based projects, focusing on interacting with the ZED-F9P GNSS receiver using I2C communication. It offers capabilities for writing RTCM messages, essential for accurate RTK corrections, and reading NMEA messages that contain crucial positional and timing information in a standardized format.
## Features

- **RTCM Message Writing:** Transmit RTCM messages to the ZED-F9P GNSS receiver using I2C communication for real-time kinematic (RTK) correction data.

- **NMEA Message Reading:** Interpret NMEA messages from the GNSS receiver through I2C communication to access standard positioning and timing information.

## Using RTKLIB's str2str Utility
The provided `gnss` binary is designed to work in conjunction with [RTKLIB's `str2str`](https://github.com/tomojitakasu/RTKLIB) utility. `str2str` serves as a bridge, enabling the `gnss` program to receive RTCM messages from the NTRIP server and forward them to the GNSS receiver.

## Usage

1. Connect your ZED-F9P GNSS receiver to your Raspberry Pi using the I2C interface.

2. Set desired config of the receiver modifying UBX messages specified in config.cpp

2. Make the GNSS program using provided Makefile:

   ```bash
   cd path/to/gnss/build
   make
   ```

3. Run the gnss binary with RTKLIB's str2str utility for writing RTCM messages to the receiver and saving NMEA messages to a file specified in protocol.cpp:

    ```bash
    str2str -in ntrip://[username]:[password]@ntripserver:[port]/[access_point] | path/to/gnss/build/gnss 
    ```
