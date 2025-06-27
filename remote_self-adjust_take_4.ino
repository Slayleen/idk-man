#include <Wire.h>
#include <MPU6050.h>
#include <IRremote.h>

MPU6050 mpu;

const int IR_RECEIVE_PIN = 12;  // Define the pin number for the IR Sensor

const int A_1B = 5;
const int A_1A = 6;
const int B_1B = 9;
const int B_1A = 10;

int baseSpeed = 200;          // Base motor speed
float correctionFactor = 1;  // How strongly to correct based on angle
float angleThreshold = 1.5;   // Degrees threshold for correction

bool isMovingForward = false;
bool isMovingBackward = false;

// unsigned long lastTime = 0;
float zAngle = 0;  // Yaw angle in degrees

float loop_ms = 0.12785; // Selin's constant <3

int badDegrees;

int speed = 150;

void setup() {
  Serial.begin(9600);

  //motor
  pinMode(A_1B, OUTPUT);
  pinMode(A_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);

  //IR remote
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Start the IR receiver // Start the receiver
  Serial.println("REMOTE CONTROL START");

  // Initialize MPU6050
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1)
      ;
  }

  Serial.println("MPU6050 ready!");
  // lastTime = millis();
}


void loop() {
  if (IrReceiver.decode()) {
    //    Serial.println(results.value,HEX);
    String key = decodeKeyValue(IrReceiver.decodedIRData.command);
    if (key != "ERROR") {
      Serial.println(key);
      if (key == "+") {
        speed += 50;
      } else if (key == "-") {
        speed -= 50;
      } else if (key == "2") {
        isMovingForward = true;
        // applyDriftCorrection();
        int leftSpeed = baseSpeed;
        int rightSpeed = baseSpeed;
        analogWrite(A_1A, rightSpeed); //right forward
        analogWrite(A_1B, 0); // right backward
        analogWrite(B_1A, 0); //left backward 
        analogWrite(B_1B, leftSpeed); // left forward
        isMovingForward = false;
        // delay(1000);
      } else if (key == "1") {
        moveLeft(speed);
      } else if (key == "3") {
        moveRight(speed);
      } else if (key == "4") {
        turnLeft(speed);
      } else if (key == "6") {
        turnRight(speed);
      } else if (key == "7") {
        backLeft(speed);
      } else if (key == "9") {
        backRight(speed);
      } else if (key == "8") {
        isMovingBackward = true;
        applyDriftCorrection();
        isMovingBackward = false;
        delay(1000);
      }
      else {
        resetYaw();
      }

      if (speed >= 255) {
        speed = 255;
      }
      if (speed <= 0) {
        speed = 0;
      }
      delay(50);
      stopMove();
    }

    IrReceiver.resume();  // Enable receiving of the next value
  }
}

