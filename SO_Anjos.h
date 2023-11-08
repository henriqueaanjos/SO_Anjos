#ifndef SO_ANJOS_H
#define SO_ANJOS_H

#include "Arduino.h"
#include "libraries/Adafruit_GFX_Library/Adafruit_GFX.h"
#include "libraries/Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "libraries/SPI/SPI.h"
#include "libraries/OneButton/src/OneButton.h"
#include "libraries/NewTone/src/NewTone.h"
#include "libraries/OneWire/OneWireMod.h"
#include "libraries/SCoop/SCoop.h"

#define TFT_CS 34
#define TFT_DC 33
#define V_Bat A7

#define P1 A3
#define P2 A4
#define P3 A5
#define P4 A6
#define P6 A12
#define P7 A13
#define P8 A14
#define P5 A15

#define m1 1 		
#define m2 2
#define m3 3
#define m4 4 

#define A 10  
#define B 11
#define C 12
#define D 13


class SO_Anjos{
    public:   
        static int startProgram;
        static int setupProgram;

        int ButtonDL = 48;
        int ButtonUL = 49;
        int ButtonDR = 38;
        int ButtonUR = 47;

        SO_Anjos();
        void BAT(int a);
        static void printTexto(String text, uint16_t color, int x, int y,int textSize);
        static void printNumero(int text, uint16_t color, int x, int y,int textSize);
        static int getTouchSensorValue(int port);
        static byte getColorSensorValue(int port);
        static byte getUtrasonicSensorValue(int port);
        static long getCodePin(uint8_t code);
        static void setMotorPin(byte motor, int value);
        void updateValues();
        void startSO();

        static void clickButtonDownRight();
        static void clickButtonUpRight();
        static void clickButtonDownLeft();
        static void clickButtonUpLeft();

        void tickButtons();
        static Adafruit_ILI9341 tft;



    private:
        static int option;
        static int tela;

        static int velocidadeM1;
        static int velocidadeM2;
        static int velocidadeM3;
        static int velocidadeM4;

        static uint8_t CodeFlag1; 	
        static uint8_t CodeFlag2;	
        static uint8_t CodeFlag3; 
        static uint8_t CodeFlag4; 

        static long  MotorCodeCnt1;	
        static long  MotorCodeCnt2;	
        static long  MotorCodeCnt3;	
        static long  MotorCodeCnt4;

        static uint8_t MotorFlag1;
        static uint8_t MotorFlag2;
        static uint8_t MotorFlag3;
        static uint8_t MotorFlag4 ;

        static uint8_t PWMB1;
        static uint8_t PWMB2;

        static uint8_t PWMA1;
        static uint8_t PWMA2;

        static uint8_t PWMC1;
        static uint8_t PWMC2;

        static uint8_t PWMD1;
        static uint8_t PWMD2;

        static int motorSelected;

        static unsigned long previousMillis;
        static const long interval; 

        static unsigned long previousMillisUltrasonic;
        static const long intervalUltrasonic;

        static uint8_t cmd1 [2]; //comando para configurar o sensor de cor
        static uint8_t cmd2 [2]; //comando para pedir os valores do código da cor e a cor vermelha em rgb
        static uint8_t cmd3 [2]; //comando para pedir os valores das cores verde e azul em rgb
        static uint8_t CodigoVermelho [2]; 
        static uint8_t VerdeAzul [2];
        static uint8_t controle;

        static uint8_t cmd;
        static int dist;

        

        static int Filtro_Analog(int porta, int amostra, int diferenca);
        static int getDigitalPin(uint8_t pin);
        static void getCode1Interrupt();
        static void getCode2Interrupt();
        static void getCode3Interrupt();
        static void getCode4Interrupt();
        static void setCodeInit(uint8_t interrupt);
        static void TELA_MENU();
        static void telaMotores();
        static void telaSensores();
        static void telaProgram();
        static void updateSelection();
};


#endif