const int FLOW_PIN = 2;
volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;
float totalLitros = 0.0;
const float K = 7.5;

void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulseCounter, RISING);
  Serial.println("Flujo(L/min) ");//| Flujo(m3/min) | Total(L)
}

void loop() {
  if (millis() - lastTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_PIN));

    unsigned long pulses = pulseCount;
    pulseCount = 0;
    lastTime = millis();

    float flowLPM  = pulses / K;
    float flowM3   = flowLPM / 1000.0;
    totalLitros   += flowLPM / 60.0;

    Serial.println(flowLPM, 3);
    //Serial.print("| ");
    //Serial.print(flowM3, 6);
    //Serial.print("| ");
    //Serial.print(totalLitros, 4);
    //Serial.println(" ");

    attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pulseCounter, RISING);
  }
}