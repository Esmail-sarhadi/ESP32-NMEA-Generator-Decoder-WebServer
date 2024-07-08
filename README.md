
# ESP32 NMEA Data Generation and Decoding

This project demonstrates an ESP32-based system for generating and decoding NMEA (National Marine Electronics Association) data. The ESP32 connects to a Wi-Fi network and hosts a web server that provides a user interface for generating sample NMEA data and uploading NMEA files for decoding.

## Features

- **Random NMEA Data Generation**: Generates random NMEA sentences (GPGGA) with valid checksums.
- **Web Interface**: Provides a user-friendly web interface to generate and display NMEA data.
- **File Upload and Decoding**: Allows users to upload NMEA files for decoding and displays the parsed data.
- **Data Parsing**: Parses and decodes GPGGA sentences to extract time, latitude, longitude, and satellite count.

## Getting Started

### Prerequisites

- ESP32 development board
- Arduino IDE
- Wi-Fi network credentials

### Installation

1. **Clone the repository**:
    ```bash
    git clone https://github.com/Esmail-sarhadi/ESP32-NMEA-Generator-Decoder-WebServer.git
    ```

2. **Open the project in Arduino IDE**:
    - File -> Open -> Select `esp32-nmea-data.ino`

3. **Install necessary libraries**:
    - Install `WiFi`, `WebServer`, `FS`, and `SPIFFS` from the Arduino Library Manager if not already installed.

4. **Update Wi-Fi credentials**:
    - Replace the placeholder values in the code with your actual Wi-Fi SSID and password:
    ```cpp
    const char* ssid = "your_SSID";
    const char* password = "your_PASSWORD";
    ```

5. **Upload the code to the ESP32**:
    - Connect your ESP32 board to your computer and upload the code from the Arduino IDE.

### Usage

1. **Connect to Wi-Fi**:
    - Upon startup, the ESP32 will attempt to connect to the specified Wi-Fi network.
    - Open the Serial Monitor in Arduino IDE to see the connection status and the IP address assigned to the ESP32.

2. **Access the Web Interface**:
    - Once connected, the ESP32 will start a web server. Open a web browser and navigate to the IP address displayed in the Serial Monitor to access the interface.

3. **Generate NMEA Data**:
    - Click the "Generate Sample Data" button to generate and display random NMEA data.

4. **Upload NMEA File**:
    - Click the "Upload NMEA File" button to upload a file from your computer. The decoded data will be displayed.

## Project Structure

- `esp32-nmea-data.ino`: The main Arduino sketch that initializes the web server and handles requests.
- `index.html`: The HTML file served to the client, containing the web interface.
- `data`: Directory containing additional files to be served by the ESP32 (if any).

## Functions

- `generateRandomNMEAData()`: Generates random NMEA sentences.
- `generateRandomTime()`: Generates a random time string.
- `generateRandomCoordinate(bool isLatitude)`: Generates random latitude or longitude coordinates.
- `generateRandomSatelliteCount()`: Generates a random satellite count.
- `calculateChecksum(String nmea)`: Calculates the checksum for an NMEA sentence.
- `decodeNMEAData(String nmea_data)`: Decodes NMEA data and extracts information from GPGGA sentences.
- `process_gpgga_line(const String& line, GPGGAData& data)`: Processes individual GPGGA sentences.

## Detailed Function Descriptions

### `generateRandomNMEAData()`

Generates a series of random NMEA sentences (GPGGA) with valid checksums. This function creates 10 GPGGA sentences with randomly generated time, coordinates, and satellite counts.

### `generateRandomTime()`

Generates a random time string in the format HHMMSS. This is used to simulate the time component in NMEA sentences.

### `generateRandomCoordinate(bool isLatitude)`

Generates a random coordinate string. If `isLatitude` is true, it generates a latitude coordinate; otherwise, it generates a longitude coordinate. The coordinates are generated in degrees and minutes, with a random direction (N/S for latitude, E/W for longitude).

### `generateRandomSatelliteCount()`

Generates a random satellite count between 1 and 12. This simulates the number of satellites tracked by the receiver.

### `calculateChecksum(String nmea)`

Calculates the checksum for an NMEA sentence. The checksum is calculated by XORing all characters between the '$' and '*' characters in the sentence.

### `decodeNMEAData(String nmea_data)`

Decodes NMEA data, specifically GPGGA sentences, and extracts information such as time, latitude, longitude, and satellite count. The extracted data is formatted into a human-readable string.

### `process_gpgga_line(const String& line, GPGGAData& data)`

Processes an individual GPGGA sentence and extracts the relevant data into a `GPGGAData` structure. This function splits the sentence into its components and assigns them to the corresponding fields in the `GPGGAData` structure.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- This project was inspired by the need to understand and work with NMEA data in IoT applications.
- Special thanks to the open-source community for providing resources and libraries that made this project possible.

