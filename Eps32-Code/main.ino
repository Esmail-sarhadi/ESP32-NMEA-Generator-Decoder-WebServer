#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

// Replace these with your network credentials
const char* ssid = "charon";
const char* password = "12121212";

WebServer server(80);

String generateRandomNMEAData();
String generateRandomTime();
String generateRandomCoordinate(bool isLatitude);
String generateRandomSatelliteCount();
String calculateChecksum(String nmea);
String decodeNMEAData(String nmea_data);
bool process_gpgga_line(const String& line, struct GPGGAData& data);

struct GPGGAData {
    String time;
    String latitude;
    String latitude_direction;
    String longitude;
    String longitude_direction;
    String satellite_count;
};

void handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP32 NMEA Data</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        .button { display: inline-block; padding: 10px 20px; font-size: 16px; cursor: pointer; }
        .button:hover { background-color: #ddd; }
    </style>
</head>
<body>
    <h1>ESP32 NMEA Data</h1>
    <button class="button" onclick="generateData()">Generate Sample Data</button>
    <button class="button" onclick="document.getElementById('fileInput').click()">Upload NMEA File</button>
    <input type="file" id="fileInput" style="display: none;" onchange="uploadFile()">
    <pre id="output"></pre>
    <script>
        function generateData() {
            fetch('/generate')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('output').textContent = data;
                });
        }
        function uploadFile() {
            const fileInput = document.getElementById('fileInput');
            const file = fileInput.files[0];
            const formData = new FormData();
            formData.append('file', file);
            fetch('/upload', {
                method: 'POST',
                body: formData
            })
            .then(response => response.text())
            .then(data => {
                document.getElementById('output').textContent = data;
            });
        }
    </script>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", html);
}

void handleGenerateData() {
    String data = generateRandomNMEAData();
    server.send(200, "text/plain", data);
    Serial.println("Generated NMEA Data:");
    Serial.println(data);
}

void handleUpload() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        String received_data = String((char*)upload.buf, upload.currentSize);
        String decoded_data = decodeNMEAData(received_data);
        Serial.println("Received NMEA Data:");
        Serial.println(received_data);
        Serial.println("Decoded NMEA Data:");
        Serial.println(decoded_data);
        server.send(200, "text/plain", decoded_data);
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.setDebugOutput(false);
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.on("/", handleRoot);
    server.on("/generate", handleGenerateData);
    server.on("/upload", HTTP_POST, []() {}, handleUpload);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}

String generateRandomNMEAData() {
    String nmea = "";

    for (int i = 0; i < 10; i++) {
        nmea += "$GPGGA," + generateRandomTime() + "," + generateRandomCoordinate(true) + "," + generateRandomCoordinate(false) + ",1," + generateRandomSatelliteCount() + ",1.0,1.0,M,1.0,M,,";
        nmea += "*" + calculateChecksum(nmea) + "\n";
    }

    return nmea;
}

String generateRandomTime() {
    char buffer[7];
    snprintf(buffer, sizeof(buffer), "%02d%02d%02d", random(0, 24), random(0, 60), random(0, 60));
    return String(buffer);
}

String generateRandomCoordinate(bool isLatitude) {
    char buffer[12];
    int degrees = isLatitude ? random(0, 90) : random(0, 180);
    float minutes = random(0, 6000) / 100.0;
    char direction = isLatitude ? (random(0, 2) == 0 ? 'N' : 'S') : (random(0, 2) == 0 ? 'E' : 'W');
    snprintf(buffer, sizeof(buffer), "%02d%07.4f,%c", degrees, minutes, direction);
    return String(buffer);
}

String generateRandomSatelliteCount() {
    return String(random(1, 13));
}

String calculateChecksum(String nmea) {
    int checksum = 0;
    for (int i = 1; i < nmea.length(); i++) {
        checksum ^= nmea[i];
    }
    char buffer[3];
    snprintf(buffer, sizeof(buffer), "%02X", checksum);
    return String(buffer);
}

bool process_gpgga_line(const String& line, GPGGAData& data) {
    int index = 0;
    int lastIndex = 0;
    String parts[15];
    for (int i = 0; i < 15; i++) {
        index = line.indexOf(',', lastIndex);
        if (index == -1) {
            parts[i] = line.substring(lastIndex);
            break;
        }
        parts[i] = line.substring(lastIndex, index);
        lastIndex = index + 1;
    }

    if (parts[0] != "$GPGGA") {
        return false;
    }

    data.time = parts[1].substring(0, 2) + ":" + parts[1].substring(2, 4) + ":" + parts[1].substring(4, 6);
    data.latitude = parts[2];
    data.latitude_direction = parts[3];
    data.longitude = parts[4];
    data.longitude_direction = parts[5];
    data.satellite_count = parts[7];

    return true;
}

String decodeNMEAData(String nmea_data) {
    String decoded_data = "";
    int start = 0;
    int end = nmea_data.indexOf('\n');
    while (end != -1) {
        String line = nmea_data.substring(start, end);
        if (line.indexOf("$GPGGA") != -1) {
            GPGGAData data;
            if (process_gpgga_line(line, data)) {
                decoded_data += "Time: " + data.time + ", ";
                decoded_data += "Latitude: " + data.latitude + " " + data.latitude_direction + ", ";
                decoded_data += "Longitude: " + data.longitude + " " + data.longitude_direction + ", ";
                decoded_data += "Satellite Count: " + data.satellite_count + "\n";
            }
        }
        start = end + 1;
        end = nmea_data.indexOf('\n', start);
    }
    return decoded_data;
}
