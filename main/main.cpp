#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET      -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ss 16
#define rst 0
#define dio0 15

int slt;
struct strt_pkt {
  String text;
  int no;
  int dev_ping;
  int rssi;
  int snr;
};

//####################################################################################################

void send_mode(String message);

//####################################################################################################

void print_display(strt_pkt packet) {
  display.clearDisplay(); //odio i display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("Rssi: "));
  display.print(packet.rssi);
  display.setCursor(0, 30);
  display.print(F("Snr: "));
  display.print(packet.snr);
  display.display();
}

//####################################################################################################

void print_serial(strt_pkt packet) {
  Serial.println("text:   ");
  Serial.print(packet.text);
  Serial.println("no pkt: ");
  Serial.print(packet.no);
  // Serial.print("dev ping: ");
  // Serial.print(packet.dev_ping);
  Serial.println("rssi:   ");
  Serial.print(packet.rssi);
  Serial.println("snr:    ");
  Serial.print(packet.snr);
}

//####################################################################################################

void print_data(strt_pkt packet) {
  print_serial(packet);
  // print_display(packet);
}

//####################################################################################################

void read_packet(int packet_size) {
  strt_pkt packet;
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
          packet.no = tmp.toInt();
          break;

        case 2:
          packet.dev_ping = tmp.toInt();
          break;
      }

      j++;
      tmp = "";
    }
  }

  packet.rssi = LoRa.packetRssi();
  packet.snr = LoRa.packetSnr();

  print_data(packet);

  if (slt == 3 + 48) {
    send_mode(packet.text);
  }
}

//####################################################################################################

int menu() {
  while (!Serial);

  Serial.println("receiver mode      [1]");
  Serial.println("sender mode        [2]");
  //Serial.println("relay mode capo    [3]");
  //Serial.println("relay mode schiavo [4]");
  Serial.println("rssi radio         [5]");
  Serial.println("monitor            [6]");
  Serial.println("set gain           [7]");
  Serial.println("\nquit               [0]");

  // int rssi = LoRa.rssi(); da fare

  while (!Serial.available());

  return (int)Serial.read();
}

//####################################################################################################

void set_gain() {
  int gain;

  Serial.println("value: 0-6");

  while (!Serial.available());

  gain = Serial.read();

  LoRa.setGain(gain);
}

//####################################################################################################

void rssi_monitor_mode() {
  Serial.println("rssi radio: ");

  Serial.println(LoRa.rssi());
}

//####################################################################################################

void recive_mode() {
  Serial.println("recive mode starting");

  int quit = 1;

  LoRa.onReceive(read_packet);
  LoRa.receive();

  while (quit != 0) {
    quit = Serial.read(); //mezzo fake
    delay(100);
  }

  LoRa.idle();
  Serial.println("recive mode exiting");
}

//####################################################################################################

void send_mode(String message) {
  Serial.println("sender mode starting");

  int quit = 1;
  int counter = 0;

  while (quit != 0) {

    LoRa.beginPacket();
    LoRa.println(message);
    LoRa.println(counter);
    LoRa.endPacket(); //too time

    Serial.print("sending pkg no: ");
    Serial.println(counter);

    counter++;

    delay(2000);
  }

  Serial.println("sender mode exiting");
}

//####################################################################################################

// void relay_mode(){
//   Serial.println("relay mode starting");

//   int quit = 1;
//   int counter = 0;

//   LoRa.onReceive(read_packet);
//   LoRa.receive();

//   while(quit != 0){
//     LoRa.beginPacket();
//     LoRa.println(counter);
//     LoRa.println(0);
//     LoRa.endPacket(); //too time

//     Serial.print("sending pkg no: ");
//     Serial.println(counter);

//     counter++;

//     // delay(2000);
//   }

//   LoRa.idle();
//   Serial.println("relay mode exiting");
// }

//####################################################################################################
//####################################################################################################

void setup() {
  Serial.begin(9600);

  Serial.println("LoRa Receiver");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  } else {
    Serial.println("SSD1306 Started");
  }

  while (!Serial);
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
  } else {
    Serial.println("LoRa Started");
  }

  LoRa.setTxPower(18);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(31.25E3);
  LoRa.setCodingRate4(8);
}

//####################################################################################################

void loop() {
  slt = menu();

  switch (slt) {
    case 1+48:
      recive_mode();
      break;
    case 2+48:
      send_mode("ciro");
      break;
    case 3+48:
      //relay_mode_capo();
      break;
    case 4+48:
      //ralay_mode_schiavo();
      break;
    case 5+48:
      rssi_monitor_mode();
      break;
    case 6+48:
      break;
    case 7+48:
      set_gain();
      break;

    default:
      break;
  }
}
