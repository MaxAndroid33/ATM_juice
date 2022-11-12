#include <Arduino.h>
#include <Wire.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);
#define flow_meter 21         // White Wire
#define ir1 22                // Yellow wire
#define ir2 23                // brown wire
#define pump_apple 24               // orange wire pump_apple for apple juice
#define pump_orange 25               // orange wire pump_orange for orange juice
#define change_V_Of_Bottle 10 // change_V_Of_ one Bottle ,from here chang the volume of battle with mL
#define en_stepper 25
#include <LiquidCrystal.h>
// Initialise the LCD with the arduino. LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define data_length 2
int type_juice;
char Data[data_length];
char Master[data_length] = "1";
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {31, 33, 35, 37}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {39, 41, 43, 45}; // connect to the column pinouts of the keypad
Keypad myKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//################################
const int stepPin = 7; // X.STEP
const int dirPin = 8;  // X.DIR
//########################################
int select_type();
void pump_start(int pump_type,bool condation);
void flowmeter();
void stepper(int x);
void irsensor1();
void irsensor2();
int counter = 0;

int xx = change_V_Of_Bottle; // the voulume of one battel
volatile int flow_frequency = 0;
volatile int flow_frequency2 = 0;
int volume = 0;
double flow_per_second = 0;
float flow_per_mint = 0;
float flow_per_hore = 0;
unsigned long current_millis = 0;
unsigned long previous_millis = 0;
/// 2565 pulse for 0.5 liter
void flow()
{
  flow_frequency++;
  flow_frequency2++;
}
void setup()
{

  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(flow_meter, INPUT);

  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  // Sets the two pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(pump_apple, OUTPUT);
  pinMode(pump_orange, OUTPUT);
  pinMode(en_stepper, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(flow_meter), flow, RISING);
  sei();
  current_millis = millis();
  previous_millis = current_millis;
  digitalWrite(pump_apple, HIGH);
  digitalWrite(pump_orange, HIGH);
}
void loop()
{
 type_juice=  select_type();
  flowmeter();
  stepper(1);
  irsensor1();
  irsensor2();
}
//#############################################
void irsensor1()
{
  int read_ir1 = digitalRead(ir1);
  // int y=0;
  // Serial.println(read_ir1);

  if (read_ir1 == 0)
  {
    stepper(0); // steper stop
    while (1)
    {
      read_ir1 = digitalRead(ir1);

      // digitalWrite(pump_apple, HIGH);
      digitalWrite(pump_apple, LOW);

      //#############################editing########

      Serial.println(volume);
      flowmeter();
      //
      lcd.setCursor(0, 0);
      lcd.print("V=");
      lcd.setCursor(3, 0);
      lcd.print(volume);
      lcd.setCursor(8, 0);
      lcd.print("L");
      if (volume == xx)
      {
        // digitalWrite(pump_apple, LOW);
        digitalWrite(pump_apple, HIGH);
        counter++;
        xx = volume + change_V_Of_Bottle; // change the volume
        flowmeter();
        break;
      }
    }
    //###########################

    while (1)
    {
      read_ir1 = digitalRead(ir1);
      int x = read_ir1;
      if (read_ir1 == 0 && x == 0)
      {
        stepper(1);
      }
      else
      {
        delay(500);
        break; // steper run
      }
    }
  }
}
//#############################################
void irsensor2()
{
  int read_ir2 = digitalRead(ir2);
  // Serial.println(read_ir2);
  if (read_ir2 == 0)
  {
    stepper(0); // steper stop
    while (1)
    {
      read_ir2 = digitalRead(ir2);
      if (read_ir2 == 1)
        break;
    }
  }
  stepper(1); // steper run
}
//#############################################
void flowmeter()
{
  // put your main code here, to run repeatedly:
  // 0.5 L = 1678 pulse
  // 1 L = (1678/.5)
  // volume (L) = 1/(5100)* N (No of pulses)
  // volume = Q(L/s) * t(s)
  // 1/(5100)* N = Q(L/s) * t(s)
  // N/t(s) = (5100)* Q(L/s)
  // F = (5100)* Q(L/s)
  // Q(L/s) = F / (5100)
  // Q(L/min) = F*60 / (5100) = F / 85
  // Q(L/h) = F*60*60 / (5100) = F *

  // Serial.println(flow_frequency);
  current_millis = millis();
  if (current_millis > (previous_millis + 1000))
  {
    flow_per_mint = (flow_frequency / (85.0));
    flow_per_second = flow_per_mint / 60;
    // volume = ((flow_frequency2/85.0)/60) ;
    volume = ((flow_frequency2 / 85.0));
    previous_millis = current_millis;
    // Serial.print(flow_frequency);Serial.print(" ");
    Serial.println(flow_frequency2);

    lcd.setCursor(0, 0);
    lcd.print("V=");
    lcd.setCursor(3, 0);
    lcd.print(volume * 10);
    lcd.setCursor(8, 0);
    lcd.print("mL");
    // lcd.setCursor(8,0);lcd.print("Q=");lcd.setCursor(10,0);lcd.print(flow_per_mint/60,4);lcd.setCursor(14,1);lcd.print("L/s");
    lcd.setCursor(0, 1);
    lcd.print("NO.bottle=");
    lcd.print(counter);
    flow_frequency = 0;
    // Serial.print("Volume = ");Serial.print(volume,4);Serial.print("L ");
    // Serial.print("Q(L/s) = ");Serial.print(flow_per_mint/60,4);Serial.print(" ");
    //  Serial.print("Q(L/min) = ");Serial.print(flow_per_mint);Serial.println(" ");
  }
}
void stepper(int x)
{
  if (x == 1)
  {
    digitalWrite(en_stepper, LOW);
    // digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
    // Makes 200 pulses for making one full cycle rotation
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  if (x == 0)
    digitalWrite(en_stepper, HIGH); // digitalWrite(dirPin,LOW);
}

// ########################## Select type of juice ###################################
int select_type()
{
  while (1)
  {

    lcd.setCursor(0, 0);
    lcd.print("1-APPLE");
    lcd.setCursor(0, 1);
    lcd.print("2-ORANGE");

    customKey = myKeypad.getKey();
    if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
    {
      Data[data_count] = customKey; // store char into data array
      lcd.setCursor(data_count, 1); // move cursor to show each new char
      lcd.print(Data[data_count]);  // print char at said cursor
      data_count++;                 // increment data array by 1 to store new char, also keep track of the number of chars entered
    }

    if (data_count == data_length - 1) // if the array index is equal to the number of expected chars, compare data to master
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("choise is ");

      if (!strcmp(Data, Master)) // equal to (strcmp(Data, Master) == 0)
      {
        lcd.print("Apple");
        delay(10000);
        return 0; // 0 means is Apple 
      }
      if (!strcmp(Data, "2")) // equal to (strcmp(Data, Master) == 0)
      {
        lcd.print("orange");
        delay(10000);
        return 1 ; // 1 means is orange
      }

      else
      {
        lcd.print("INCORRECT");

        delay(10000);
      }
      delay(1000); // added 1 second delay to make sure the password is completely shown on screen before it gets cleared.
      lcd.clear();
      clearData();
    }
  }
}







//######################### start pump #############################
// if condation is true the pump will be on
void pump_start(int pump_type,bool condation){

if (pump_type == 0 ){

  if(condation){
    digitalWrite(pump_apple, LOW);  // pump apple is on 
  }
  else{
     digitalWrite(pump_apple, HIGH);  // pump apple is off
  }

}
if (pump_type == 1 ){

  if(condation){
    digitalWrite(pump_orange, LOW);  // pump orange is on 
  }
  else{
     digitalWrite(pump_orange, HIGH);  // pump orange is off
  }

}
}
//########################### clear lcd ##########################
void clearData()
{
  while (data_count != 0)
  {                         // This can be used for any array size,
    Data[data_count--] = 0; // clear array for new data
  }
  int choies = int(Data);
}