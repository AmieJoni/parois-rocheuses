//LoRaWAN
#include "LoRaWANNode.h"
#define FRAME_DELAY 300000  // in ms. Every 5 minutes by default.
//VL53L0X
#include <Wire.h>
#include <vl53l0x_class.h>

//LoRaWAN
// Serial port use to communicate with the USI shield.
// By default, use D0 (Rx) and D1(Tx).
// For Nucleo64, see "Known limitations" chapter in the README.md
HardwareSerial SerialLora(D0, D1);

// AppKey and AppEUI.
const char appKey[] = "112233445566778899aabbccddeeff00";
const char appEUI[] = "0000000000000000";

// Data send
char frameTx[4];

//VL53L0X
// Create components.
TwoWire WIRE1(PB11, PB10);  //SDA=PB11 & SCL=PB10
VL53L0X sensor_vl53l0x(&WIRE1, PC6, PC7); //XSHUT=PC6 & INT=PC7

void setup()
{   
  //LoRaWAN & VL53L0X
   Serial.begin(9600);
   
  //VL53L0X
  // Initialize I2C bus.
  WIRE1.begin();
  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();
  // Initialize VL53L0X top component.
  int status = sensor_vl53l0x.InitSensor(0x10);
  if(status)
  {
    Serial.println("Init sensor_vl53l0x failed...");
  }
  
  //LoRaWAN
  Serial.println("-- LoRaWAN OTAA sketch --");
  // Enable the USI module and set the radio band.
  while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial.println("Lora module not ready");
    delay(1000);
  }

  // Send a join request and wait the join accept
  while(!loraNode.joinOTAA(appKey, appEUI)) {
    Serial.println("joinOTAA failed!!");
    delay(1000);
  }

  Serial.println("Lora module ready, join accepted.\n");
  /*
  String str = "Device EUI: ";
  loraNode.getDevEUI(&str);
  Serial.println(str);
  str = "Application key: ";
  loraNode.getAppKey(&str);
  Serial.println(str);
  str = "Application EUI: ";
  loraNode.getAppEUI(&str);
  Serial.println(str);
  */
}

void loop()
{ 
  setData();
  transmit();
  delay(FRAME_DELAY);
}

void transmit(void) {
  // Send unconfirmed data to a gateway (port 1 by default)
  int status = loraNode.sendFrame(frameTx, sizeof(frameTx), UNCONFIRMED);
  if(status == LORA_SEND_ERROR) {
    Serial.println("Send frame failed!!!");
  } else if(status == LORA_SEND_DELAYED) {
    Serial.println("Module busy or duty cycle");
  } else {
    Serial.println("Frame sent");
  }
}

void setData() {
// Read Range.
  uint32_t distance;
  int status;
  status = sensor_vl53l0x.GetDistance(&distance);

  while(status != VL53L0X_ERROR_NONE){
    status = sensor_vl53l0x.GetDistance(&distance);
  }
  Serial.print("Distance [mm]:");
  Serial.println(distance);
  Serial.println(sizeof(distance));
  //split distance(32 bytes) to 4xbytes
  for(int i=0; i<4; i++) {
    if(i != 0){
      distance = distance >> 8;
    }
    byte data = distance & 255;
    frameTx[i] = data;
    Serial.print("Byte:");
    Serial.println(data);
  }

}
