#define PIN_ten 39
#define PIN_five 36
#define PIN_two 35
#define PIN_one 34

int amount;
int tenCoin;
int fiveCoin;
int twoCoin;
int oneCoin;

int deposit_state = 0;

// debug
int lastTime;
int prevTime = 2000;

int lastDepositTime;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ten, INPUT);
  pinMode(PIN_five, INPUT);
  pinMode(PIN_two, INPUT);
  pinMode(PIN_one, INPUT);
}

void loop() {
  if(millis() - lastDepositTime > 500)
  if(!deposit_state){
    if(digitalRead(PIN_ten)){
      tenCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
    }else if(digitalRead(PIN_five)){
      fiveCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
    }else if(digitalRead(PIN_two)){
      twoCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
    }else if(digitalRead(PIN_one)){
      oneCoin++;
      deposit_state = 1; 
      lastDepositTime = millis();
    }
  }
  if(!digitalRead(PIN_ten) && !digitalRead(PIN_five) && !digitalRead(PIN_two) && !digitalRead(PIN_one)){
    deposit_state = 0;
  }
  if(millis() - lastTime > prevTime || lastTime == 0){
    lastTime = millis();
    Serial.print("10 coin = ");
    Serial.println(tenCoin);
    Serial.print("5 coin = ");
    Serial.println(fiveCoin);
    Serial.print("2 coin = ");
    Serial.println(twoCoin);
    Serial.print("1 coin = ");
    Serial.println(oneCoin);
    Serial.println(digitalRead(PIN_ten));
    Serial.println(digitalRead(PIN_five));
    Serial.println(digitalRead(PIN_two));
    Serial.println(digitalRead(PIN_one));
    Serial.println("===================");
  }
}
