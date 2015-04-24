#include <Wire.h>

#define EEP_I2C_ADDR 80 //This is the EEPROM Base Adress
#define EEPROMWP   4    //Write protect PIN
#define PAGESIZE 16
#define PROMCOUNT 8

void startTest()
{
  for (int i = 0; i < PROMCOUNT; i++) {

    int promName = i + 1;

    String message = "Testing EEPROM ";
    String out = String();
    out = message + promName;
    Serial.println(out);

    bool error = false;

    //Handle pages a PAGESIZE length
    for (uint16_t page = 0; page < (65536 / PAGESIZE); page++)
    {
      if (!testPage(i, page))
      {
        error = true;
        break;
      }
    }

    if (error)
    {
      String error = "\r\nEEPROM ";
      error = error + " " + promName + " has error or is not pressent.";
      Serial.println(error);
    }
  }

  Serial.println("");
}

bool testPage(int prom, uint16_t page)
{
  String message = "Testing page: ";
  String out = message + page;

  Serial.print("\r" + out);

  uint16_t eeaddress = (uint16_t)page * (uint16_t)PAGESIZE;
  int writeData[PAGESIZE];

  //Write a page of PAGESIZE bytes
  Wire.beginTransmission(EEP_I2C_ADDR + prom);
  Wire.write((uint16_t)(eeaddress >> 8)); // MSB
  Wire.write((uint16_t)(eeaddress & 0xFF)); // LSB

  //Do PAGESIZE writes
  for (int pageWrite = 0; pageWrite < PAGESIZE; pageWrite++)
  {
    byte randNumber = random(255);
    writeData[pageWrite] = randNumber;
    Wire.write(randNumber); //Data
  }

  Wire.endTransmission();
  delay(6);

  //Read a page of PAGESIZE bytes
  int readCount = 0;
  while (readCount != PAGESIZE)
  {
    uint16_t readAddress = (uint16_t)eeaddress + (uint16_t)readCount;

    //Read
    Wire.beginTransmission(EEP_I2C_ADDR + prom);
    Wire.write((uint16_t)(readAddress >> 8)); // MSB
    Wire.write((uint16_t)(readAddress & 0xFF)); // LSB
    Wire.endTransmission();

    Wire.requestFrom(EEP_I2C_ADDR + prom, PAGESIZE - readCount);
    if (!Wire.available())
    {
      return false;
    }

    while (Wire.available())
    {
      byte readByte = Wire.read();
      if (readByte != writeData[readCount])
      {
        return false;
      }

      readCount++;
    }
  }

  return true;
}

void startReset()
{
  for (int i = 0; i < PROMCOUNT; i++) {

    int promName = i + 1;

    String message = "Rest EEPROM ";
    String out = String();
    out = message + promName;
    Serial.println(out);

    //Handle pages a PAGESIZE length
    for (uint16_t page = 0; page < (65536 / PAGESIZE); page++)
    {
      resetPage(i, page);
    }
  }
  Serial.println("");
}

void resetPage(int prom, uint16_t page)
{
  String message = "Resetting page: ";
  String out = message + page;

  Serial.print("\r" + out);

  uint16_t eeaddress = (uint16_t)page * (uint16_t)PAGESIZE;

  //Write a page of PAGESIZE bytes
  Wire.beginTransmission(EEP_I2C_ADDR + prom);
  Wire.write((uint16_t)(eeaddress >> 8)); // MSB
  Wire.write((uint16_t)(eeaddress & 0xFF)); // LSB

  //Do PAGESIZE writes
  for (int pageWrite = 0; pageWrite < PAGESIZE; pageWrite++)
  {
    Wire.write(0); //Data
  }

  Wire.endTransmission();
  delay(6);
}

void setup() {
  // put your setup code here, to run once

  Wire.begin();
  Serial.begin(9600);

  pinMode(EEPROMWP, OUTPUT);
  digitalWrite(EEPROMWP, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:

  while ( !Serial );
  Serial.println("I2E EEPROM tester.");
  Serial.println("Press T for test and R for reset");

  Serial.flush(); //flush all previous received and transmitted data

  while (!Serial.available());

  char inchar = Serial.read();

  if (  inchar == 84 || inchar == 116) {
    startTest();
  } else if (  inchar == 82 || inchar == 114) {
    startReset();
  }
}
