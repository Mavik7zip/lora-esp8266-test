//codice main
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LoRa_E22.h>
#include <Adafruit_I2CDevice.h>


// Dichiarazione per il display SSD1306 connesso all'I2C
// #define SCREEN_WIDTH 128 // larghezza in pixel
// #define SCREEN_HEIGHT 64 // altezza in pixel
// #define OLED_RESET    -1 // (pin di reset, -1 è lo stesso pin di arduino)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Pin del modulo lora
#define NSS 16
#define RST 0
#define DIO0 15

// Intervallo tra due pacchetti
#define send_delay 1000

// default message
#define defMessage "MSG"
#define PrintDisplaySignal true

// wifi data
#define SSID "FASTGATE_2.4G"
#define PWD "Af4339XcbrSn"

#define ebyte_send true
#define ebyte_recive true

#define DESTINATION_ADDL 2

// Valori radio di dafault
int spredingfactor = 12;   // fattore di diffusione (12 mele)
int codrate = 8;        // velocità di codifica (8 banane)
double bandwidth = 250E3;   // larghezza di banda (7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500 + E3)
int txpower = 18;       // potenza Tx (0-18 dBm, dipende dal moduli)
int gain = 0;         // guadagno Rx (0-6 dB, con 0 è automatico)


int slt = 48;
int settings_slt;
// srand(time(NULL));
int id = rand() % 9999;
struct {
  String text;
  int counter;
  int ping;
  int rssi;
  int snr;
  int id;
  bool is_arrive;
} packet;

ESP8266WebServer http_rest_server(80);

LoRa_E22 e22ttl(0, 0, 0, 0, 0);


// ####################################################################################################

void send_mode(String message);

// ####################################################################################################

void serialFlush() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}

// ####################################################################################################

// void print_mod(String mod){
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.print(mod);
//   display.display();
// }

// ####################################################################################################

// void print_display() {
//   display.clearDisplay();
//   display.setTextSize(2);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.print(F("Rssi: "));
//   display.print(packet.rssi);
//   display.setCursor(0, 30);
//   display.print(F("Snr: "));
//   display.print(packet.snr);
//   display.display();
// }

// ####################################################################################################

// void print_display_settings() {
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0, 0);
//   display.print(F("TX: "));
//   display.print(txpower);
//   display.setCursor(0, 10);
//   display.print(F("BDW: "));
//   display.print(bandwidth/1000);
//   display.setCursor(0, 20);
//   display.print(F("Gain: "));
//   display.print(gain);
//   display.display();
// }

// ####################################################################################################

void print_serial() {
  Serial.println("\r\n    text: " + String(packet.text));
  Serial.println("  n° pkt: " + String(packet.counter));
  Serial.println("    ping: " + String(packet.ping));
  Serial.println("    rssi: " + String(packet.rssi));
  Serial.println("     snr: " + String(packet.snr));
}

// ####################################################################################################

String read_string() {
  char read;
  String string;

  do {
    while (!Serial.available());
    read = Serial.read();
    Serial.print(read);

    if (read != '\n') {
      string += read;
    }
  } while (read != '\n');

  return string;
}

// ####################################################################################################

void read_packet(int packet_size) {
  String tmp;
  char tmp_char;
  int j = 0;

  for (int i = 0; i < packet_size; i++) {
    tmp_char = (char)LoRa.read();

    if (tmp_char != '\n') {
      tmp += tmp_char;
    } else {
      switch (j) {
        case 0: 
          packet.text = tmp;
          break;
        case 1: 
          packet.counter = tmp.toInt(); 
          break;
        case 2:
          packet.id = tmp.toInt();
          break;
      }

      j++;
      tmp = "";
    }
  }

  packet.rssi = LoRa.packetRssi();
  packet.snr = LoRa.packetSnr();

  packet.is_arrive = true;
  
  // if (PrintDisplaySignal){
  //   print_display();
  // }

  if(slt == 4 + 48){   // perche serve l'interrupt (bidirectional message)
    Serial.println(packet.text);
  }
}

// ####################################################################################################

int menu() {
  while (!Serial);
  Serial.print("\r\n");
  Serial.println("----------------------");
  Serial.println("receiver mode      [1]");
  Serial.println("sender mode        [2]");
  Serial.println("bidirectional mode [3]");
  Serial.println("bidirec message    [4]");
  Serial.println("message mode       [5]");
  Serial.println("settings menu      [6]");
  Serial.println("----------------------");


  while (!Serial.available());

  return (int)Serial.read();
}

