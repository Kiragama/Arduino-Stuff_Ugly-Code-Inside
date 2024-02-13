#include <SPI.h>
#include <RFID.h>
// D10:pin of card reader SDA. D9: pin of card reader RST
RFID rfid(10, 9);
// 4-byte card serial number, the fifth byte is check byte
unsigned char serNum[5];
unsigned char status;
unsigned char str[MAX_LEN];
unsigned char blockAddr; //Select the operation block address: 0 to 63

const int key = 69; //This is nice and simple as it just compares who sector to 69. For specific key, can use char array without much issue
const int sector = 4;

// The A password of original sector: 16 sectors; the length of password in each sector is 6 bytes.
unsigned char sectorKeyA[16][16] = {
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } ,
};
void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.init();
  Serial.println("Please put the card to the induction area...");
}
void loop()
{
  //find the card
  rfid.findCard(PICC_REQIDL, str);
  //Anti-collision detection, read serial number of card
  if (rfid.anticoll(str) == MI_OK) {
    Serial.print("The card's number is : ");
    //print the card serial number
    for (int i = 0; i < 4; i++) {
      Serial.print(0x0F & (str[i] >> 4), HEX);
      Serial.print(0x0F & str[i], HEX);
    }
    Serial.println("");
    memcpy(rfid.serNum, str, 5);
  }
  //select card and return card capacity (lock the card to prevent multiple read and written)
  rfid.selectTag(rfid.serNum);

  readCard(sector); //Read this sector and continue
  rfid.halt();
}

//read the card
void readCard(int blockAddr) {
  if ( rfid.auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[blockAddr / 4], rfid.serNum) ==
       MI_OK) // authenticate
  {
    // select a block of the sector to read its data
    Serial.print("Read from the blockAddr of the card : ");
    Serial.println(blockAddr, DEC);
    if ( rfid.read(blockAddr, str) == MI_OK) {
      Serial.print("The data is (char type display): ");
      Serial.println((char *)str);
      Serial.print("The data is (HEX type display): ");
      /*for (int i = 0; i < sizeof(str); i++) {
        Serial.print(str[i], HEX);
        Serial.print(" ");
        }*/
      dispSector();
      validateSector();
      Serial.println();
    }
  }
}
void dispSector() { // reads out variable contents
  for (int i = 0; i < sizeof(str); i++) {
    Serial.print(str[i], HEX);
    Serial.print(" ");
  }
}

bool validateSector() {
  int counter = 0;
  for (int i = 0; i < sizeof(str); i++) {
    if (str[i], HEX == key, HEX)//compare value to key and ensure it comparing it in hex (will compare memory values otherwise.
    {
      Serial.print("Good");
      counter++; //records every good
    }
    else
    {
      Serial.print("bad");
    }
  }
  if (counter == sizeof(str)); //ensure that all good are equal to the block size
  {
    Serial.print("Sector: "); //<--
    Serial.print(sector);
    Serial.print(" Verified with Key");
    
  }
}
