#define MOTOR1_A 4
#define MOTOR1_B 3
#define MOTOR2_A 6
#define MOTOR2_B 5

#define MOTOR3_A 8
#define MOTOR3_B 7
#define MOTOR4_A 10
#define MOTOR4_B 9

#define ENABLE 11
#define ENABLE_START 150  //мощность на подъем
#define ENABLE_LOW 20     //слабая мощность для плавного отпускания до середины
#define ENABLE_STOP 100   //полное усиленное отпускание

#define BEEP_12 13 //
#define BEEP_34 12

#define LED_VAL 14
#define VAL_BUTTON 15

#define LED_START 16
#define START_BUTTON 17

#define SPEED_DELAY A6
#define BEEP 2

#define DELAY_M1 300
#define DELAY_M2 300
#define DELAY_M3 300
#define DELAY_M4 300

#define DELAY_M1_OTPUSKANIE 400
#define DELAY_M2_OTPUSKANIE 400
#define DELAY_M3_OTPUSKANIE 400
#define DELAY_M4_OTPUSKANIE 400

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 (0x3F) for a 16 chars and 2 line display

/*
I2C FastLED
*/






/*
// I2C FastLED
*/


volatile unsigned int counterPlay = 0;  //переменная-счетчик игр
volatile byte counterRegimLoop = 0;
volatile bool startButtonPress = 0;
volatile bool startButtonPressLoop = 0;
volatile bool valButtonPress = 0;
volatile bool valButtonPressLoop = 0;
volatile bool printLCD = 0;
volatile bool printLCDLoop = 0;
int prevSpeed = 0;


byte timer1Step = 0;
bool timer1Start = 0;
int timer1  = 0;
unsigned long prevMillisTimer1  = 0;

byte timer2Step = 0;
bool timer2Start = 0;
int timer2  = 0;
unsigned long prevMillisTimer2  = 0;

byte timer3Step = 0;
bool timer3Start = 0;
int timer3  = 0;
unsigned long prevMillisTimer3  = 0;

byte timer4Step = 0;
bool timer4Start = 0;
int timer4  = 0;
unsigned long prevMillisTimer4  = 0;

bool motor1Start = 0;
bool motor2Start = 0;
bool motor3Start = 0;
bool motor4Start = 0;

byte Y[8] = 
{
  B10001,
  B10001,
  B10001,
  B01111,
  B00001,
  B00001,
  B11110,
  B00000,
};



ISR (TIMER0_COMPA_vect) //функция, вызываемая таймером-счетчиком каждые 0,001 сек.
{ 
  if(digitalRead(START_BUTTON) == 0)
  {
    if(startButtonPress == 0)
    {
      startButtonPress = 1;
    }
  }

  if(digitalRead(VAL_BUTTON) == 0)
  {
    if(valButtonPress == 0)
    {
      valButtonPress = 1;    
    }
  }
}



void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(LED_START, OUTPUT);
  digitalWrite(LED_START, HIGH);
  pinMode(LED_VAL, OUTPUT);
  digitalWrite(LED_VAL, HIGH);
  pinMode(BEEP, OUTPUT);
  digitalWrite(BEEP, HIGH);
  delay(200);
  digitalWrite(BEEP, LOW);

  lcd.init();           
  lcd.backlight();
  lcd.createChar(1, Y);
  lcd.setCursor(0,0);
  lcd.print(char(1));
  lcd.print("POB:");
  lcd.setCursor(0,1);
  lcd.print("CKOP:");

  readEEPROM();
  pinMode(BEEP_34, OUTPUT);
  pinMode(BEEP_12, OUTPUT);


  pinMode(ENABLE, OUTPUT);

  pinMode(SPEED_DELAY, INPUT);
  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(VAL_BUTTON, INPUT_PULLUP);


  pinMode(MOTOR3_A, OUTPUT);
  pinMode(MOTOR3_B, OUTPUT);
  pinMode(MOTOR4_A, OUTPUT);
  pinMode(MOTOR4_B, OUTPUT);
  
  pinMode(MOTOR1_A, OUTPUT);
  pinMode(MOTOR1_B, OUTPUT);
  pinMode(MOTOR2_A, OUTPUT);
  pinMode(MOTOR2_B, OUTPUT);

  /**** настройка прерывания по таймеру каждую 0,001 сек (вызов функции ISR (TIMER0_COMPA_vect)) ****/
  TCCR0A |= (1 << WGM01);              //сброс при совпадении
  OCR0A = 0xF9;                        //начало отсчета до переполнения (249)
  TIMSK0 |= (1 << OCIE0A);             //разрешить прерывание при совпадении с регистром А
  TCCR0B |= (1 << CS01) | (1 << CS00); //установить делитель частоты на 64 
  sei();                               //разрешить прерывания
  digitalWrite(LED_VAL, LOW);
  digitalWrite(LED_START, LOW);


  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(111);              // sends one byte
  Wire.endTransmission();    // stop transmitting

  motor1Start = 1;
  motor2Start = 1;
  motor3Start = 1;
  motor4Start = 1;
}

