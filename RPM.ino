int sensorPin = 0;
int currentValue;
int pulse;
int lastValue = 60;
int dmax = 0;
unsigned long currentTime;
unsigned long start;
float RPM;
int dt;
int dv;

void setup(){
  Serial.begin(9600);
  start = millis();
}
//void loop(){
//currentValue = analogRead(A0);
//if (currentValue > max){
  //  max = currentValue;
  //  Serial.println(max);
  //  }
//}

void loop() {
  currentValue = analogRead(A0);
  //Serial.println(currentValue);
  currentTime = millis();
  dv = abs(currentValue - lastValue);
  dt = currentTime - start;
  //if (dv > dmax){
  //dmax = dv;
  //Serial.println(dmax);
  //}
  //Serial.println(dt);
  if(dv > 30) {
    pulse++;
    //Serial.println("pulse");
    //Serial.println(pulse);
  }
  lastValue = currentValue;
  if((dt) >= 500.0) {
    //Serial.println(pulse);
    RPM = (pulse*120)/15;
    Serial.println(RPM);
    pulse = 0;
    start = currentTime;
  }
  //Serial.println(pulse);
}