void applyDriftCorrection() {
  int16_t gx, gy, gz;
  mpu.getRotation(&gx, &gy, &gz);

  // Convert gyro Z axis to degrees per second (yaw rate)
  float gz_dps = gz / 131.0;

  // Calculate time elapsed
  // unsigned long now = millis();
  // lastTime = now;

  // Integrate gyro rate to get angle
  zAngle += gz_dps * loop_ms;

  // Default speeds
  // int leftSpeed = baseSpeed;
  // int rightSpeed = baseSpeed;

  // analogWrite(A_1A, rightSpeed); //right forward
  // analogWrite(A_1B, 0); // right backward
  // analogWrite(B_1A, 0); //left backward 
  // analogWrite(B_1B, leftSpeed); // left forward

  // // Correction logic based on angle
  // if ((zAngle > angleThreshold) && isMovingForward == true) {
  //   // Robot has turned right too much, slow right motor to correct left
  //   // leftSpeed -= correctionFactor * abs(zAngle);
  //   analogWrite(A_1A, rightSpeed); //right forward
  //   analogWrite(A_1B, 0); // right backward
  //   analogWrite(B_1A, 0); //left backward 
  //   analogWrite(B_1B, leftSpeed); // left forward

  // } else if (zAngle < -angleThreshold && isMovingForward == true) {
  //   // Robot has turned left too much, slow left motor to correct right
  //   // rightSpeed -= correctionFactor * abs(zAngle);
  //   analogWrite(A_1A, rightSpeed); //right forward
  //   analogWrite(A_1B, 0); // right backward
  //   analogWrite(B_1A, 0); //left backward 
  //   analogWrite(B_1B, leftSpeed); // left forward
  // }
  // } else if ((zAngle > angleThreshold) && isMovingBackward == true) {
  //   // Robot has turned right too much, slow right motor to correct left
  //   analogWrite(A_1A, 0);
  //   analogWrite(A_1B, rightSpeed);
  //   analogWrite(B_1A, leftSpeed);
  //   analogWrite(B_1B, 0);
  //   leftSpeed += correctionFactor * abs(zAngle);
  // } else if (zAngle < -angleThreshold && isMovingBackward == true) {
  //   // Robot has turned left too much, slow left motor to correct right
  //   analogWrite(A_1A, 0);
  //   analogWrite(A_1B, rightSpeed);
  //   analogWrite(B_1A, leftSpeed);
  //   analogWrite(B_1B, 0);
  //   rightSpeed += correctionFactor * abs(zAngle);
  // }
  // end of if

  // Constrain speeds to 0–255
  // leftSpeed = constrain(leftSpeed, 0, 255);
  // rightSpeed = constrain(rightSpeed, 0, 255);


  // Serial.print("Yaw angle: ");
  // Serial.print(zAngle);
  // Serial.print(" | L: ");
  // Serial.print(leftSpeed);
  // Serial.print(" R: ");
  // Serial.println(rightSpeed);

  // Serial.print("left speed: ");
  // Serial.println(leftSpeed);
  // Serial.print("right speed: ");
  // Serial.println(rightSpeed);
}

  void turnRight(int speed) {
    analogWrite(A_1B, speed);
    analogWrite(A_1A, 0);
    analogWrite(B_1B, speed);
    analogWrite(B_1A, 0);
  }

  void turnLeft(int speed) {
    analogWrite(A_1B, 0);
    analogWrite(A_1A, speed);
    analogWrite(B_1B, 0);
    analogWrite(B_1A, speed);
  }

  void moveLeft(int speed) {
    analogWrite(A_1B, 0);
    analogWrite(A_1A, speed);
    analogWrite(B_1B, 0);
    analogWrite(B_1A, 0);
  }

  void moveRight(int speed) {
    analogWrite(A_1B, 0);
    analogWrite(A_1A, 0);
    analogWrite(B_1B, speed);
    analogWrite(B_1A, 0);
  }

  void backLeft(int speed) {
    analogWrite(A_1B, speed);
    analogWrite(A_1A, 0);
    analogWrite(B_1B, 0);
    analogWrite(B_1A, 0);
  }

  void backRight(int speed) {
    analogWrite(A_1B, 0);
    analogWrite(A_1A, 0);
    analogWrite(B_1B, 0);
    analogWrite(B_1A, speed);
  }

  void stopMove() {
    analogWrite(A_1B, 0);
    analogWrite(A_1A, 0);
    analogWrite(B_1B, 0);
    analogWrite(B_1A, 0);
  }

  void resetYaw() {
  if (abs(zAngle) > angleThreshold) {
    zAngle = 0;
    // lastTime = millis();
  }
}


  String decodeKeyValue(long result) {
    switch (result) {
      case 0x16:
        return "0";
      case 0xC:
        return "1";
      case 0x18:
        return "2";
      case 0x5E:
        return "3";
      case 0x8:
        return "4";
      case 0x1C:
        return "5";
      case 0x5A:
        return "6";
      case 0x42:
        return "7";
      case 0x52:
        return "8";
      case 0x4A:
        return "9";
      case 0x9:
        return "+";
      case 0x15:
        return "-";
      case 0x7:
        return "EQ";
      case 0xD:
        return "U/SD";
      case 0x19:
        return "CYCLE";
      case 0x44:
        return "PLAY/PAUSE";
      case 0x43:
        return "FORWARD";
      case 0x40:
        return "BACKWARD";
      case 0x45:
        return "POWER";
      case 0x47:
        return "MUTE";
      case 0x46:
        return "MODE";
      case 0x0:
        return "ERROR";
      default:
        return "ERROR";
        resetYaw();
    }
  }