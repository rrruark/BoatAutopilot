#include <driver/adc.h>

#define HOME_CURRENT_THRESHOLD 1750
#define MOTOR_POSITIVE_PIN 26
#define MOTOR_NEGATIVE_PIN 27
#define ENCODER_PIN_0 22
#define ENCODER_PIN_1 23
#define CURRENT_SENSE_PIN 34
#define LED1 16
#define LED2 17
#define BUTTON1 21
#define BUTTON2 19
#define BUTTON3 18
#define BUZZER 5

volatile int32_t setpoint_position = 0;
volatile int32_t current_position = 0;

int button_count = 0;

void IRAM_ATTR handlePulse()
{
  if(digitalRead(ENCODER_PIN_1))
  {
    current_position++;
    //digitalWrite(LED1,1);
    //digitalWrite(LED2,0);
  }
  else
  {
    current_position--;
    //digitalWrite(LED1,0);
    //digitalWrite(LED2,1);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(MOTOR_POSITIVE_PIN, OUTPUT);
  pinMode(MOTOR_NEGATIVE_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1,1);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2,0);
  digitalWrite(BUZZER,0);

  pinMode(ENCODER_PIN_0, INPUT);
  pinMode(ENCODER_PIN_1, INPUT);
  
  //adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_0), handlePulse, FALLING);
  //attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1, FALLING);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_0), handlePulser, RISING);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_1), handlePulse2, FALLING);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_1), handlePulse2r, RISING);
  delay(100);
  HomeActuator();
}
 
void loop()
{
  if(setpoint_position>70)
  {
    setpoint_position=70;
  }
  if(setpoint_position<0)
  {
    setpoint_position=0;
  }
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
  while (Serial.available() > 0)
  {
    setpoint_position = Serial.parseInt();

    //Do something with the data - like print it
    Serial.println(setpoint_position);
  }

  if(!digitalRead(BUTTON1) || !digitalRead(BUTTON2) || !digitalRead(BUTTON3))
  {
    button_count++;
    if(button_count>20)
    {
      if(!digitalRead(BUTTON1))
      {
        setpoint_position++;
        button_count=0;
      }
      else if(!digitalRead(BUTTON3))
      {
        setpoint_position--;
        button_count=0;
      }      
    }
  }
  else
  {
    button_count=0;
  }  
  //Serial.print(digitalRead(ENCODER_PIN_0));
  //Serial.print(",");
  Serial.print(setpoint_position);
  Serial.print(",");
  Serial.print(current_position);
  Serial.print(",");
  Serial.println(GetMotorCurrent());
  
  delay(10);
}

void HomeActuator()
{
  int temp_current = 0;
  while (temp_current < HOME_CURRENT_THRESHOLD)
  {
    RunMotorNegative();
    //Serial.print("Measured Motor Current is: ");
    Serial.print(analogRead(CURRENT_SENSE_PIN));
    Serial.print(",");
    Serial.println(temp_current);
    delay(100);
    temp_current = int(temp_current*0.8)+int(GetMotorCurrent()*0.2);
  }
  
  StopMotor();
  
  current_position = -1;
}

int GetMotorCurrent()
{
  return analogRead(CURRENT_SENSE_PIN);
}

void RunMotorPositive()
{
  digitalWrite(MOTOR_POSITIVE_PIN, LOW);
  digitalWrite(MOTOR_NEGATIVE_PIN, HIGH);
  digitalWrite(LED2,1);
}

void RunMotorNegative()
{
  digitalWrite(MOTOR_POSITIVE_PIN, HIGH);
  digitalWrite(MOTOR_NEGATIVE_PIN, LOW);
  digitalWrite(LED1,1);

  //Serial.println("Running motor negative");
}

void StopMotor()
{
  digitalWrite(MOTOR_POSITIVE_PIN, HIGH);
  digitalWrite(MOTOR_NEGATIVE_PIN, HIGH);
  digitalWrite(LED1,0);
  digitalWrite(LED2,0);
}