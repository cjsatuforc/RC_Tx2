/* Frequencies */
int PREAM_HIGH_FREQ     = 1500;
int PREAM_LOW_FREQ      = 500;
int FUNCTION_FREQ       = 500;
/* number of function code (N) W1 */
int CMD_ENDCODE         =  4;
int CMD_FORWARD         = 10;
int CMD_FORWARD_TURBO   = 16;
int CMD_TURBO           = 22;
int CMD_FORWARD_LEFT    = 28;
int CMD_FORWARD_RIGHT   = 34;
int CMD_BACKWARD        = 40;
int CMD_BACKWARD_RIGHT  = 46;
int CMD_BACKWARD_LEFT   = 52;
int CMD_LEFT            = 58;
int CMD_RIGHT           = 64;
int CMD_WIPE_LEFT       = 70;
int CMD_WIPE_RIGHT      = 76;
int CMD_PUSH            = 82;
/* output pin */
int SI_PIN              = 9;
int PUSH_PIN            = 4;
int PUSH_LED_STATE      = 11;
int JOYX_PIN            = A0;
int JOYY_PIN            = A1;
int JOYZ_PIN            = A2;
int X_val               = 0;
int Y_val               = 0;
int Z_val               = 0;
int pushState           = 0;

char cmd[4];                      // array that holds command for two or more commands

int middlePoint = 1023 / 2;       // 1023 is the default max value of analog
int wiperMiddlePoint = 512 / 2;   // result will be the middle point of wiper, which added/subtracted by the wiper threshold
int wiperThreshold = 180;         // it helps reading sensitivity of the Z-axis/Wiper
int axisThreshold = 80;           // it helps reading sensitivity X & Y axis

void setup() {
  Serial.begin(9600);
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);
  pinMode(JOYZ_PIN, INPUT);
  pinMode(PUSH_PIN, OUTPUT);
  pinMode(PUSH_LED_STATE, OUTPUT);
  pinMode(SI_PIN, OUTPUT);
}
void loop() {
  X_val = analogRead(JOYX_PIN);
  Y_val = analogRead(JOYY_PIN);
  Z_val = analogRead(JOYZ_PIN);
  pushState = digitalRead(PUSH_PIN);

  if (Y_val > (middlePoint + axisThreshold) || Y_val < (middlePoint - axisThreshold)) {
    cmd[0] = (Y_val > (middlePoint + axisThreshold))  ? 'F' : 'B';
  } else {
    cmd[0] = 0;
  }
  if (X_val > (middlePoint + axisThreshold) || X_val < (middlePoint - axisThreshold)) {
    cmd[1] = (X_val > (middlePoint + axisThreshold)) ? 'L' : 'R';
  } else {
    cmd[1] = 0;
  }
  command();
  //encode_cmd(CMD_BACKWARD);
  //delay(10);
}
void command() {
  String cmds;
  for (int i = 0; i < 2; i++) {
    char c = cmd[i];
    if (c != NULL && c != '\0') {
      cmds += c;
    }
  }
  // check if commands is not empty
  if (cmds != NULL && cmds != '\0') {
    Serial.println(cmds);
    if (cmds == "F") {
      encode_cmd(CMD_FORWARD);
    } else if (cmds == "B") {
      encode_cmd(CMD_BACKWARD);
    } else if (cmds == "L") {
      encode_cmd(CMD_LEFT);
    } else if (cmds == "R") {
      encode_cmd(CMD_RIGHT);
    } else if (cmds == "FR") {
      encode_cmd(CMD_FORWARD_RIGHT);
    } else if (cmds == "FL") {
      encode_cmd(CMD_FORWARD_LEFT);
    } else if (cmds == "BL") {
      encode_cmd(CMD_BACKWARD_LEFT);
    } else if (cmds == "BR") {
      encode_cmd(CMD_BACKWARD_RIGHT);
    } else {
      send_nosignal();
    }
  } else {
    /* if no direction command to transmit then let's check the wiper command*/
    if (Z_val > (wiperMiddlePoint + wiperThreshold) || Z_val < (wiperMiddlePoint - wiperThreshold)) {
      if (Z_val > (wiperMiddlePoint + wiperThreshold)) {
        encode_cmd(CMD_WIPE_RIGHT);
      } else if (Z_val < (wiperMiddlePoint - wiperThreshold)) {
        encode_cmd(CMD_WIPE_LEFT);
      } else {
        send_nosignal();
      }
    } else {
      /* if no wiper command to transmit then let's check the push button state */
      if (pushState == HIGH) {
        encode_cmd(CMD_PUSH);
      } else {
        send_nosignal();
      }
    }
  }
}
void encode_cmd( int code) {
  int w1 = 0, w2 = 0;
  /* start code  4 x W2, 500Hz, Duty Cycle 75% */
  for ( w2 = 0; w2 < CMD_ENDCODE ; w2++) {
    digitalWrite( SI_PIN, HIGH);
    delayMicroseconds( PREAM_HIGH_FREQ);
    digitalWrite( SI_PIN, LOW);
    delayMicroseconds( PREAM_LOW_FREQ);
  }
  /* function code , n x W1, 1KHz, Duty Cycle 50% */
  for ( w1 = 0; w1 < code; w1++) {
    digitalWrite( SI_PIN, HIGH);
    delayMicroseconds(FUNCTION_FREQ);
    digitalWrite( SI_PIN, LOW);
    delayMicroseconds(FUNCTION_FREQ);
  }
}

void send_nosignal() {
  digitalWrite( SI_PIN, LOW);
  //delayMicroseconds(FUNCTION_FREQ);
}
