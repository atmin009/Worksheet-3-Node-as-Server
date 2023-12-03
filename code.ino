#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer server(80);

const int ledPin = D6;
DHT dht14(D4, DHT11);

void init_wifi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {
  Serial.begin(115200);
  init_wifi("min_suppanat", "14433000");

  pinMode(ledPin, OUTPUT);

  dht14.begin();

server.on("/", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();

    String html = "<html><head>";
    html += "<style>";
     html += ".button-group { ";
    html += "  text-align: center;";
    html += "}";
    html += ".button-group form { ";
    html += "  display: inline-block;";
    html += "  margin: 0 10px;";
    html += "}";
    html += "<body><div>";
    html += "body { font-family: Arial, Helvetica, sans-serif; background-color: #E7E6FF; }";
    html += "div { ";
    html += "  background-color: #ffffff;";
    html += "  border-radius: 10px;";
    html += "  padding: 25px;";
    html += "  margin: auto;";
    html += "  max-width: 600px;";
    html += "  text-align: center;";
    html += "  box-shadow: 0 5px 15px rgba(0,0,0,0.2);";
    html += "  border: 1px solid #ddd;";
    html += "}";    
    html += "h1 { color: #7E0637; font-size: 40px; }";
    html += "p { color: #333333; font-size: 20px; }";
    html += "input[type='submit'] { background-color: #4CAF50; color: white; padding: 12px 24px; border: none; border-radius: 5px; cursor: pointer; font-size: 18px; }";
    html += "input[type='submit']:hover { background-color: #45a049; }";
    html += "#turnOffButton { background-color: #f44336; }";
    html += "#turnOffButton:hover { background-color: #d32f2f; }";
    html += "</style>";
    html += "</head>";
    html += "<body><div>";
    html += "<h1>Worksheet 3: Node as Server</h1>" ;
    html += "<hr>";
    html += "<p>Temperature: <span id='temperature'>" + String(temperature, 2) + " Celsius</span></p>";
    html += "<p>Humidity: <span id='humidity'>" + String(humidity, 2) + " percent</span></p>";
    html += "<p>LED Status: ";
    if (digitalRead(ledPin) == HIGH) {
        html += "<span style='color: green;'>LED On</span>";
    } else {
        html += "<span style='color: red;'>LED Off</span>";
    }
    html += "</p>";
     html += "<div class='button-group'>";
    html += "<form action='/TurnOn' method='get'><input type='submit' value='Turn On'></form>";
    html += "<form action='/TurnOff' method='get'><input type='submit' id='turnOffButton' value='Turn Off'></form>";
    html += "</div>";

    html += "</div></body></html>";

    html += "<script>";
    html += "function updateData() {";
    html += "  var xhr = new XMLHttpRequest();";
    html += "  xhr.open('GET', '/data', true);";
    html += "  xhr.onreadystatechange = function() {";
    html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
    html += "      var data = JSON.parse(xhr.responseText);";
    html += "      document.getElementById('temperature').textContent = data.temperature.toFixed(2) + ' Celsius';";
    html += "      document.getElementById('humidity').textContent = data.humidity.toFixed(2) + ' percent';";
    html += "    }";
    html += "  };";
    html += "  xhr.send();";
    html += "}";
    html += "setInterval(updateData, 500);";
    html += "</script>";

    server.send(200, "text/html", html);
});


  server.on("/TurnOn", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/TurnOff", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/data", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();

    String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
    server.send(200, "application/json", data);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}