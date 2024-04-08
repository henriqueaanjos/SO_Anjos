#include "Arduino.h"
#include "SO_Anjos.h"
#include "libraries/Adafruit_GFX_Library/Adafruit_GFX.h"
#include "libraries/Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "libraries/SPI/SPI.h"
#include "libraries/OneButton/src/OneButton.h"
#include "libraries/NewTone/src/NewTone.h"
#include "libraries/OneWire/OneWireMod.h"
#include "libraries/SCoop/SCoop.h"

int SO_Anjos::option = 0;
int SO_Anjos::tela = 0;

int SO_Anjos::velocidadeM1 = 0;
int SO_Anjos::velocidadeM2 = 0;
int SO_Anjos::velocidadeM3 = 0;
int SO_Anjos::velocidadeM4 = 0;

uint8_t SO_Anjos::CodeFlag1  = 0; 	
uint8_t SO_Anjos::CodeFlag2  = 0;	
uint8_t SO_Anjos::CodeFlag3  = 0; 
uint8_t SO_Anjos::CodeFlag4  = 0; 

long  SO_Anjos::MotorCodeCnt1= 0;	
long  SO_Anjos::MotorCodeCnt2= 0;	
long  SO_Anjos::MotorCodeCnt3= 0;	
long  SO_Anjos::MotorCodeCnt4= 0;

uint8_t SO_Anjos::MotorFlag1 = 0;
uint8_t SO_Anjos::MotorFlag2 = 0;
uint8_t SO_Anjos::MotorFlag3 = 0;
uint8_t SO_Anjos::MotorFlag4 = 0;

uint8_t SO_Anjos::PWMB1 = 12;
uint8_t SO_Anjos::PWMB2 = 11;

uint8_t SO_Anjos::PWMA1 = 9;
uint8_t SO_Anjos::PWMA2 = 10;

uint8_t SO_Anjos::PWMC1 = 13;
uint8_t SO_Anjos::PWMC2 = 46;

uint8_t SO_Anjos::PWMD1 = 45;
uint8_t SO_Anjos::PWMD2 = 44;

int SO_Anjos::motorSelected = -1;

unsigned long SO_Anjos::previousMillis = 0;
const long SO_Anjos::interval = 260; 

unsigned long SO_Anjos::previousMillisUltrasonic = 0;
const long SO_Anjos::intervalUltrasonic = 1500;

uint8_t SO_Anjos::cmd1 [2] = {0x12, 0x15}; //comando para configurar o sensor de cor
uint8_t SO_Anjos::cmd2 [2] = {0x13, 0x15}; //comando para pedir os valores do código da cor e a cor vermelha em rgb
uint8_t SO_Anjos::cmd3 [2] = {0x14, 0x15}; //comando para pedir os valores das cores verde e azul em rgb
uint8_t SO_Anjos::CodigoVermelho [2] = {0x00, 0x00}; 
uint8_t SO_Anjos::VerdeAzul [2] = {0x00, 0x00};
uint8_t SO_Anjos::controle = 0;

uint8_t SO_Anjos::cmd = 0x13;
int SO_Anjos::dist =0;

int SO_Anjos::startProgram = 0;
int SO_Anjos::setupProgram = 0;

int ButtonDL = 48;
int ButtonUL = 49;
int ButtonDR = 38;
int ButtonUR = 47;


Adafruit_ILI9341 SO_Anjos::tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

SO_Anjos::SO_Anjos(){}

// OneButton SO_Anjos::buttonUpLeft(49,true); 
// OneButton SO_Anjos::buttonDownRight(38,true);
// OneButton SO_Anjos::buttonUpRight(47,true);
// OneButton SO_Anjos::buttonDownLeft(48,true);

OneButton buttonUpLeft(49,true); 
OneButton buttonDownRight(38,true);
OneButton buttonUpRight(47,true);
OneButton buttonDownLeft(48,true);



int SO_Anjos::Filtro_Analog(int porta, int amostra, int diferenca){    // GERALMENTE USA 15 PARA AMOSTRA, 8 PARA DIFERENÇA
  int reads = 0;
  int lastread = 0;
  int correntread = 0;
  for(int ii=0;ii<amostra;ii++){      
      reads+=analogRead(porta);      
  }
  correntread=reads/amostra;
  if((abs(correntread - lastread))>diferenca){      
      lastread=correntread;      
  }
  return lastread;
}

void SO_Anjos::BAT(int a ){
    tft.fillRect(199,10,17,9,ILI9341_DARKGREY);
    tft.setCursor(199, 11);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setTextWrap(true);
    tft.print(a);  
}

//////////////////////////////////////////////////////// FUNÇÕES TELA ////////////////////////////////////////////////////////