// ####################################################################################################

int settings_menu() {
  while (!Serial);

  Serial.println("----------------------");
  Serial.println("set gain           [1]");
  Serial.println("set bandwidth      [2]");
  Serial.println("set txpower        [3]");
  Serial.println("rssi radio         [4]");
  Serial.println("set txpower+amp    [5]");
  Serial.println("                      ");
  Serial.println("quit               [0]");
  Serial.println("----------------------");

  while (!Serial.available());
  return (int)Serial.read();
}

// ####################################################################################################

void set_gain() { //int gain
  // Serial.println("\r\nvalue: 0-6 (0 => automatico)");

  // while (!Serial.available());
  // int gain = ((int)Serial.read() - 48);

  if (gain <= 6 && gain >= 0) {
    LoRa.setGain(gain);
    Serial.println("gain = " + String(gain));
  }  else {
    Serial.println("value not set");
  }

  Serial.println();
}

// ####################################################################################################

void set_bandwidth(double bandwidth) {
  // Serial.println("(7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500)(125 => default)");

  LoRa.setSignalBandwidth(bandwidth * 1000);
  Serial.println("bandwidth = " + String(bandwidth));

  Serial.println();
}

// ####################################################################################################

void set_txpower(int txpower) {
  // Serial.println("(2-20)(18 => default)");

  LoRa.setTxPower(txpower);
  Serial.println("txpower = " + String(txpower));

  Serial.println();
}

// ####################################################################################################

// void CAD() {
//   void channel_detection(boolean signalDetected) {
    
//   }

//   LoRa.channelActivityDetection();
//   LoRa.onCadDone(channel_detection);
  
// }

// ####################################################################################################

// void set_txpower_amplifier(int dbm) {    l'asciamola per ultima
//   float watt_request;

//   Serial.println("(21-37)(37 => default)");

//   delay(1000);

//   watt_request = pow(10,(dbm/10))/1000;
//   watt_request = watt_request/80;
//   dbm = 10*log10(watt_request)+30;

//   LoRa.setTxPower(dbm);
//   Serial.println("txpower = " + String(dbm));

//   Serial.println();
// }

// ####################################################################################################

void rssi_radio() {
  Serial.println("rssi radio: " + String(LoRa.rssi()) + "\r\n");
}

// ####################################################################################################

void receive_mode() {
  Serial.println("  Starting receive mode");
  // print_mod("receive mode");

  boolean quit = false;

  LoRa.onReceive(read_packet);
  LoRa.receive();

  while (quit != 1) {
    if (packet.is_arrive == true) {
      // print_serial();
      packet.is_arrive = false;
    }

    delay(500);

    // if (Serial.available() > 0) {
    //   if (Serial.read() == '0') quit = true;
    //   serialFlush();
    // }

    http_rest_server.handleClient();
  }
  
  LoRa.idle();
  Serial.println("  Exiting receive mode");
}

// ####################################################################################################

void send_data(String message, int counter, int id) {
  if(ebyte_send == true){
    e22ttl.begin();
    e22ttl.setMode(MODE_1_WOR);
    ResponseStatus rs = e22ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Hello, world?");
    Serial.println(rs.getResponseDescription());
  
    e22ttl.setMode(MODE_0_NORMAL);
  }else {
    LoRa.beginPacket();
    LoRa.println(message);
    LoRa.println(counter);
    LoRa.println(id);
    LoRa.endPacket();
  }  
}

// ####################################################################################################

void send_mode(String message) {
  Serial.println("  Starting send ciro mode");
  // print_mod("send mode");

  boolean quit = false;
  int counter = 0;

  while (quit != 1) {
    send_data(message, counter, id);

    Serial.print("sending pkg n°: ");
    Serial.println(counter);

    counter++;

    delay(send_delay);

    // if (Serial.available() > 0) {
    //   if (Serial.read() == '0') quit = true;
    //   serialFlush();
    // }

    http_rest_server.handleClient();
  }

  LoRa.idle();
  Serial.println("  Exiting send ciro mode");
}

// ####################################################################################################

