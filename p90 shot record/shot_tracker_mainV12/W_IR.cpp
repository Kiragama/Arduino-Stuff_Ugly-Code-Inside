#include <IRremote.hpp>


#include "W_IR.h"
#include "Display.h"
// This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
#include "ResourceSetup.h"

Display_Stuff irdisplay;



W_IR::W_IR() {
  IrReceiver.begin(irPin);
}


//////////////////////////////////////////////////////////////////////////////////////////

int16_t W_IR::irMyController() {  //scans for input from remote and saves
  irdisplay.clearInput();
  if (IrReceiver.decode()) {
    IrReceiver.printIRSendUsage(&Serial);                   //print whole results
    Serial.println(IrReceiver.decodedIRData.command, HEX);  //print command in hex
    size_t size = sizeof(remote) / sizeof(remote[0]);  //gets size of array remote
    //ONCE MADE AN ARRAY, TRY TO DO THIS AS A FOR LOOP. MAYBE ANOTHER ARRAY THAT INCLUDES THE ACTUAL COMMANDS? I ARRAY THAT CONTAINS THE DATA CHECKED AND ANOTHER THAT INCLUDED THE CORRESPONDING COMMAND
    for (int16_t i = 0; i < size; i++) {                    //cycles through whole document until hit value it needs
      if (IrReceiver.decodedIRData.command == remote[i]) {  //compared the command (hex value of the button) to the array
        
        IrReceiver.resume();  // Enable receiving of the next value
        delay(700); //stops double reads
        return i;
      }
    }
    
  }
  IrReceiver.resume();  // Enable receiving of the next value
  delay(700);//stops double reads
    return 69;
}
////////////////////////////////////////////////////////////////////////////////////////