void loop() 
{
  
  motor1();
  motor2();
  motor3();
  motor4();
  /*clearEEPROM();
  speedDelay();
  cli();
  valButtonPressLoop = valButtonPress;
  startButtonPressLoop = startButtonPress;
  sei();
  if(valButtonPressLoop == 1)
  {
      digitalWrite(LED_VAL, HIGH); 
      cli();
      valButtonPress = 1;
      sei();
      printLCD = 0;
      beep(); 
      counterRegimLoop++;   
      if(counterRegimLoop > 3)
      {
        counterRegimLoop = 0;
      }
      digitalWrite(LED_VAL, LOW);
  }

  if(startButtonPressLoop == 1)
  {
    digitalWrite(LED_START, HIGH);
    beep();
  }

  switch(counterRegimLoop)
  {
    case 0:
        if(printLCD == 0) //если еще не выводили инфу на экран
        {
            //вывести пробный режим
            lcd.setCursor(5,0);
            lcd.print("   ");
            lcd.setCursor(5,0);
            lcd.print("___");
            delay(500);
            cli();  
            valButtonPress = 0;
            sei();
            printLCD = 1; //запретить повторный вывод
            valButtonPressLoop = 0; //разрешить повторное нажатие кнопки         
        }
        if(startButtonPressLoop == 1)
        {
          probMode();
          cli();
          startButtonPress = 0;
          sei();
          startButtonPressLoop = 0;
          //прописать режим пробный  
        }
        
    break;

    case 1:
        if(printLCD == 0)
        {
            //вывести пробный режим
            lcd.setCursor(5,0);
            lcd.print("   ");
            lcd.setCursor(5,0);
            lcd.print("0__");
            delay(500);
            cli();  
            valButtonPress = 0;
            sei();
            printLCD = 1;
            valButtonPressLoop = 0;
        }
        if(startButtonPressLoop == 1)
        {
          easyMode();
          cli();
          startButtonPress = 0;
          sei();
          startButtonPressLoop = 0;
          //прописать режим пробный  
        }
    break;

    case 2:
        if(printLCD == 0)
        {
            //вывести пробный режим
            lcd.setCursor(5,0);
            lcd.print("   ");
            lcd.setCursor(5,0);
            lcd.print("00_");
            delay(500);
            cli();  
            valButtonPress = 0;
            sei();
            printLCD = 1;
            valButtonPressLoop = 0;
        }
        if(startButtonPressLoop == 1)
        {
          sredMode();
          cli();
          startButtonPress = 0;
          sei();
          startButtonPressLoop = 0;
          //прописать режим пробный  
        }
    break;

    case 3:
        if(printLCD == 0)
        {
            //вывести пробный режим
            lcd.setCursor(5,0);
            lcd.print("   ");
            lcd.setCursor(5,0);
            lcd.print("000");
            delay(500);
            cli();  
            valButtonPress = 0;
            sei();
            printLCD = 1;
            valButtonPressLoop = 0;
        }
        if(startButtonPressLoop == 1)
        {
          hardMode();
          cli();
          startButtonPress = 0;
          sei();
          startButtonPressLoop = 0;
          //прописать режим пробный  
        }
    break;
  }
  */
}

void writeEEPROM(int address, int value1)
{
   EEPROM.put(address, value1); // Записать значение value1 в ячейку с адресом 0
   address += sizeof(int); // Корректируем адрес на уже записанное значение - 2 байта занимает тип int
   EEPROM.put(address, value1); // Записать значение value1 в ячейку с адресом +2 (для проверки целостности данных)
   readEEPROM();
}

