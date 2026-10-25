#include <Servo.h>

Servo servo1;
Servo servo2;

const int CLK = 2;
const int DT  = 3;
const int SW  = 4;

int lastCLK;

int angle1 = 90;
int angle2 = 90;

bool controllingServo1 = true;
bool lastButtonState = HIGH;

unsigned long lastClickTime = 0;
const unsigned long clickDelay = 400;
int clickCount = 0;

bool delivered = false;

bool waitingForReturnClick = false;


void setup() {

  Serial.begin(9600);

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  servo1.attach(9);
  servo2.attach(10);

  servo1.write(angle1);
  servo2.write(angle2);

  lastCLK = digitalRead(CLK);

  Serial.println("Servo 1 selected");
}


void delivery() {

  Serial.println("Delivery started");

  while (angle1 > 0) {

    angle1--;
    servo1.write(angle1);
    delay(15);
  }

  delay(500);

  servo2.write(90);

  delivered = true;

  Serial.println("Delivery complete");
}


void returnDelivery() {

  Serial.println("Return started");

  while (angle1 < 180) {

    angle1++;
    servo1.write(angle1);
    delay(15);
  }

  Serial.println("Press button to finish return");

  waitingForReturnClick = true;
}


void finishReturn() {

  Serial.println("Finishing return");

  servo2.write(180);

  delivered = false;

  waitingForReturnClick = false;

  Serial.println("Return complete");
}


void loop() {

  bool buttonState = digitalRead(SW);


  if (lastButtonState == HIGH && buttonState == LOW) {

    if (waitingForReturnClick) {

      finishReturn();

      delay(200);

      lastButtonState = buttonState;
      return;
    }


    unsigned long now = millis();


    if (now - lastClickTime < clickDelay) {

      clickCount++;

    } 
    else {

      clickCount = 1;
    }


    lastClickTime = now;

    delay(50);
  }


  lastButtonState = buttonState;


  if (clickCount > 0 && millis() - lastClickTime > clickDelay) {

    if (clickCount == 1) {

      controllingServo1 = !controllingServo1;

      if (controllingServo1)

        Serial.println("Manual control: Servo 1");

      else

        Serial.println("Manual control: Servo 2");

    }

    else if (clickCount >= 2) {

      if (delivered == false) {

        delivery();

      }

      else {

        returnDelivery();

      }

    }

    clickCount = 0;
  }


  int currentCLK = digitalRead(CLK);


  if (currentCLK != lastCLK && currentCLK == HIGH) {

    int change = (digitalRead(DT) != currentCLK) ? 5 : -5;


    if (controllingServo1) {

      angle1 += change;

      angle1 = constrain(angle1, 0, 180);

      servo1.write(angle1);

      Serial.print("Servo 1: ");

      Serial.println(angle1);

    }

    else {

      angle2 += change;

      angle2 = constrain(angle2, 0, 180);

      servo2.write(angle2);

      Serial.print("Servo 2: ");

      Serial.println(angle2);

    }

  }

  lastCLK = currentCLK;

}
