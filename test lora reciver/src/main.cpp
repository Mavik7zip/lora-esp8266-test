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


struct strt_pkt{
  String text;
  int no;
  int dev_ping;
  int rssi;
  int snr;
};

void print_display(strt_pkt packet){
  display.clearDisplay(); //odio i display
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

void print_serial(strt_pkt packet){
  Serial.println("text");
  Serial.println(packet.text);
  Serial.println("no pkt");
  Serial.println(packet.no);
  // Serial.println("dev ping");
  // Serial.println(packet.dev_ping);
  Serial.println("rssi");
  Serial.println(packet.rssi);
  Serial.println("snr");
  Serial.println(packet.snr);
}

void print_data(strt_pkt packet){
  print_serial(packet);
  print_display(packet);
}

void read_packet(int packet_size){
  strt_pkt packet;
  String tmp;
  char tmp_char;
  int j = 0;
  int timestamp = millis(); // + base_time;

  
  for (int i = 0; i < packet_size; i++){
    tmp_char = (char)LoRa.read();

    if(tmp_char != '\n'){
      tmp += tmp_char;
    }else{

      switch (j){
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
  // packet.dev_ping = timestamp - packet.dev_ping;

  print_data(packet);
}

void setup() {
  Serial.begin(9600);

  Serial.println("LoRa Receiver");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  while (!Serial);
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    delay(100);
    while (1);
  }else{
    Serial.println("Starting LoRa");
  }

  LoRa.setTxPower(18);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(7.8E3);
  LoRa.setCodingRate4(8);

  LoRa.onReceive(read_packet);
  LoRa.receive();
}

void loop(){
  // Serial.println("it work perfectly");
  // delay(2000);
}
