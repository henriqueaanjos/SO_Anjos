#include <SO_Anjos.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <OneButton.h>
#include <NewTone.h>
#include <OneWireMod.h>
#include <SCoop.h>

SO_Anjos so = SO_Anjos();

//////////////////////////////////////////////////////// TASK 1////////////////////////////////////////////////////////
defineTask(scoopTask1)
void scoopTask1::setup() {
  so.startSO();
}
void scoopTask1::loop() {}

//////////////////////////////////////////////////////// TASK 2////////////////////////////////////////////////////////
defineTask(scoopTask2)
void scoopTask2::setup() {}
void scoopTask2::loop() {
  if(so.startProgram && so.setupProgram){
    //loop do programa desenvolvido
  }
}

//////////////////////////////////////////////////////// SETUP ////////////////////////////////////////////////////////
void setup(){
  mySCoop.start();
}

//////////////////////////////////////////////////////// LOOP ////////////////////////////////////////////////////////
void loop(){
  yield();
  so.updateValues();
  if(so.startProgram){
    if(!so.setupProgram){
      // setup do programa desenvolvido
      so.setupProgram = 1;
    }
  }
  so.tickButtons();
}