void clearEEPROM()
{
  if((digitalRead(START_BUTTON) == 0) && (digitalRead(VAL_BUTTON) == 0) && (speedDelay() >= 15) && (speedDelay() <= 20) )
  {
     beep();
     beep();
     beep();
     writeEEPROM(0, 0);  
     while(digitalRead(START_BUTTON) == 0)
     {
        digitalWrite(LED_START, HIGH);
        digitalWrite(LED_VAL, LOW);
        delay(200);
        digitalWrite(LED_START, LOW);
        digitalWrite(LED_VAL, HIGH);
        delay(200);     
     }
     digitalWrite(LED_VAL, LOW);
     delay(3000);
  }
}

bool readEEPROM()
{
  int byteRead1;
  int byteRead2;
  EEPROM.get(0, byteRead1);
  EEPROM.get(2, byteRead2);
  if(byteRead1 == byteRead2)
  {
    if(byteRead1 == 255)
    {
      byteRead1 = 0;
      writeEEPROM(0, 0);  
    }
     lcd.setCursor(11,1);
     lcd.print("     "); 
     lcd.setCursor(11,1);
      if(byteRead1 > 999)
      {
        lcd.print("#" + String(byteRead1));
      }
      else if(byteRead1 > 99)
        {
          lcd.print("#0" + String(byteRead1));
        }
        else if (byteRead1 > 9)
        {
          lcd.print("#00" + String(byteRead1));
        }
        else
        {
          lcd.print("#000" + String(byteRead1));
          
        }  
     counterPlay = byteRead1; 
  }
  else
  {
     lcd.setCursor(11,0);
     lcd.print("     "); 
     lcd.setCursor(11,0);
     lcd.print("ERROR");
  }
}

int speedDelay()
{
  int speedPlay = map(readMedian(SPEED_DELAY, 15), 0, 1023, 200, 1);
  if(speedPlay != prevSpeed)
  {
    prevSpeed = speedPlay;
    lcd.setCursor(5,1);
    lcd.print("    ");
    lcd.setCursor(5,1);
    lcd.print(String(speedPlay) + "%");
  }
  return speedPlay;
}

int speedDelayKoef()
{
  int speedVal = speedDelay();
  speedVal = map(speedVal, 1, 200, 200, 1) / 100;
  return speedVal;
}

// функция считывает аналоговый вход заданное количество раз
// и возвращает медианное отфильтрованное значение
int readMedian (int pin, int samples){
  // массив для хранения данных
  int raw[samples];
  // считываем вход и помещаем величину в ячейки массива
  for (int i = 0; i < samples; i++){
    raw[i] = analogRead(pin);
  }
  // сортируем массив по возрастанию значений в ячейках
  int temp = 0; // временная переменная

  for (int i = 0; i < samples; i++){
    for (int j = 0; j < samples - 1; j++){
      if (raw[j] > raw[j + 1]){
        temp = raw[j];
        raw[j] = raw[j + 1];
        raw[j + 1] = temp;
      }
    }
  }
  // возвращаем значение средней ячейки массива
  return raw[samples/2];
}

void beep()
{
  digitalWrite(BEEP, HIGH);
  delay(200);
  digitalWrite(BEEP, LOW); 
}

bool motor1()
{
    if (motor2Start == 1){
        timer2Step = 0;
        motor2Start = 0; 
    }
    if (timer1Start == 0){
        prevMillisTimer1 = millis();
        timer1Start = 1;
    }
    timer1 = millis() - prevMillisTimer1;

   if (timer1Step == 0) {
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(11);              // sends one byte
    Wire.endTransmission();    // stop transmitting
    analogWrite(ENABLE, ENABLE_START);
    digitalWrite(MOTOR1_A, HIGH); 
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(BEEP_12, HIGH);
    timer1Step = 1;
    timer1Start = 0;
    Serial.println("HL ENABLE_START Sound_ON 1");
    return 0;
  }
  
  
  if ((timer1Step == 1) && (timer1 >= DELAY_M1)) {
    digitalWrite(BEEP_12, LOW);
    timer1Step = 2;
    timer1Start = 0;
    Serial.println("Sound_OFF 2");
    return 0;
  }

  if ((timer1Step == 2) && (timer1 >= DELAY_M1)) {
    analogWrite(ENABLE, ENABLE_LOW);
    timer1Step = 3;
    timer1Start = 0;
    Serial.println("ENABLE_LOW 3");
    return 0;
  }

  if ((timer1Step == 3) && (timer1 >= 200)) {
    analogWrite(ENABLE, ENABLE_STOP);
    digitalWrite(MOTOR1_A, LOW); 
    digitalWrite(MOTOR1_B, HIGH);
    timer1Step = 4;
    timer1Start = 0;
    Serial.println("LH ENABLE_STOP 4");
    return 0;
  }
  
  if ((timer1Step == 4) && (timer1 >= DELAY_M1_OTPUSKANIE)) {
    analogWrite(ENABLE, 0);
    digitalWrite(MOTOR1_A, LOW); 
    digitalWrite(MOTOR1_B, LOW);
    timer1Step = 5;
    timer1Start = 0;
    Serial.println("LL 0 5");
    return 0;
  }
}

