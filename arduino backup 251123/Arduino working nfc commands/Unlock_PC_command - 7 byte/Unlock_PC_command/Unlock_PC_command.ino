#include <Keyboard.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (8)
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
   #define Serial SerialUSB
#endif

char ctrlK = KEY_LEFT_CTRL;

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
  nfc.begin();
  
  Serial.println("Hello World!");
  pinMode(PN532_RESET, OUTPUT);

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Cannot find PN53x board");
    while (1);
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  nfc.SAMConfig();
  //config board to read RFID tags

  Serial.println("Waiting for an ISO14443A Tag ");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; //buffer to store the returned UID
  uint8_t uidLength;

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success)
  {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 7)
    {
        uint8_t data[32];
        success = nfc.ntag2xx_ReadPage (7, data);

        if (success)
        {
          Serial.println("Reading Page 7");
          if(data[1] == 0x65)
          {
            Serial.println("authenticated");
            //type pass here
            Keyboard.print(ctrlK);
            delay(100);
            Keyboard.print(ctrlK);
            delay(100);
            Keyboard.println("115935"); //Types then presses enter
            delay(2000);
          }
          else
          {
            Serial.println("NOPE!");
            digitalWrite(PN532_RESET, HIGH);
          }          
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
      }
    }
  
