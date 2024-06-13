#include "state_machine.h"

StateMachine stateMachine;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  stateMachine = StateMachine(STATE_INIT);

  delay(1000);

  Serial.println("Setup done");
}

void loop() {
  stateMachine.update();
  delay(500);
}