bool motor2()
{
    if (motor2Start == 1){
        timer2Step = 0;
        motor2Start = 0; 
    }
    if (timer2Start == 0){
        prevMillisTimer2 = millis();
        timer2Start = 1;
    }
    timer2 = millis() - prevMillisTimer2;

   if (timer2Step == 0) {
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(22);              // sends one byte
    Wire.endTransmission();    // stop transmitting
    analogWrite(ENABLE, ENABLE_START);
    digitalWrite(MOTOR2_A, HIGH); 
    digitalWrite(MOTOR2_B, LOW);
    digitalWrite(BEEP_12, HIGH);
    timer2Step = 1;
    timer2Start = 0;
    Serial.println("2HL ENABLE_START Sound_ON 1");
    return 0;
  }
  
  
  if ((timer2Step == 1) && (timer2 >= DELAY_M2)) {
    digitalWrite(BEEP_12, LOW);
    timer2Step = 2;
    timer2Start = 0;
    Serial.println("2Sound_OFF 2");
    return 0;
  }

  if ((timer2Step == 2) && (timer2 >= DELAY_M2)) {
    analogWrite(ENABLE, ENABLE_LOW);
    timer2Step = 3;
    timer2Start = 0;
    Serial.println("2ENABLE_LOW 3");
    return 0;
  }

  if ((timer2Step == 3) && (timer2 >= 200)) {
    analogWrite(ENABLE, ENABLE_STOP);
    digitalWrite(MOTOR2_A, LOW); 
    digitalWrite(MOTOR2_B, HIGH);
    timer2Step = 4;
    timer2Start = 0;
    Serial.println("2LH ENABLE_STOP 4");
    return 0;
  }
  
  if ((timer2Step == 4) && (timer2 >= DELAY_M2_OTPUSKANIE)) {
    analogWrite(ENABLE, 0);
    digitalWrite(MOTOR2_A, LOW); 
    digitalWrite(MOTOR2_B, LOW);
    timer2Step = 5;
    timer2Start = 0;
    Serial.println("2LL 0 5");
    return 0;
  }
}

bool motor3()
{
    if (motor3Start == 1){
        timer3Step = 0;
        motor3Start = 0; 
    }
    if (timer3Start == 0){
        prevMillisTimer3 = millis();
        timer3Start = 1;
    }
    timer3 = millis() - prevMillisTimer3;

   if (timer3Step == 0) {
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(33);              // sends one byte
    Wire.endTransmission();    // stop transmitting
    analogWrite(ENABLE, ENABLE_START);
    digitalWrite(MOTOR3_A, HIGH); 
    digitalWrite(MOTOR3_B, LOW);
    digitalWrite(BEEP_34, HIGH);
    timer3Step = 1;
    timer3Start = 0;
    Serial.println("3HL ENABLE_START Sound_ON 1");
    return 0;
  }
  
  
  if ((timer3Step == 1) && (timer3 >= DELAY_M3)) {
    digitalWrite(BEEP_34, LOW);
    timer3Step = 2;
    timer3Start = 0;
    Serial.println("3Sound_OFF 2");
    return 0;
  }

  if ((timer3Step == 2) && (timer3 >= DELAY_M3)) {
    analogWrite(ENABLE, ENABLE_LOW);
    timer3Step = 3;
    timer3Start = 0;
    Serial.println("3ENABLE_LOW 3");
    return 0;
  }

  if ((timer3Step == 3) && (timer3 >= 200)) {
    analogWrite(ENABLE, ENABLE_STOP);
    digitalWrite(MOTOR3_A, LOW); 
    digitalWrite(MOTOR3_B, HIGH);
    timer3Step = 4;
    timer3Start = 0;
    Serial.println("3LH ENABLE_STOP 4");
    return 0;
  }
  
  if ((timer3Step == 4) && (timer3 >= DELAY_M3_OTPUSKANIE)) {
    analogWrite(ENABLE, 0);
    digitalWrite(MOTOR3_A, LOW); 
    digitalWrite(MOTOR3_B, LOW);
    timer3Step = 5;
    timer3Start = 0;
    Serial.println("3LL 0 5");
    return 0;
  }
}

