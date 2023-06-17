#include <driver/adc.h>

#define HOME_CURRENT_THRESHOLD 2000
#define MOTOR_POSITIVE_PIN 26
#define MOTOR_NEGATIVE_PIN 27
#define ENCODER_PIN_0 22
#define ENCODER_PIN_1 23

int32_t setpoint_position = 0;

volatile int32_t current_position = 0;

void IRAM_ATTR handlePulse()
{
  current_position++;
}

void setup()
{
  Serial.begin(9600);

  pinMode(MOTOR_POSITIVE_PIN, OUTPUT);
  pinMode(MOTOR_NEGATIVE_PIN, OUTPUT);
  
  pinMode(ENCODER_PIN_0, INPUT);
  pinMode(ENCODER_PIN_1, INPUT);
  
  //adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_0), handlePulse, RISING);

  HomeActuator();
}

void loop()
{
  int32_t error;
  
  error = setpoint_position - current_position;
  
  if (error < 0)
  {
    RunMotorNegative();
  }
  else if (error > 0)
  {
    RunMotorPositive();
  }
  else
  {
    StopMotor();
  }
}

void HomeActuator()
{
  while (GetMotorCurrent() < HOME_CURRENT_THRESHOLD)
  {
    RunMotorNegative();
    Serial.print("Measured Motor Current is: ");
    Serial.println(GetMotorCurrent());
    Serial.print("Current Position is: ");
    Serial.println(current_position);
    delay(500);
  }
  
  StopMotor();
  current_position = 0;
}

int GetMotorCurrent()
{
  return adc1_get_raw(ADC1_CHANNEL_0);
}

void RunMotorPositive()
{
  digitalWrite(MOTOR_POSITIVE_PIN, HIGH);
  digitalWrite(MOTOR_NEGATIVE_PIN, LOW);
}

void RunMotorNegative()
{
  digitalWrite(MOTOR_POSITIVE_PIN, LOW);
  digitalWrite(MOTOR_NEGATIVE_PIN, HIGH);
  Serial.println("Running motor negative");
}

void StopMotor()
{
  digitalWrite(MOTOR_POSITIVE_PIN, LOW);
  digitalWrite(MOTOR_NEGATIVE_PIN, LOW);
}