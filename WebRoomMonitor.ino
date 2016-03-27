//https://www.arduino.cc/en/Reference/Ethernet
#include <SPI.h>
#include <Ethernet.h>

#include <dht.h>
#include <BH1750.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

EthernetClient client;

  dht dhtSens; // Объект-сенсор
  BH1750 lightMeter;
  int WaterSensorPin = A3;
  int motionPin = 5;
  int DHT22_PIN = A0;
  int GasPin = A1;

  int temperature = 23;
  int humidity = 37;
  int light = 10;
  bool flooded = false;
  int ws = 10;
  int gas = 0;
  bool motion;

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  lightMeter.begin();
  Serial.println("\n[backSoon]");


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void SendHTMLPageToClient(){
  // send a our http response header
  client.print("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nRetry-After: 600\r\n Connection: close \r\n\r\n");
  client.print("<html><head><meta http-equiv='refresh' content='4'/>");
  client.print("<title>SensorsWebServer</title></head>");
  client.print("<body style=\"background-color:#004600\"><table cellpadding=\"5\" style=\"background-color:#004600;border-style:hidden;color:#11ff11\" align=center><tbody><tr>"); // refresh the page automatically every 5 sec
  client.print("<td>Gas</td>\n<td>Termperature</td>\n<td>Humidity</td>\n<td>Light</td>\n<td>Floodance</td>\n<td>Inhabitance</td>\n</tr><tr>");
  
  // output the values
  client.print("<td>");
  client.print(gas);
  client.print("</td>");
  
  client.print("<td>");
  client.print(temperature);
  client.print("</td>");
  
  client.print("<td>");
  client.print(humidity);
  client.print("</td>");
  
  client.print("<td>");
  client.print(light);
  client.print("</td>");
  
  client.print("<td>");
  client.print(flooded);
  client.print("</td>");
  
  client.print("<td>");
  client.print(motion);
  client.print("</td>");
  
  client.println("</body></html>");
  client.stop();
}
 

 void UpdateSensors(){
  dhtSens.read22(DHT22_PIN);
  temperature = dhtSens.temperature;
  humidity = dhtSens.humidity;
  light = lightMeter.readLightLevel();
  
  ws = analogRead(WaterSensorPin);
  flooded = (ws > 100);
  
  motion = (digitalRead(motionPin) == HIGH);

  gas = analogRead(GasPin);
}

void loop() {
  // listen for incoming clients
  client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
      Serial.print("GOT AN EMPTY LINE HERE!");
          UpdateSensors();
          SendHTMLPageToClient();
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
