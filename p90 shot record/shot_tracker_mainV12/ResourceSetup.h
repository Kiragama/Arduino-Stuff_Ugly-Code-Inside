#ifndef ResourceSetup_h
#define ResourceSetup_h



class Resources_Base {
private:
  int ammo = 10; //just the display value
  int lastammo = 0;
  int16_t keyPressed;
  float magazine; //actual value for the magazine


public:
  void setMag(float mag) {
    magazine = mag;
  }
  float getMag() {
    return magazine;
  }
  int16_t getkeyPressed() {
    return keyPressed;
  }
  void setkeyPressed(int16_t i) {
    keyPressed = i;
  }
  void setammo(int i) {
    ammo = i;
  }
  int getammo() {
    return ammo;
  }
  void setlastammo(int i) {
    lastammo = i;
  }
  int getlastammo() {
    return lastammo;
  }
  int8_t refreshCounter;
  
};




#endif