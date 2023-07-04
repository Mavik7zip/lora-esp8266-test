//codice main

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Dichiarazione per il display SSD1306 connesso all'I2C
#define OLED_RESET -1 // Pn di reset (-1 se condivide lo stesso dell'arduino)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// pin del modulo lora
#define NSS 16
#define RST 0
#define DIO0 15

#define send_delay 2000

int slt;
// srand(time(NULL)); bo non gli piace
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

// ####################################################################################################

void send_mode(String message);

// ####################################################################################################

void print_display() {
  display.clearDisplay(); // odio i display
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

// ####################################################################################################

void print_serial() {
  Serial.println("    text: " + String(packet.text));
  Serial.println("  No pkt: " + String(packet.counter));
  Serial.println("    ping: " + String(packet.ping));
  Serial.println("    rssi: " + String(packet.rssi));
  Serial.println("     snr: " + String(packet.snr));
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
}

// ####################################################################################################

int menu() {
  while (!Serial);

  Serial.println("receiver mode      [1]");
  Serial.println("sender mode        [2]");
  Serial.println("bidirectional mode [3]");
  // Serial.println("relay slave        [4]");
  Serial.println("message mode       [5]");
  Serial.println("settings menu      [6]");
  Serial.println("                      ");
  Serial.println("quit               [0]");

  while (!Serial.available());

  return (int)Serial.read();
}

int settings_menu() {
  while (!Serial);

  Serial.println("set gain           [1]");
  Serial.println("set bandwidth      [2]");
  Serial.println("set txpower        [3]");
  Serial.println("rssi radio         [4]");
  Serial.println("                      ");
  Serial.println("quit               [0]");

  while (!Serial.available());
  return (int)Serial.read();
}

// ####################################################################################################

void set_gain() {
  Serial.println("value: 0-6 (0 => automatico)");

  while (!Serial.available());
  int gain = ((int)Serial.read() - 48);

  if (gain <= 6 && gain >= 0) {
    LoRa.setGain(gain);
    Serial.println("gain = " + String(gain));
  }  else {
    Serial.println("value not set");
  }

  Serial.println();
}

// ####################################################################################################

void set_bandwidth() {
  Serial.println("(7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500)(31.25 => default)");

  while (!Serial.available());
  double bandwidth = (Serial.readString()).toDouble(); //fa cacare il read string

  LoRa.setSignalBandwidth(bandwidth * 1000);
  Serial.println("bandwidth = " + String(bandwidth));

  Serial.println();
}

// ####################################################################################################

void set_txpower() {
  Serial.println("(2-20)(18 => default)");

  while (!Serial.available());
  int dbm = Serial.parseInt();

  LoRa.setTxPower(dbm);
  Serial.println("bandwidth = " + String(dbm));

  Serial.println();
}

// ####################################################################################################

void rssi_radio() {
  Serial.println("rssi radio: ");

  Serial.println(LoRa.rssi());
}

// ####################################################################################################

void recive_mode() {
  Serial.println("recive mode starting");

  int quit = 1;

  LoRa.onReceive(read_packet);
  LoRa.receive();

  while (quit != 0) {
    quit = Serial.read(); // due mezzi fake
    if (packet.is_arrive == true) {
      print_serial();
      packet.is_arrive = false;
    }

    delay(200);
  }

  LoRa.idle();
  Serial.println("recive mode exiting");
}

// ####################################################################################################

void send_data(String message, int counter, int id) {
  LoRa.beginPacket();
  LoRa.println(message);
  LoRa.println(counter);
  LoRa.println(id);
  LoRa.endPacket();
}

// ####################################################################################################

void send_mode(String message) {
  Serial.println("sender mode starting");

  int quit = 1;
  int counter = 0;

  while (quit != 0) {
    send_data(message, counter, id);

    Serial.print("sending pkg n°: ");
    Serial.println(counter);

    counter++;

    delay(send_delay);
  }

  Serial.println("sender mode exiting");
}

// ####################################################################################################

void send_message_mode() {
  Serial.println("send message mode starting");

  int quit = 1;
  int counter = 0;
  String message;

  while (quit != 0) {

    while (!Serial.available());
    message = Serial.readString();

    send_data(message, counter, 0);
    Serial.println(message + String(counter));

    counter++;

    delay(send_delay);
  }

  Serial.println("send message mode exiting");
}

// ####################################################################################################

void bidirectional_mode() {
  Serial.println("bidirectional mode starting");

  int quit = 1;
  int counter = 0;

  LoRa.onReceive(read_packet);
  LoRa.receive();

  while (quit != 0) {

    if (packet.is_arrive == true) {
      if (packet.id != id) {
        send_data("bidirectional test", packet.counter, id);
        packet.is_arrive = false;
      }
    } else {
      print_serial();
    }

    if (counter % 4 == 0) {
      send_data("bidirectional test", counter / 4, id);
      Serial.println("sending No" + String(counter / 4));
    }

    counter++;

    delay(send_delay / 2);
  }

  LoRa.idle();
  Serial.println("bidirectional mode exiting");
}

// ####################################################################################################
// ####################################################################################################

void setup() {
  delay(1000);

  Serial.begin(115200);
  Serial.setTimeout(3000);

  Serial.println();
  Serial.println();
  Serial.println("LoRa Main code");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //Address 0x3D for 128x64
    Serial.println("SSD1306 starting fail");
    // while (1); //uncommentare perchè se non va almeno posso usarre la seriale
  } else Serial.println("SSD1306 started");

  // scrivo le prime cose cose sul display
  //display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(19, 26);
  display.print("LoRa Main");
  display.display();

  while (!Serial); // aspetto la seriale

  // starto l'antenna a 433 MHz
  LoRa.setPins(NSS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("started LoRa");

    while (1);
  }

  LoRa.setTxPower(18);              // setto la potenza d'uscita (18 dBm)
  // LoRa.setGain(6);                  // setto il guadagno in ingresso (0-6 dB con 0 è automatico)
  LoRa.setSpreadingFactor(12);      // setto il fattore di diffusione (12 mele)
  LoRa.setSignalBandwidth(31.25E3); // setto la larghezza di banda (7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500)
  LoRa.setCodingRate4(8);           // setto la velocità di codifica (8)
}

// ####################################################################################################

void loop() {
  slt = menu();
  int settings_slt = 99;

  switch (slt) {
    case 1 + 48:
      recive_mode();
      break;
    case 2 + 48:
      send_mode("ciro");
      break;
    case 3 + 48:
      bidirectional_mode();
      break;
    case 4 + 48:
      // ralay_slave();
      break;
    case 5 + 48:
      send_message_mode();
      break;
    case 6 + 48:
      while (settings_slt != 48) {
        settings_slt = settings_menu();

        switch (settings_slt) {
          case 0:
            slt = 0;
            break;
          case 1 + 48:
            set_gain();
            break;
          case 2 + 48:
            set_bandwidth();
            break;
          case 3 + 48:
            set_txpower();
            break;
          case 4 + 48:
            rssi_radio();
            break;

          default:
            break;
        }
      } break;

    default:
      break;
  }
}
