#include <Wire.h>
#define I2C_SLAVE1_ADDRESS 11
#define I2C_SLAVE2_ADDRESS 12
#define I2C_SLAVE3_ADDRESS 13
#define PAYLOAD_SIZE 2

//     A
//    ---
// F |   | B
//   | G |
//    ---
// E |   | C
//   |   |
//    ---
//     D

int NUMBERS[3]; // 3 random numbers stored from the I2C protocol communications

void setup(){
  Wire.begin();      
  Serial.begin(9600);  
  Serial.println(F("-------------------------------------I am the Master"));
}

void loop(){
  Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
  Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1); 
  NUMBERS[0] = Wire.read();
  Wire.endTransmission();
  Serial.print("first recieved value : ");
  Serial.println(NUMBERS[0]);
  
  Wire.beginTransmission(I2C_SLAVE2_ADDRESS);
  Wire.requestFrom(I2C_SLAVE2_ADDRESS, 1); 
  NUMBERS[1] = Wire.read();
  Wire.endTransmission();
  Serial.print("second recieved value : ");
  Serial.println(NUMBERS[1]);

  Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
  Wire.requestFrom(I2C_SLAVE3_ADDRESS, 1); 
  NUMBERS[2] = Wire.read();
  Wire.endTransmission();
  Serial.print("third recieved value : ");
  Serial.println(NUMBERS[2]);
  delay(1000);
}
