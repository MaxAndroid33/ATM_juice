#include <Arduino.h>

#define flow_meter 21 //White Wire
#define ir1 22 //Yellow wire
#define ir2 23 //brown wire
#define pump 24 // orange wire
#define change_V_Of_Bottle 10 //change_V_Of_ one Bottle ,from here chang the volume of battle with mL
#define en_stepper 25 
#include <LiquidCrystal.h>
//Initialise the LCD with the arduino. LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//################################
const int stepPin = 7; //X.STEP
const int dirPin = 8; // X.DIR
//########################################
void stepper(int x);
int irsensor1();
int irsensor2();
int counter=0;

int xx=change_V_Of_Bottle;//the voulume of one battel 
volatile int flow_frequency =0;
volatile int flow_frequency2 =0;
int volume =0;
double flow_per_second=0; 
float flow_per_mint=0;
float flow_per_hore=0;
unsigned long current_millis=0;
unsigned long previous_millis=0;
/// 2565 pulse for 0.5 liter
void flow (){
 flow_frequency++;
 flow_frequency2++;
 }
void setup() {
 
 // put your setup code here, to run once:
 Serial.begin(9600);
 lcd.begin(16, 2);
 pinMode(flow_meter,INPUT);

 pinMode(ir1,INPUT);
 pinMode(ir2,INPUT);
// Sets the two pins as Outputs
pinMode(stepPin,OUTPUT); 
pinMode(dirPin,OUTPUT);
pinMode(pump,OUTPUT);
pinMode(en_stepper,OUTPUT);
attachInterrupt(digitalPinToInterrupt(flow_meter),flow,RISING);
sei();
current_millis = millis();
previous_millis = current_millis;
digitalWrite(pump, HIGH);
}
void loop() {
 flowmeter();
 stepper(1);
 irsensor1();
 irsensor2(); 
 }
//#############################################
int irsensor1(){
 int read_ir1=digitalRead(ir1);
 //int y=0;
// Serial.println(read_ir1);
 
 if(read_ir1==0){
 stepper(0); //steper stop
 while(1){
 read_ir1=digitalRead(ir1);
 
 //digitalWrite(pump, HIGH);
 digitalWrite(pump, LOW);
 
//#############################editing########
 
 Serial.println(volume);
 flowmeter();
 // 
lcd.setCursor(0,0);lcd.print("V=");lcd.setCursor(3,0);lcd.print(volume);lcd.setCursor(8,0);lcd.print("L");
 if(volume==xx){
 //digitalWrite(pump, LOW);
 digitalWrite(pump, HIGH);
 counter++;
 xx=volume+change_V_Of_Bottle; //change the volume
 flowmeter();
 break; 
 } 
 
 }
 //###########################
 
 while(1){
 read_ir1=digitalRead(ir1);
 int x=read_ir1;
 if(read_ir1==0&&x==0){
 stepper(1);}
 else {
    delay(500);
break; //steper run
 }
 } 
 
 }

}
//############################################# 
int irsensor2(){
int read_ir2=digitalRead(ir2);
 //Serial.println(read_ir2);
 if(read_ir2==0){
 stepper(0); //steper stop 
 while(1){
  read_ir2=digitalRead(ir2);
 if(read_ir2==1)break;
 }
 
 }
 stepper(1); //steper run
 }
//#############################################
void flowmeter(){
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
 
 //Serial.println(flow_frequency);
 current_millis = millis();
 if (current_millis > (previous_millis+1000)){
 flow_per_mint = (flow_frequency / (85.0));
 flow_per_second = flow_per_mint/60;
 // volume = ((flow_frequency2/85.0)/60) ;
 volume = ((flow_frequency2/85.0)) ;
 previous_millis = current_millis;
 // Serial.print(flow_frequency);Serial.print(" "); 
Serial.println(flow_frequency2);

 
lcd.setCursor(0,0);lcd.print("V=");lcd.setCursor(3,0);lcd.print(volume*10);lcd.setCursor(8,0);lcd.print("mL");
 // lcd.setCursor(8,0);lcd.print("Q=");lcd.setCursor(10,0);lcd.print(flow_per_mint/60,4);lcd.setCursor(14,1);lcd.print("L/s");
 lcd.setCursor(0,1);lcd.print("NO.bottle=");lcd.print(counter);
 flow_frequency =0;
 //Serial.print("Volume = ");Serial.print(volume,4);Serial.print("L ");
 //Serial.print("Q(L/s) = ");Serial.print(flow_per_mint/60,4);Serial.print(" ");
 // Serial.print("Q(L/min) = ");Serial.print(flow_per_mint);Serial.println(" "); 
 }
 
 }
 void stepper(int x){
 if(x==1){
 digitalWrite(en_stepper,LOW); 
 //digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
// Makes 200 pulses for making one full cycle rotation
digitalWrite(stepPin,HIGH); 
delayMicroseconds(1000); 
digitalWrite(stepPin,LOW); 
delayMicroseconds(1000); 
 }
 if(x==0)digitalWrite(en_stepper,HIGH);//digitalWrite(dirPin,LOW); 
 }
 