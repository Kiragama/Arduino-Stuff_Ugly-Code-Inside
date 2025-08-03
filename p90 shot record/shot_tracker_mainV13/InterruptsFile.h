#pragma once

void IRAM_ATTR reload() {  //Reload being an interrupt is good as I can reload whenever
  resetMag = true;
  //Serial.println("reload");
  last_time = millis();  //reset sleep timer
  once = true;           //so triggering reload displays disconnected afterwards. If disconnected
}


void IRAM_ATTR reloadRF() {  //Reload being an interrupt is good as I can reload whenever
  resetMag = true;
  //Serial.println("RFreload");
  last_time = millis();  //reset sleep timer
  once = true;           //so triggering reload displays disconnected afterwards. If disconnected
}


void IRAM_ATTR pair() {        //Reload being an interrupt is good as I can reload whenever
  if (!isInPairMode) {         //uses bool to flag as in pair mode but also to stop this interrupt from happening alot
    Serial.print("PairMode");  //I don't care about serial prints while doing pairmode
    isInPairMode = true;
    
  }
}