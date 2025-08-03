#pragma once

class W_IR {

public:
  W_IR();
  int16_t irMyController();
  int getRemote(int i) {
    return remote[i];
  }

protected:
  const int remote[20] = {
    0x16,  //zero 0
    0xC,   //one 1
    0x18,  //two 2
    0x5E,  //three 3
    0x8,   //four 4
    0x1C,  //five 5
    0x5A,  //six 6
    0x42,  //seven 7
    0x52,  //eight 8
    0x4A,  //nine 9
    0xD,   //clear 10
    0x19,  //down 11
    0x45,  //Power 12
    0x47,  //Menu 13
    0x44,  //Test 14
    0x40,  //up 15
    0x43,  //back 16
    0x7,   //left 17
    0x15,  //confirm 18
    0x9,   //right 19
  };
};

extern W_IR irUse;