bool motor4()
{
    if (motor4Start == 1){
      timer4Step = 0;
      motor4Start = 0; 
    }
    if (timer4Start == 0){
        prevMillisTimer4 = millis();
        timer4Start = 1;
    }
    timer4 = millis() - prevMillisTimer4;

   if (timer4Step == 0) {
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(44);              // sends one byte
    Wire.endTransmission();    // stop transmitting
    analogWrite(ENABLE, ENABLE_START);
    digitalWrite(MOTOR4_A, HIGH); 
    digitalWrite(MOTOR4_B, LOW);
    digitalWrite(BEEP_34, HIGH);
    timer4Step = 1;
    timer4Start = 0;
    Serial.println("4HL ENABLE_START Sound_ON 1");
    return 0;
  }
  
  
  if ((timer4Step == 1) && (timer4 >= DELAY_M4)) {
    digitalWrite(BEEP_34, LOW);
    timer4Step = 2;
    timer4Start = 0;
    Serial.println("4Sound_OFF 2");
    return 0;
  }

  if ((timer4Step == 2) && (timer4 >= DELAY_M4)) {
    analogWrite(ENABLE, ENABLE_LOW);
    timer4Step = 3;
    timer4Start = 0;
    Serial.println("4ENABLE_LOW 3");
    return 0;
  }

  if ((timer4Step == 3) && (timer4 >= 200)) {
    analogWrite(ENABLE, ENABLE_STOP);
    digitalWrite(MOTOR4_A, LOW); 
    digitalWrite(MOTOR4_B, HIGH);
    timer4Step = 4;
    timer4Start = 0;
    Serial.println("4LH ENABLE_STOP 4");
    return 0;
  }
  
  if ((timer4Step == 4) && (timer4 >= DELAY_M4_OTPUSKANIE)) {
    analogWrite(ENABLE, 0);
    digitalWrite(MOTOR4_A, LOW); 
    digitalWrite(MOTOR4_B, LOW);
    timer4Step = 5;
    timer4Start = 0;
    Serial.println("4LL 0 5");
    return 0;
  }
}

void easyMode()
{
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(66);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(3000);
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay((1000+200)*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay((1000+200)*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay((500+100)*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay((1000+200)*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(500*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(900*speedDelayKoef());
    counterPlay++;
    writeEEPROM(0, counterPlay); // Записать количество игр в ячейки с адресом 0 и 2
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(900*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(1200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(900*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1300*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(900*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(500*speedDelayKoef());
  }
  digitalWrite(LED_START, LOW);
  digitalWrite(BEEP, HIGH);
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(55);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(1000);
  digitalWrite(BEEP, LOW);
}

void sredMode()
{
    Wire.beginTransmission(8); // transmit to device #8
  Wire.write(66);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(3000);
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1000*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(1000*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(500*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(1000*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(500*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(700*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(300*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(600*speedDelayKoef());
    counterPlay++;
    writeEEPROM(0, counterPlay); // Записать количество игр в ячейки с адресом 0 и 2
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(200*speedDelayKoef());
  }
  digitalWrite(LED_START, LOW);
  digitalWrite(BEEP, HIGH);
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(55);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(1000);
  digitalWrite(BEEP, LOW);
}

void hardMode()
{

  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(66);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(3000);
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(800*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(800*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(300*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(800*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(300*speedDelayKoef());
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(300*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(1000*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(800*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(600*speedDelayKoef());
    counterPlay++;
    writeEEPROM(0, counterPlay); // Записать количество игр в ячейки с адресом 0 и 2
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(300*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(200*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(700*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(300*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(500*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(600*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(300*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(400*speedDelayKoef());
  }
  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(200*speedDelayKoef());
  }
  digitalWrite(LED_START, LOW);
  digitalWrite(BEEP, HIGH);
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(55);              // sends one byte
  Wire.endTransmission();    // stop transmitting
  delay(1000);
  digitalWrite(BEEP, LOW);
}

void probMode()
{
  delay(3000);
  if(digitalRead(START_BUTTON) == 1)
  {
    motor1();
    delay(1000);
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor2();
    delay(1000);
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor3();
    delay(1000);
  }

  if(digitalRead(START_BUTTON) == 1)
  {
    motor4();
    delay(1000);
  }
  digitalWrite(LED_START, LOW);
  digitalWrite(BEEP, HIGH);
  delay(1000);
  digitalWrite(BEEP, LOW);
}
