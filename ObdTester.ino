


struct Pin {
  int num;
  int out;
  int in;

  Pin() {

  }

  Pin(int num_, int out_, int in_) {
    num = num_;
    out = out_;
    in = in_;
  }

  void on() {
    pinMode(in, OUTPUT);
    pinMode(out, OUTPUT);
    digitalWrite(in, LOW);
    digitalWrite(out, HIGH);
  }

  void off() {
    pinMode(in, OUTPUT);
    pinMode(out, OUTPUT);
    digitalWrite(in, LOW);
    digitalWrite(out, LOW);
  }

};

Pin pins[16];

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(100);
  log("Setup (v1)...");

  pinMode(18, OUTPUT); //Blue
  pinMode(19, OUTPUT); //Green

  pins[0] = Pin(1, 2, 3);
  pins[1] = Pin(2, 3, 2);
  pins[2] = Pin(3, 4, 5);
  pins[3] = Pin(4, 5, 4);

  pins[4] = Pin(5, 6, 7);
  pins[5] = Pin(6, 7, 6);
  pins[6] = Pin(7, 8, 9);
  pins[7] = Pin(8, 9, 8);

  pins[8] = Pin(9, 10, 11);
  pins[9] = Pin(10, 11, 10);
  pins[10] = Pin(11, 12, 13);
  pins[11] = Pin(12, 13, 12);

  pins[12] = Pin(13, 14, 15);
  pins[13] = Pin(14, 15, 14);
  pins[14] = Pin(15, 16, 17);
  pins[15] = Pin(16, 17, 16);
  log("Ready!");
}

void log(String msg) {
  Serial.println(msg);
  Serial.flush();
}

void setInputs() {
  for (int i = 2; i < 18; ++i) {
    pinMode(i, INPUT);
  }
}

void setOutputs() {
  for (int i = 2; i < 18; ++i) {
    pinMode(i, OUTPUT);
  }
}

void allOff() {
  for (int i = 2; i < 18; ++i) {
    digitalWrite(i, LOW);
  }
}

void flip() {
  for (int i = 2; i < 18; i += 2) {
    digitalWrite(i, HIGH);
    digitalWrite(i + 1, LOW);
  }
}

void flop() {
  for (int i = 2; i < 18; i += 2) {
    digitalWrite(i, LOW);
    digitalWrite(i + 1, HIGH);
  }
}


boolean checkPin(Pin pin) {
  log("Check Pin: " + String(pin.num) + ", Output: " + String(pin.out) + ", Input: " + String(pin.in));
  delay(10);

  setOutputs();
  allOff();
  log("  Pin Setup");
  pinMode(pin.in, INPUT);
  pinMode(pin.out, OUTPUT);
  digitalWrite(pin.out, LOW);
  delay(10);

  int initRead = digitalRead(pin.in);
  log("  InitRead: " + String(initRead));

  log("  Test High");
  digitalWrite(pin.out, HIGH);
  delay(10);
  int highRead = digitalRead(pin.in);
  log("  HighRead: " + String(highRead));
  delay(10);

  log("  Test Low");
  digitalWrite(pin.out, LOW);
  delay(10);
  int lowRead = digitalRead(pin.in);
  log("  LowRead: " + String(lowRead));
  delay(10);

  int crosstalk = 0;
  for (int i = 2; i < 18; ++i) {
    if (i != pin.in) {
      pinMode(i, INPUT);
      delay(10);
      int ct = digitalRead(i);
      if (ct) {
        log("  !! Crosstalk on pin: " + String(pins[i].num));
      }
      crosstalk += ct;
      pinMode(i, OUTPUT);
      digitalWrite(i, LOW);
    }
  }
  log("  Crosstalk: " + String(crosstalk));
  delay(10);

  if (initRead == 0 && highRead == 1 && lowRead == 0 && crosstalk == 0) {
    log("  SUCCESS!\n");
    return true;
  } else {
    log("  FAILED!\n");
    return false;
  }
}

void blue() {
  digitalWrite(18, HIGH);
  digitalWrite(19, LOW);
}

void green() {
  digitalWrite(19, HIGH);
  digitalWrite(18, LOW);
}

void off() {
  digitalWrite(19, LOW);
  digitalWrite(18, LOW);
}

void loop() {

  blue();
  bool allOk = true;
  bool results[16];
  for (int i = 0; i < 16; ++i) {
    Pin pin = pins[i];
    if (checkPin(pin)) {
      results[i] = true;
      pin.on();
      delay(100);
      pin.off();
    } else {
      results[i] = false;
      allOk = false;
      off();
    }
  }
  setOutputs();
  log("\nOverall Result: " + String(allOk ? "SUCCESS" : "!!! FAILED !!!") + "\n");
  for (int i = 0; i < 16; ++i) {
    Pin pin = pins[i];
    log("Pin " + String(pin.num) + "\t" + String(results[i] ? "Ok" : "FAILED!"));
  }

  if (allOk) {
    green();
    for (;;) {
      flip();
      delay(500);
      flop();
      delay(500);
    }
  } else {
    for (;;) {
      flip();
      off();
      delay(50);
      flop();
      blue();
      delay(50);
    }
  }
}
