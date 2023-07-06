//codice main

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Dichiarazione per il display SSD1306 connesso all'I2C
#define SCREEN_WIDTH 128 // larghezza in pixel
#define SCREEN_HEIGHT 64 // altezza in pixel
#define OLED_RESET    -1 // (pin di reset, -1 è lo stesso pin di arduino)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Pin del modulo lora
#define NSS 16
#define RST 0
#define DIO0 15

// Intervallo tra due pacchetti
#define send_delay 2000

// Valori radio di dafault
#define defTxPower 18       // potenza Tx (0-18 dBm, potrebbe variare con altri moduli)
#define defRxGain 0         // guadagno Rx (0-6 dB, con 0 è automatico)
#define defSpredFactor 12   // fattore di diffusione (12 mele)
#define defSignBand 125E3   // larghezza di banda (7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500 + E3)
#define defCodRate 8        // velocità di codifica (8 banane)

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
  Serial.println("\r\n    text: " + String(packet.text));
  Serial.println("  No pkt: " + String(packet.counter));
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
        case 0: packet.text = tmp;            break;
        case 1: packet.counter = tmp.toInt(); break;
        case 2: packet.id = tmp.toInt();      break;
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

  Serial.println("----------------------");
  Serial.println("receiver mode      [1]");
  Serial.println("sender mode        [2]");
  Serial.println("bidirectional mode [3]");
  // Serial.println("relay slave        [4]");
  Serial.println("message mode       [5]");
  Serial.println("settings menu      [6]");
  Serial.println("                      ");
  Serial.println("quit               [0]");
  Serial.println("----------------------");


  while (!Serial.available());

  return (int)Serial.read();
}

int settings_menu() {
  while (!Serial);

  Serial.println("----------------------");
  Serial.println("set gain           [1]");
  Serial.println("set bandwidth      [2]");
  Serial.println("set txpower        [3]");
  Serial.println("rssi radio         [4]");
  Serial.println();
  Serial.println("quit               [0]");
  Serial.println("----------------------");

  while (!Serial.available());
  return (int)Serial.read();
}

// ####################################################################################################

void set_gain() {
  Serial.println("\r\nvalue: 0-6 (0 => automatico)");

  delay(1000);

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
  Serial.println("(7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500)(125 => default)");

  delay(1000);

  while (!Serial.available());
  double bandwidth = (Serial.readString()).toDouble(); //fa cacare il read string

  LoRa.setSignalBandwidth(bandwidth * 1000);
  Serial.println("bandwidth = " + String(bandwidth));

  Serial.println();
}

// ####################################################################################################

void set_txpower() {
  Serial.println("(2-20)(18 => default)");

  delay(1000);

  while (!Serial.available());
  int dbm = Serial.parseInt();

  LoRa.setTxPower(dbm);
  Serial.println("txpower = " + String(dbm));

  Serial.println();
}

// ####################################################################################################

void rssi_radio() {
  Serial.println("rssi radio: " + String(LoRa.rssi()) + "\r\n");
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
    message = read_string();

    send_data(message, counter, 0);
    Serial.println(message + String(counter));

    counter++;
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
      if (packet.id == id) {
        if (counter % 4 == 0) {
          print_serial();
        }
      } else {
        send_data("bidirectional test", packet.counter, packet.id);
        LoRa.receive();
        packet.is_arrive = false;
      }
    }

    if (counter % 4 == 0) {
      send_data("bidirectional test", counter / 4, id);
      LoRa.receive();
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

  while (!Serial); // aspetto la seriale

  Serial.begin(115200);
  Serial.setTimeout(3000);

  Serial.println("\r\n\r\nLoRa Main code");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //Address 0x3D for 128x64
    Serial.println("SSD1306 starting fail");
    // while (1); //uncommentare perchè se non va almeno posso usarre la seriale
  } else {
    Serial.println("SSD1306 started");

    // scrivo le prime cose cose sul display
    //display.display();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(19, 26);
    display.print("LoRa Main");
    display.display();
  }

  // starto l'antenna a 433 MHz
  LoRa.setPins(NSS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("starting LoRa failed");
    display.display();

    while (1);
  } else {
    Serial.println("LoRa started succesfully");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("LoRa started succesfully");
    display.display();
  }

  //setto le impostazioni base della radio
  LoRa.setTxPower(defTxPower);              // setto la potenza d'uscita (18 dBm)
  LoRa.setGain(defRxGain);                  // setto il guadagno in ingresso (0-6 dB con 0 è automatico)
  LoRa.setSpreadingFactor(defSpredFactor);  // setto il fattore di diffusione (12 mele)
  LoRa.setSignalBandwidth(defSignBand);     // setto la larghezza di banda (7.8 10.4 15.6 20.8 31.25 41.7 62.5 125 250 500)
  LoRa.setCodingRate4(defCodRate);          // setto la velocità di codifica (8)
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
