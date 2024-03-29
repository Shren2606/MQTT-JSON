#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.
const char* ssid = "A21";           // name wifi of you
const char* password = "744653aa";  //
const char* mqtt_server = "192.168.1.6";
const uint16_t mqtt_port = 1883;
int ledpin = 2;

WiFiClient espClient;
PubSubClient client(espClient);
///
//receive data
///
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  //practise string
  //char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

  if (String(topic) == "json") {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload);
    // deserializeJson(doc,str); can use string instead of payload
    const char sensor = doc["sensor"];
    String time = doc["time"];
    float data1 = doc["data"][0];
    float data2 = doc["data"][1];


    Serial.print("time =");
    Serial.print(time);
    Serial.print("data1 =");
    Serial.print(data1);
  }
}

void setup_wifi() {
  delay(10);
  //ket noi wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  delay(1500);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void reconect() {

  while (!client.connected()) {
    Serial.print("Attemting MQTT connecting....");

    if (client.connect("espClient")) {
      Serial.println("Connected");
      //khi Kết nối thành công sẽ gửi chuỗi hello world lên topic event
      client.publish("hello", "2_Lê Hoàng Thiện - 41801239 ");

      //... sau đó sub lại thông tin
      client.subscribe("json");  // nhận thông tin từ node-red
      client.subscribe("message");
      Serial.println("subscribe ");

    } else {
      Serial.print("fail, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconect();
  }
  client.loop();
  client.publish("hello", "2_Lê Hoàng Thiện - 41801239 ");

  StaticJsonDocument<256> doc;
  doc["sensor"] = "gps";
  doc["time"] = 1351824120;
  doc["address"] = "460/4/29, LVL";

  // Add an array.
  JsonArray data = doc.createNestedArray("data");
  data.add(4180123999);
  data.add(26062000);
  data.add(41801239);
  //doc["data"]=data;
  // Generate the minified JSON and send it to the Serial port.
  //
  char out[128];
  int b = serializeJson(doc, out);
  Serial.print("publishing bytes = ");
  Serial.println(b, DEC);
  client.publish("json", out);

  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
}