void send_message_mode() {
  Serial.println("  Starting send message mode");

  boolean quit = false;
  int counter = 0;
  String message;

  while (quit != 1) {
    message = read_string();

    send_data(message, counter, 0);
    Serial.println(message + String(counter));

    counter++;

    // if (Serial.available() > 0) {
    //   if (Serial.read() == '0') quit = true;
    //   serialFlush();
    // }

    http_rest_server.handleClient();
  }

  LoRa.idle();
  Serial.println("  Exiting send message mode");
}

// ####################################################################################################

void bidirectional_mode() {
  Serial.println("  Starting bidirectional mode");
  // print_mod("bidirec mode");

  boolean quit = false;
  int counter = 0;
  int last_count = 0;

  LoRa.onReceive(read_packet);
  LoRa.receive();

  while (quit != 1) {

    if (packet.is_arrive == true) {
      if (packet.id == id) {
        if (counter % 4 == 0) {
          if(packet.counter != last_count){
            print_serial();
            last_count = packet.counter;
          }
        }
      } else {
        send_data(packet.text, packet.counter, packet.id);
        LoRa.receive();
        packet.is_arrive = false;
      }
    }

    if (counter % 12 == 0) {
      send_data("kk", counter / 12, id);
      LoRa.receive();
      Serial.println("sending n°" + String(counter / 12));
    }

    counter++;

    delay(send_delay / 4);

    // if (Serial.available() > 0) {
    //   if (Serial.read() == '0') quit = true;
    //   serialFlush();
    // }

    http_rest_server.handleClient();
  }

  LoRa.idle();
  Serial.println("  Exiting bidirectional mode");
}


void bidirectional_message() {
  Serial.println("  Starting bidirectional message mode");
  // print_mod("bidirec mess mode");


  boolean quit = 1;
  String message;

  LoRa.onReceive(read_packet);
  LoRa.receive();

  while (quit != 0) {

    message = read_string();

    send_data(message, 0, id);
    LoRa.receive();

    http_rest_server.handleClient();
  }

  LoRa.idle();
  Serial.println("  Exiting bidirectional message mode");
}

// ####################################################################################################

void select_mod(int mod){
  switch (mod) {
    case 1:
      receive_mode();
      break;
    case 2:
      send_mode(defMessage);
      break;
    case 3:
      bidirectional_mode();
      break;
    case 4:
      bidirectional_message();
      break;
    case 5:
      send_message_mode();
      break;
  }
}

// ####################################################################################################

void get_bees() {
  DynamicJsonDocument doc(256);
  char JSONmessageBuffer[256];
  
  doc["rssi_radio"] = LoRa.rssi();
  doc["rssi"] = packet.rssi;
  doc["snr"] = packet.snr;
  doc["text"] = packet.text;
  doc["ping"] = packet.ping;
  doc["counter"] = packet.counter;
  doc["is_arrive"] = packet.is_arrive;
  doc["mod"] = slt;

  serializeJson(doc, JSONmessageBuffer);
  http_rest_server.send(256, "application/json", JSONmessageBuffer);
}

// ####################################################################################################

void get_settings() {
  DynamicJsonDocument doc(256);
  char JSONmessageBuffer[256];

  doc["bandwidth"] = (bandwidth/1000);
  doc["txpower"] = txpower;
  doc["gain"] = gain;
  doc["spredingfactor"] = spredingfactor;
  doc["codrate"] = codrate;

  serializeJson(doc, JSONmessageBuffer);
  http_rest_server.send(256, "application/json", JSONmessageBuffer);
}

// ####################################################################################################

void post_bees(){
  String postBody = http_rest_server.arg("plain");
  DynamicJsonDocument doc(256);
  deserializeJson(doc, postBody); // DeserializationError error = 

  // if(error == (DeserializationError::Ok)){
  //   Serial.println("ho yea im working");
  // }else {
  //   Serial.println("mamt");
  // }

  String mod = doc["mod"];
  Serial.println(mod);

  http_rest_server.send(256, "text/plain", "ok");

  select_mod((mod.toInt()));
}

// ####################################################################################################

