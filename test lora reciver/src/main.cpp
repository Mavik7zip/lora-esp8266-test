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

#define ss 15
#define rst 16
#define dio0 0


struct strt_pkt{
  String text;
  int rssi;
  int snr;
};

void print_data(strt_pkt packet){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(F("Rssi: "));
  display.print(packet.rssi);
  display.setCursor(0,30);
  display.print(F("Snr: "));
  display.print(packet.snr);
  display.display();
}

void read_packet(int packet_size){
  strt_pkt packet;

  for (int i = 0; i < packet_size; i++){
    packet.text += (char)LoRa.read();
  }
  packet.rssi = LoRa.packetRssi();
  packet.snr = LoRa.packetSnr();

  print_data(packet);
}

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  Serial.println("LoRa Receiver");

  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
  }

  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(62.5E3);

  LoRa.onReceive(read_packet);
}

void loop(){
}
