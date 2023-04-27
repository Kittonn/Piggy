#define PIN_ten 5
#define PIN_five 17
#define PIN_two 16
#define PIN_one 4

int amount;
int tenCoin;
int fiveCoin;
int twoCoin;
int oneCoin;

int deposit_state;

// debug
int lastTime;
int prevTime = 2000;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_ten, INPUT);
  pinMode(PIN_five, INPUT);
  pinMode(PIN_two, INPUT);
  pinMode(PIN_one, INPUT);
}

void loop() {
  if(!deposit_state){
    if(digitalRead(PIN_ten)){
      tenCoin++;
      deposit_state = 1;
    }else if(digitalRead(PIN_five)){
      fiveCoin++;
      deposit_state = 1;
    }else if(digitalRead(PIN_two)){
      twoCoin++;
      deposit_state = 1;
    }else if(digitalRead(PIN_one)){
      oneCoin++;
      deposit_state = 1; 
    }
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
  }
}