void post_settings(){
  String postBody = http_rest_server.arg("plain");
  DynamicJsonDocument doc(256);
  deserializeJson(doc, postBody);

  String bandwidth_raw = doc["bandwidth"];
  String txpower_raw = doc["txpower"];
  String gain_raw = doc["gain"];
  String spredingfactor_raw = doc["spredingfactor"];
  String codrate_raw = doc["codrate"];


  bandwidth = (bandwidth_raw.toDouble())*1000;
  txpower = txpower_raw.toInt();
  gain = gain_raw.toInt();
  spredingfactor = spredingfactor_raw.toInt();
  codrate = codrate_raw.toInt();

  // Serial.println(bandwidth_raw);
  // Serial.println(txpower_raw);
  // Serial.println(gain_raw);
  // Serial.println(spredingfactor_raw);
  // Serial.println(codrate_raw);

  LoRa.setTxPower(txpower);
  LoRa.setGain(gain);
  LoRa.setSignalBandwidth((bandwidth));
  LoRa.setSpreadingFactor(spredingfactor);
  LoRa.setCodingRate4(codrate);

  // print_display_settings();

  http_rest_server.send(256, "text/plain", "ok");

}

// ####################################################################################################

void config_rest_server_routing() {
  
  http_rest_server.on("/post", HTTP_OPTIONS, []() {
    http_rest_server.enableCORS(true);
  });
  http_rest_server.on("/post_settings", HTTP_OPTIONS, []() {
    http_rest_server.enableCORS(true);
  });

  http_rest_server.on("/get", HTTP_GET, get_bees);
  http_rest_server.on("/settings", HTTP_GET , get_settings);
  http_rest_server.on("/post", HTTP_POST , post_bees);
  http_rest_server.on("/post_settings", HTTP_POST , post_settings);
}

// ####################################################################################################
// ####################################################################################################

void setup() {

  int Ntry = 0;

  while (!Serial);

  Serial.begin(115200);
  Serial.setTimeout(3000);
  serialFlush();

  Serial.println("\r\n\r\nLoRa Main code");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //Address 0x3D for 128x64
  //   Serial.println("SSD1306 starting fail");
  // } else {
  //   Serial.println("SSD1306 started");

  //   // scrivo le prime cose cose sul display
  //   //display.display();
  //   display.clearDisplay(); // questo non esce mai
  //   display.setTextSize(1);
  //   display.setTextColor(WHITE);
  //   display.setCursor(19, 26);
  //   display.print("LoRa Main");
  //   display.display();
  // }

  // starto l'antenna a 433 MHz
  if(ebyte_recive == false || ebyte_send == false){
    LoRa.setPins(NSS, RST, DIO0);
    if (!LoRa.begin(433E6)) {
      Serial.println("Starting LoRa failed!");

      // display.clearDisplay();
      // display.setCursor(0, 0);
      // display.print("starting LoRa failed");
      // display.display();

    } else {
      Serial.println("LoRa started succesfully");

      // display.clearDisplay();
      // display.setCursor(0, 0);
      // display.print("LoRa started succesfully");
      // display.display();
    }
  }else {
    LoRa_E22 e22ttl(D7, D8, D4, D6, D5);

    e22ttl.begin();
    e22ttl.setMode(MODE_0_NORMAL);
  }

  // setto le impostazioni base della radio
  LoRa.setTxPower(txpower);              // setto la potenza d'uscita (18 dBm)
  LoRa.setGain(gain);                  // setto il guadagno in ingresso (0-6 dB con 0 è automatico)
  LoRa.setSpreadingFactor(spredingfactor);  // setto il fattore di diffusione (12 mele)
  LoRa.setSignalBandwidth(bandwidth);     // setto la larghezza di banda (7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500)
  LoRa.setCodingRate4(codrate);          // setto la velocità di codifica (8)

  // wifi connection
  WiFi.begin(SSID, PWD);
  while(WiFi.status() != WL_CONNECTED && Ntry < 32){
    delay(500);
    Serial.print(".");
    Ntry++;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("connected");

    // display.clearDisplay();
    // display.setCursor(0, 0);
    // display.print("wifi connected");
    // display.setCursor(0, 12);
    // display.print(WiFi.localIP());
    // display.display();
    // Serial.print("Connected with IP: ");
    // Serial.println(WiFi.localIP());
  } else {
    Serial.println("wifi fail");

    // display.clearDisplay();
    // display.setCursor(0, 0);
    // display.print("wifi fail");
    // display.display();
  }

  // starto le apine
  http_rest_server.enableCORS(true);
  http_rest_server.begin();
  config_rest_server_routing();
}

// ####################################################################################################

void loop() {
  http_rest_server.handleClient();
}