uint32_t SO_Anjos::hexStringToUInt(const char* hexString) {
    uint32_t result = 0;
    while (*hexString) {
        char digit = *hexString;
        if (digit >= '0' && digit <= '9') {
            result = (result << 4) | (digit - '0');
        } else if (digit >= 'A' && digit <= 'F') {
            result = (result << 4) | (digit - 'A' + 10);
        } else if (digit >= 'a' && digit <= 'f') {
            result = (result << 4) | (digit - 'a' + 10);
        }
        hexString++;
    }
    return result;
}

// Função para converter cor hexadecimal para RGB565 usando apenas bitwise
uint16_t SO_Anjos::hexToRGB565Bitwise(const char* hexColor) {
    uint32_t rgbValue = hexStringToUInt(hexColor);

    // Extrai os componentes Red, Green e Blue
    uint8_t red = (rgbValue >> 16) & 0xFF;
    uint8_t green = (rgbValue >> 8) & 0xFF;
    uint8_t blue = rgbValue & 0xFF;

    // Ajusta os valores para os bits correspondentes do formato RGB565 usando bitwise
    uint16_t rgb565 = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);

    return rgb565;
}

void SO_Anjos::paintScreen(uint16_t color){
  tft.fillRect(0,32,240,281,color);
}

void SO_Anjos::printTexto(String text, uint16_t color, int x, int y,int textSize){
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

void SO_Anjos::printNumero(int text, uint16_t color, int x, int y,int textSize){
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

//////////////////////////////////////////////////////// SENSORES ////////////////////////////////////////////////////////

int SO_Anjos::getTouchSensorValue(int port){
  switch(port){
    case 1: 
      return analogRead(P1) >= 600 ? 1 :0;
    case 2: 
      return analogRead(P2) >= 600 ? 1 :0;
    case 3: 
      return analogRead(P3) >= 600 ? 1 :0;
    case 4: 
      return analogRead(P4) >= 600 ? 1 :0;
    case 5: 
      return analogRead(P5) >= 600 ? 1 :0;
    case 6: 
      return analogRead(P6) >= 600 ? 1 :0;
    case 7: 
      return analogRead(P7) >= 600 ? 1 :0;
    case 8: 
      return analogRead(P8) >= 600 ? 1 :0;
  }
}

byte SO_Anjos::getColorSensorValue(int port){
  uint8_t p;
  switch(port){
    case 1: 
      p = P1;
      break;
    case 2: 
      p = P2;
      break;
    case 3: 
      p = P3;
      break;
    case 4: 
      p = P4;
      break;
    case 5: 
      p = P5;
      break;
    case 6: 
      p = P6;
      break;
    case 7: 
      p = P7;
      break;
    case 8: 
      p = P8;
      break;
  }
  OneWire color(p);
  if(color.reset_sensorCor()&&controle==0){
    color.write_bytes_100us(cmd1,2);
    color.reset_finalSensorCor();
    controle=1;
  }
  delay(64);
  if(color.reset_sensorCor()&&controle==1){
    color.write_bytes_100us(cmd2,2);
    color.reset_finalSensorCor();
    color.read_bytes_sensorCor(CodigoVermelho, 2);
    controle=2;
  }
  if(color.reset_sensorCor()&&controle==2){
    color.write_bytes_100us(cmd3,2);
    color.reset_finalSensorCor();
    color.read_bytes_sensorCor(VerdeAzul, 2);
    controle=0;
  }else{
    return 0;
  }
  return CodigoVermelho[0];
}

byte SO_Anjos::getUtrasonicSensorValue(int port){
  delay(254);
  uint8_t p;
  switch(port){
    case 1: 
      p = P1;
      break;
    case 2: 
      p = P2;
      break;
    case 3: 
      p = P3;
      break;
    case 4: 
      p = P4;
      break;
    case 5: 
      p = P5;
      break;
    case 6: 
      p = P6;
      break;
    case 7: 
      p = P7;
      break;
    case 8: 
      p = P8;
      break;
  }
  OneWire ultrasonic(p);
  if(ultrasonic.reset_ultrassom()){
    ultrasonic.write_100us(cmd);
    dist=ultrasonic.read_ultrassom();
    return dist;
  }else{
    return 0;
  }
}

//////////////////////////////////////////////////////// MOTORES ////////////////////////////////////////////////////////

int SO_Anjos::getDigitalPin(uint8_t pin){
	pinMode(pin,INPUT_PULLUP);
	return digitalRead(pin);
}

void SO_Anjos::getCode1Interrupt()	{
	uint8_t m_ISR,m_Dir;	
	m_ISR = getDigitalPin(2);	
	m_Dir = getDigitalPin(A8);	
	if(m_ISR^m_Dir)			
		MotorCodeCnt1++;	
	else
		MotorCodeCnt1--;	 
} 

void SO_Anjos::getCode2Interrupt()	{
	uint8_t m_ISR,m_Dir;	
	m_ISR = getDigitalPin(3);	
	m_Dir = getDigitalPin(A9);	
	if(m_ISR^m_Dir)			
		MotorCodeCnt2++;	
	else
		MotorCodeCnt2--;	
}

void SO_Anjos::getCode3Interrupt()	{
	uint8_t m_ISR,m_Dir;	
	m_ISR = getDigitalPin(18);	
	m_Dir = getDigitalPin(A10);	
	if(m_ISR^m_Dir)			
		MotorCodeCnt3++;	
	else
		MotorCodeCnt3--;	
}

void SO_Anjos::getCode4Interrupt()	{
	uint8_t m_ISR,m_Dir;	
	m_ISR = getDigitalPin(19);	
	m_Dir = getDigitalPin(A11);	
	if(m_ISR^m_Dir)			
		MotorCodeCnt4++;	
	else
		MotorCodeCnt4--;	
}  

void SO_Anjos::setCodeInit(uint8_t interrupt){	
	switch(interrupt)
	{
		case m1: attachInterrupt(0, getCode1Interrupt, CHANGE); break;
		case m2: attachInterrupt(1, getCode2Interrupt, CHANGE); break;
		case m3: attachInterrupt(5, getCode3Interrupt, CHANGE); break;
		case m4: attachInterrupt(4, getCode4Interrupt, CHANGE); break;
		default:break;		
	}
}

long SO_Anjos::getCodePin(uint8_t code){	
	if(CodeFlag1==0 && code==m1)
	{
		CodeFlag1 = 1;
		setCodeInit(code);		
	}
	else if(CodeFlag2==0 && code==m2)
	{
		CodeFlag2 = 1;
		setCodeInit(code);				
	}
	else if(CodeFlag3==0 && code==m3)
	{
		CodeFlag3 = 1;
		setCodeInit(code);				
	}
	else if(CodeFlag4==0 && code==m4)
	{
		CodeFlag4 = 1;
		setCodeInit(code);				
	}
	switch(code)
	{
		case m1: return	MotorCodeCnt1; break; 
		case m2: return	MotorCodeCnt2; break; 
		case m3: return	MotorCodeCnt3; break; 
		case m4: return	MotorCodeCnt4; break; 
		default:break;
	}
}
void SO_Anjos::resetCodePin(uint8_t code){
  switch(code)
	{
		case m1: 
    	MotorCodeCnt1 = 0;
      break; 
		case m2: 
    	MotorCodeCnt2 = 0;
      break; 
		case m3: 
    	MotorCodeCnt3 = 0;
      break; 
		case m4: 
    	MotorCodeCnt4 = 0;
      break; 
		default:break;
	}
}

void SO_Anjos::setMotorPin(byte motor, int value){
  
  if(value > 100) value = 100;
  else if(value < -100) value = -100;

  //-----------------------------------------------
    switch (motor)
    {
    case A:
      if (MotorFlag1 == 0){
        MotorFlag1 = 1;
        pinMode(PWMA1, OUTPUT);
        pinMode(PWMA2, OUTPUT);             
      }
      break;

    case B:
      if (MotorFlag2 == 0){
        MotorFlag2 = 1;
        pinMode(PWMB1, OUTPUT);
        pinMode(PWMB2, OUTPUT);             
      }
      break;

    case C:
      if (MotorFlag3 == 0)
      {
        MotorFlag3 = 1;
        pinMode(PWMC1, OUTPUT);
        pinMode(PWMC2, OUTPUT);           
      }
      break;

    case D:
      if (MotorFlag4 == 0)
      {
        MotorFlag4 = 1;
        pinMode(PWMD1, OUTPUT);
        pinMode(PWMD2, OUTPUT);           
      }
      break;

    default:break;
    }
    switch (motor)
    {
      case A:
        if (value == 0)
        {
          analogWrite(PWMA1, 255);
          analogWrite(PWMA2, 255);
        }
        else if (value>0)
        {
          analogWrite(PWMA1, map(value,0,100,0,255));
          analogWrite(PWMA2, 0);
          
          
        }
        else if (value<0)
        {
          analogWrite(PWMA2, map(value,-100,0,255,0));
          analogWrite(PWMA1, 0); 
        }
        break;

      case B:
        if (value == 0)
        {
          analogWrite(PWMB1, 255);
          analogWrite(PWMB2, 255);
        }
        else if (value>0)
        {
          analogWrite(PWMB1, map(value,0,100,0,255));
          analogWrite(PWMB2, 0);
        }
        else if (value<0)
        {
          analogWrite(PWMB2, map(value,-100,0,255,0));
          analogWrite(PWMB1, 0);
        }
        break;

      case C:
        if (value == 0)
        {
          analogWrite(PWMC1, 255);
          analogWrite(PWMC2, 255);
        }
        else if (value>0)
        {
          analogWrite(PWMC1, map(value,0,100,0,255));
          analogWrite(PWMC2, 0);
        }
        else if (value<0)
        {
          analogWrite(PWMC2, map(value,-100,0,255,0));
          analogWrite(PWMC1, 0);
        }
        break;

      case D:
        if (value == 0)
        {
          analogWrite(PWMD1, 255);
          analogWrite(PWMD2, 255);
        }
        else if (value>0)
        {
          analogWrite(PWMD1, map(value,0,100,0,255));
          analogWrite(PWMD2, 0);
        }
        else if (value<0)
        {
          analogWrite(PWMD2, map(value,-100,0,255,0));
          analogWrite(PWMD1, 0);
        }
        break;

      default:break;
    }
}

//////////////////////////////////////////////////////// TELAS ////////////////////////////////////////////////////////

void SO_Anjos::TELA_MENU(){
  tft.setRotation(2);
  buttonUpRight.attachClick(clickButtonUpRight);
  buttonDownRight.attachClick(clickButtonDownRight);
  buttonDownLeft.attachClick(clickButtonDownLeft);
  buttonUpLeft.attachClick(clickButtonUpLeft);
  tft.drawLine(0, 30, 240, 30, ILI9341_WHITE);             // DESENHA UMA LINHA PARA O MENU
  tft.fillRect(3,6,105,21,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA
  printTexto("MENU", ILI9341_WHITE,8,9,2);

  tft.drawRect(196, 6, 31, 18, ILI9341_WHITE);             // DESENHA A BATERIA
  tft.fillRect(227,11,3,8,ILI9341_WHITE);
  printTexto("%", ILI9341_WHITE,219,11,1);

  tft.fillRect(0,32,240,281,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA

  tft.fillRoundRect(20, 60, 200, 40, 6, ILI9341_DARKGREY);
  tft.drawRoundRect(19, 59, 201, 41, 6, ILI9341_GREEN);
  printTexto("PROGRAMACAO",ILI9341_WHITE,22,68,3);

  tft.fillRoundRect(20, 110, 200, 40, 6, ILI9341_DARKGREY);
  tft.drawRoundRect(19, 109, 201, 41, 6, ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE);
  printTexto("MOTORES",ILI9341_WHITE,56,118,3);

  tft.fillRoundRect(20, 160, 200, 40, 6, ILI9341_DARKGREY);
  tft.drawRoundRect(19, 159, 201, 41, 6, ILI9341_RED);
  printTexto("SENSORES",ILI9341_WHITE,50,168,3);

  //printTexto("< Enter", ILI9341_WHITE,5,270,2); 
  printTexto("< Enter", ILI9341_DARKGREY,5,300,2); 

  printTexto("+ >", ILI9341_DARKGREY,195,270,2); 
  printTexto("- >", ILI9341_DARKGREY,195,300,2);
}

void SO_Anjos::telaMotores(){
  tft.setRotation(2);
  tft.drawLine(0, 30, 240, 30, ILI9341_WHITE);             // DESENHA UMA LINHA PARA O MENU
  tft.fillRect(3,6,105,21,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA
  printTexto("MOTORES", ILI9341_WHITE,8,9,2);

  tft.drawRect(196, 6, 31, 18, ILI9341_WHITE);             // DESENHA A BATERIA
  tft.fillRect(227,11,3,8,ILI9341_WHITE);
  printTexto("%", ILI9341_WHITE,219,11,1);

  tft.fillRect(0,37,240,281,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA 

              
  printTexto("PORTA", ILI9341_WHITE,12,50,1);
  //tft.drawString("PORTA", 12, 20,2);
  printTexto("VELOCIDADE", ILI9341_WHITE,65,50,1);
  printTexto("ENCODER", ILI9341_WHITE,152,50,1);

  tft.fillRoundRect(6, 60, 233, 39, 6, ILI9341_BLACK);
  printTexto("A",ILI9341_WHITE,12,69,3);
  printNumero(velocidadeM1,ILI9341_WHITE, 65,69,3);
  printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);

  tft.fillRoundRect(6, 110, 233, 39, 6, ILI9341_BLACK);
  printTexto("B",ILI9341_WHITE,12,119,3);
  printNumero(velocidadeM2,ILI9341_WHITE, 65,119,3);
  printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);

  tft.fillRoundRect(6, 160, 233, 39, 6, ILI9341_BLACK);
  printTexto("C",ILI9341_WHITE,12,169,3);
  printNumero(velocidadeM3,ILI9341_WHITE,65,169, 3);
  printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);

  tft.fillRoundRect(6, 210, 233, 39, 6, ILI9341_BLACK);
  printTexto("D",ILI9341_WHITE,12,219,3);
  printNumero(velocidadeM4,ILI9341_WHITE,65,219, 3);
  printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);

  printTexto("< Voltar", ILI9341_DARKGREY,5,270,2); 
  printTexto("< Enter", ILI9341_DARKGREY,5,300,2); 

  printTexto("+ >", ILI9341_DARKGREY,195,270,2); 
  printTexto("- >", ILI9341_DARKGREY,195,300,2);
  
  tft.drawRoundRect(5, 59, 235, 41, 6, ILI9341_GREEN);
  tft.drawRoundRect(5, 109, 235, 41, 6, ILI9341_RED);
  tft.drawRoundRect(5, 159, 235, 41, 6, ILI9341_RED);
  tft.drawRoundRect(5, 209, 235, 41, 6, ILI9341_RED);
}

void SO_Anjos::telaSensores(){
  tft.setRotation(2);
  tft.drawLine(0, 30, 240, 30, ILI9341_WHITE);             // DESENHA UMA LINHA PARA O MENU
  tft.fillRect(3,6,105,21,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA
  printTexto("SENSORES", ILI9341_WHITE,8,9,2);

  tft.drawRect(196, 6, 31, 18, ILI9341_WHITE);             // DESENHA A BATERIA
  tft.fillRect(227,11,3,8,ILI9341_WHITE);
  printTexto("%", ILI9341_WHITE,219,11,1);

  tft.fillRect(0,37,240,281,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA 

  tft.drawRoundRect(5, 35, 230, 86, 6, ILI9341_RED);
  printTexto("Sensor de Toque", ILI9341_WHITE,32,40,2);
  
  printNumero(2, ILI9341_WHITE,88,103,2);
  printNumero(3, ILI9341_WHITE,141,103,2);

  
  tft.drawRoundRect(5, 126, 230, 86, 6, ILI9341_YELLOW);
  printTexto("Sensor de Cor", ILI9341_WHITE,36,130,2); 
  tft.drawRoundRect(73, 149, 42, 42, 6, ILI9341_WHITE);
  tft.drawRoundRect(126, 149, 42, 42, 6, ILI9341_WHITE);
  printNumero(5, ILI9341_WHITE,88,193,2);
  printNumero(7, ILI9341_WHITE,141,193,2);

  tft.drawRoundRect(5, 217, 230, 86, 6, ILI9341_DARKGREEN);
  printTexto("Sensor de Distancia", ILI9341_WHITE,7,220,2);
}

void SO_Anjos::telaProgram(){
  tft.setRotation(2);
  tft.drawLine(0, 30, 240, 30, ILI9341_WHITE);             // DESENHA UMA LINHA PARA O MENU
  tft.fillRect(3,6,105,21,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA
  printTexto("PROGRAMA", ILI9341_WHITE,8,9,2);

  tft.drawRect(196, 6, 31, 18, ILI9341_WHITE);             // DESENHA A BATERIA
  tft.fillRect(227,11,3,8,ILI9341_WHITE);
  printTexto("%", ILI9341_WHITE,219,11,1);

  tft.fillRect(0,37,240,281,ILI9341_BLACK);                   // APAGA O SETOR DO DISPLAY PARA NOVA ESCRITA 
}

void SO_Anjos::updateSelection(){
  switch(tela){
    case 0:
      switch(option){
        case 0:
          tft.drawRoundRect(19, 59, 201, 41, 6, ILI9341_GREEN);
          tft.drawRoundRect(19, 109, 201, 41, 6, ILI9341_RED);
          break;
        case 1:
          tft.drawRoundRect(19, 59, 201, 41, 6, ILI9341_RED);
          tft.drawRoundRect(19, 109, 201, 41, 6, ILI9341_GREEN);
          tft.drawRoundRect(19, 159, 201, 41, 6, ILI9341_RED);
          break;
        case 2:
          tft.drawRoundRect(19, 109, 201, 41, 6, ILI9341_RED);
          tft.drawRoundRect(19, 159, 201, 41, 6, ILI9341_GREEN);
          break;
      }
      break;
    case 1:
      if(motorSelected < 0){
        switch(option){
          case 0:
            tft.fillRoundRect(5, 59, 235, 41, 6, ILI9341_BLACK);
            tft.drawRoundRect(5, 59, 235, 41, 6, ILI9341_GREEN);
            printTexto("A",ILI9341_WHITE,12,69,3);
            printNumero(velocidadeM1,ILI9341_WHITE, 65,69,3);
            printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
            tft.drawRoundRect(5, 109, 235, 41, 6, ILI9341_RED);
            break;
          case 1:
            tft.drawRoundRect(5, 59, 235, 41, 6, ILI9341_RED);
            tft.fillRoundRect(5, 109, 235, 41, 6, ILI9341_BLACK);
            tft.drawRoundRect(5, 109, 235, 41, 6, ILI9341_GREEN);
            printTexto("B",ILI9341_WHITE,12,119,3);
            printNumero(velocidadeM2,ILI9341_WHITE, 65,119,3);
            printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
            tft.drawRoundRect(5, 159, 235, 41, 6, ILI9341_RED);
            break;
          case 2:
            tft.drawRoundRect(5, 109, 235, 41, 6, ILI9341_RED);
            tft.fillRoundRect(5, 159, 235, 41, 6, ILI9341_BLACK);
            tft.drawRoundRect(5, 159, 235, 41, 6, ILI9341_GREEN);
            printTexto("C",ILI9341_WHITE,12,169,3);
            printNumero(velocidadeM3,ILI9341_WHITE,65,169, 3);
            printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
            tft.drawRoundRect(5, 209, 235, 41, 6, ILI9341_RED);
            break;
          case 3:
            tft.drawRoundRect(5, 159, 235, 41, 6, ILI9341_RED);
            tft.fillRoundRect(5, 209, 235, 41, 6, ILI9341_BLACK);
            tft.drawRoundRect(5, 209, 235, 41, 6, ILI9341_GREEN);
            printTexto("D",ILI9341_WHITE,12,219,3);
            printNumero(velocidadeM4,ILI9341_WHITE,65,219, 3);
            printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
            break;
        }
      }else{
        switch(option){
          case 0:
            tft.fillRoundRect(5, 59, 235, 41, 6, ILI9341_GREEN);
            printTexto("A",ILI9341_WHITE,12,69,3);
            printNumero(velocidadeM1,ILI9341_WHITE, 65,69,3);
            printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
            break;
          case 1:
            tft.fillRoundRect(5, 109, 235, 41, 6, ILI9341_GREEN);
            printTexto("B",ILI9341_WHITE,12,119,3);
            printNumero(velocidadeM2,ILI9341_WHITE, 65,119,3);
            printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
            break;
          case 2:
            tft.fillRoundRect(5, 159, 235, 41, 6, ILI9341_GREEN);
            printTexto("C",ILI9341_WHITE,12,169,3);
            printNumero(velocidadeM3,ILI9341_WHITE,65,169, 3);
            printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
            break;
          case 3:
            tft.fillRoundRect(5, 209, 235, 41, 6, ILI9341_GREEN);
            printTexto("D",ILI9341_WHITE,12,219,3);
            printNumero(velocidadeM4,ILI9341_WHITE,65,219, 3);
            printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
            break;
        }
      }
      break;
  }
}

void SO_Anjos::updateValues(){
  unsigned long currentMillis = millis();
  unsigned long currentMillisUltrasonic = millis();
  switch(tela){
    case 1:
      switch(motorSelected){
        case 0:  
          tft.fillRoundRect(120, 69, 80, 20, 6, ILI9341_GREEN);
          printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
          tft.fillRoundRect(120, 119, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
          tft.fillRoundRect(120, 169, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
          tft.fillRoundRect(120, 219, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
          break;
        case 1:
          tft.fillRoundRect(120, 69, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
          tft.fillRoundRect(120, 119, 80, 20, 6, ILI9341_GREEN);
          printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
          tft.fillRoundRect(120, 169, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
          tft.fillRoundRect(120, 219, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
          break;
        case 2:
          tft.fillRoundRect(120, 69, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
          tft.fillRoundRect(120, 119, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
          tft.fillRoundRect(120, 169, 80, 20, 6, ILI9341_GREEN);
          printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
          tft.fillRoundRect(120, 219, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
          break;
        case 3:
          tft.fillRoundRect(120, 69, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
          tft.fillRoundRect(120, 119, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
          tft.fillRoundRect(120, 169, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
          tft.fillRoundRect(120, 219, 80, 20, 6, ILI9341_GREEN);
          printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
          break;
        default:
          tft.fillRoundRect(120, 69, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m1),ILI9341_WHITE,130,69,3);
          tft.fillRoundRect(120, 119, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m2),ILI9341_WHITE,130,119,3);
          tft.fillRoundRect(120, 169, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m3),ILI9341_WHITE,130,169, 3);
          tft.fillRoundRect(120, 219, 80, 20, 6, ILI9341_BLACK);
          printNumero(getCodePin(m4),ILI9341_WHITE,130,219,3);
          break;
      }
      break;
    case 2:
       if (currentMillisUltrasonic - previousMillisUltrasonic >= intervalUltrasonic){
          tft.fillRoundRect(10, 239, 100, 40, 6, ILI9341_BLACK);
          tft.drawRoundRect(10, 239, 105, 42, 6, ILI9341_WHITE);
          printNumero(getUtrasonicSensorValue(6),ILI9341_WHITE,40,250,3);
          printNumero(6, ILI9341_WHITE,57,283,2);
          
          tft.fillRoundRect(126, 239, 100, 42, 6, ILI9341_BLACK);
          tft.drawRoundRect(126, 239, 105, 42, 6, ILI9341_WHITE);
          printNumero(getUtrasonicSensorValue(8),ILI9341_WHITE,155,250,3);
          printNumero(8, ILI9341_WHITE,173,283,2);

        previousMillisUltrasonic = currentMillisUltrasonic;
      }
      if (currentMillis - previousMillis >= interval) {
          if(getTouchSensorValue(2) == 1){
            tft.fillRoundRect(73, 59, 42, 42, 6, ILI9341_GREEN);
            printNumero(1, ILI9341_WHITE,86,70,3);
          }else{
            tft.fillRoundRect(73, 59, 42, 42, 6, ILI9341_RED);
            printNumero(0, ILI9341_WHITE,86,70,3);
          }

          if(getTouchSensorValue(3) == 1){
            tft.fillRoundRect(126, 59, 42, 42, 6, ILI9341_GREEN);
            printNumero(1, ILI9341_WHITE,139,70,3);
          }else{
            tft.fillRoundRect(126, 59, 42, 42, 6, ILI9341_RED);
            printNumero(0, ILI9341_WHITE,139,70,3);
          }
          
          switch (getColorSensorValue(5)){
            case 1:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_RED);
              printNumero(1, ILI9341_WHITE,86,160,3);
              break;
            case 2:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_GREEN);
              printNumero(2, ILI9341_WHITE,86,160,3);
              break;
            case 3:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_BLUE);
              printNumero(3, ILI9341_WHITE,86,160,3);
              break;
            case 4:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_YELLOW);
              printNumero(4, ILI9341_BLACK,86,160,3);
              break;
            case 5:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_ORANGE);
              printNumero(5, ILI9341_WHITE,86,160,3);
              break;
            case 6:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_WHITE);
              printNumero(6, ILI9341_BLACK,86,160,3);
              break;
            case 7:
              tft.fillRoundRect(73, 149, 42, 42, 6, ILI9341_BLACK);
              printNumero(7, ILI9341_WHITE,86,160,3);
              break;
          
            default:
              tft.drawRoundRect(73, 149, 42, 42, 6, ILI9341_WHITE);
              break;
          }
          
          switch (getColorSensorValue(1)){
            case 1:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_RED);
              printNumero(1, ILI9341_WHITE,139,160,3);
              break;
            case 2:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_GREEN);
              printNumero(2, ILI9341_WHITE,139,160,3);
              break;
            case 3:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_BLUE);
              printNumero(3, ILI9341_WHITE,139,160,3);
              break;
            case 4:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_YELLOW);
              printNumero(4, ILI9341_BLACK,139,160,3);
              break;
            case 5:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_ORANGE);
              printNumero(5, ILI9341_WHITE,139,160,3);
              break;
            case 6:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_WHITE);
              printNumero(6, ILI9341_BLACK,139,160,3);
              break;
            case 7:
              tft.fillRoundRect(126, 149, 42, 42, 6, ILI9341_BLACK);
              printNumero(7, ILI9341_WHITE,139,160,3);
              break;
          
            default:
              tft.drawRoundRect(126, 149, 42, 42, 6, ILI9341_WHITE);
              break;
          }
        previousMillis = currentMillis;
      }
      break;
  }
}



//////////////////////////////////////////////////////// BOTÕES ////////////////////////////////////////////////////////


typedef void (*buttonActionFunc)();
typedef void (*buttonActionLongPressFunc)(void *oneButton);

void SO_Anjos::defineButtonAction(int button, int state, buttonActionFunc func, buttonActionLongPressFunc longPressFunc){
  switch(button){
      case 2:
          switch(state){
              case 1:
                  buttonDownLeft.attachClick(func);
                  break;
              case 2:
                  buttonDownLeft.attachDoubleClick(func);
                  break;
              case 3:
                  buttonDownLeft.attachLongPressStart(longPressFunc, &buttonDownLeft);
                  break;
              case 4:
                  buttonDownLeft.attachDuringLongPress(longPressFunc, &buttonDownLeft);
                  break;
          }
          break;
      case 3:
          switch(state){
              case 1:
                  buttonUpRight.attachClick(func);
                  break;
              case 2:
                  buttonUpRight.attachDoubleClick(func);
                  break;
              case 3:
                  buttonUpRight.attachLongPressStart(longPressFunc, &buttonUpRight);
                  break;
              case 4:
                  buttonUpRight.attachDuringLongPress(longPressFunc, &buttonUpRight);
                  break;
          }
          break;
      case 4:
          switch(state){
              case 1:
                  buttonDownRight.attachClick(func);
                  break;
              case 2:
                  buttonDownRight.attachDoubleClick(func);
                  break;
              case 3:
                  buttonDownRight.attachLongPressStart(longPressFunc, &buttonDownRight);
                  break;
              case 4:
                  buttonDownRight.attachDuringLongPress(longPressFunc, &buttonDownRight);
                  break;
          }
          break;
  }
}

void SO_Anjos::clickButtonDownRight() { //Botão -
  Serial.print("tela :");
  Serial.println(tela);
  switch(tela){
    case 0:
      if(option < 2){
        NewTone(26,659,120);
        option++;
        updateSelection();
      }else{
        NewTone(26,250,120);
      }
      break;
    case 1:
      if(motorSelected < 0){
        if(option < 3){
          NewTone(26,659,120);
          option++;
          updateSelection();
        }else{
          NewTone(26,250,120);
        }
      }else{
        NewTone(26,659,120);
        switch(motorSelected){
          case 0:
            velocidadeM1 = velocidadeM1 - 10;
            setMotorPin(A, velocidadeM1);
            break;
          case 1:
            velocidadeM2 = velocidadeM2 - 10;
            setMotorPin(B, velocidadeM2);
            break;
          case 2:
            velocidadeM3 = velocidadeM3 - 10;
            setMotorPin(C, velocidadeM3);
            break;
          case 3:
            velocidadeM4 = velocidadeM4 - 10;
            setMotorPin(D, velocidadeM4);
            break;
        }
        updateSelection();
      }
      break;
    default:
      NewTone(26,250,120);
      break;
  }
}

void SO_Anjos::clickButtonUpRight() { // Botão +
  switch(tela){
    case 0:
      if(option > 0){
        NewTone(26,659,120);
        option--;
        updateSelection();
      }else{
        NewTone(26,250,120);
      }
      break;
    case 1: 
      if(motorSelected < 0){
        if(option > 0){
          NewTone(26,659,120);
          option--;
          updateSelection();
        }else{
          NewTone(26,250,120);
        }
      }else{
        NewTone(26,659,120);
        switch(motorSelected){
          case 0:
            velocidadeM1 = velocidadeM1 + 10;
            setMotorPin(A, velocidadeM1);
            break;
          case 1:
            velocidadeM2 = velocidadeM2 + 10;
            setMotorPin(B, velocidadeM2);
            break;
          case 2:
            velocidadeM3 = velocidadeM3 + 10;
            setMotorPin(C, velocidadeM3);
            break;
          case 3:
            velocidadeM4 = velocidadeM4 + 10;
            setMotorPin(D, velocidadeM4);
            break;
        }
        updateSelection();
      }
      break;
    default:
      NewTone(26,250,120);
      break;
  }
}

void SO_Anjos::clickButtonDownLeft(){  //Botão Enter
  switch(tela){
    case 0:
      NewTone(26,659,120);
      switch(option){
        case 0:
          option = 0;
          tela = 3;
          telaProgram();
          startProgram = 1;
          break;
        case 1:
          option = 0;
          tela = 1;
          telaMotores();
          break;
        case 2:
          option = 0;
          tela = 2;
          telaSensores();
          break;
      }
      break;
    case 1:
      NewTone(26,659,120);
      motorSelected = option;
      updateSelection();
      break;
    default:
      NewTone(26,250,1000);
      break;
  }
  
}
void SO_Anjos::clickButtonUpLeft(){  //Botão Voltar
  switch(tela){
    case 1:
      NewTone(26,659,120);
      if(motorSelected < 0){
        velocidadeM1 = 0;
        velocidadeM2 = 0;
        velocidadeM3 = 0;
        velocidadeM4 = 0;
        setMotorPin(A, 0);
        setMotorPin(B, 0);
        setMotorPin(C, 0);
        setMotorPin(D, 0);
        option = 0;
        tela = 0;
        TELA_MENU();
      }else{
        motorSelected = -1;
        updateSelection();
      }
      break;
    default:
      NewTone(26,659,120);
      option = 0;
      tela = 0;
      startProgram = 0;
      setupProgram = 0;
      TELA_MENU();
      break;
  }
}

void SO_Anjos::tickButtons(){
  buttonUpRight.tick();
  buttonDownRight.tick();
  buttonDownLeft.tick();
  buttonUpLeft.tick(); 
}

void SO_Anjos::startSO(){
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  TELA_MENU();
  byte C_BAT = map(Filtro_Analog(V_Bat,15,10),0,1023,0,100);
  BAT(C_BAT);
  buttonUpRight.attachClick(clickButtonUpRight);
  buttonDownRight.attachClick(clickButtonDownRight);
  buttonDownLeft.attachClick(clickButtonDownLeft);
  buttonUpLeft.attachClick(clickButtonUpLeft);
}