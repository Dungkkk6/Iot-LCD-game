#include <LiquidCrystal.h>
#include <EEPROM.h>

// Pin setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int delayTime = 100; 
int count = 0; 
 

// Tạo ký tự tùy chỉnh cho người chơi
byte playerChar[8] = {
  0b01110,
  0b10001,
  0b01110,
  0b11011,
  0b00100,
  0b01010,
  0b10001,
  0b00000
};

// Tạo ký tự tùy chỉnh cho mũi tên
byte customdan[8] = {
  0b00000,
  0b00010,
  0b01110,
  0b11111,
  0b01110,
  0b00010,
  0b00000,
  0b00000
};

// Tạo ký tự tùy chỉnh cho người cầm cung
byte archerChar[8] = {
  0b00000,
  0b00010,
  0b00110,
  0b01110,
  0b00110,
  0b00010,
  0b00000,
  0b00000
};

byte Skull[8] = {
  0b00000,
  0b01110,
  0b10101,
  0b11011,
  0b01110,
  0b01110,
  0b00000,
  0b00000
};

int playerPosition = 1;
const int buttonPin = 7;
const int buzzerPin = 8;  // Thêm chân cho buzzer
int arrowRow = 0;
int arrowPosition = 15;
bool buttonPressed = false;
int score = 0;
int highScore = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, playerChar);
  lcd.createChar(1, archerChar);
  lcd.createChar(2, customdan);
  lcd.createChar(3, Skull);
  
  // Đọc điểm cao từ EEPROM
  EEPROM.get(0, highScore);
  
  showWelcomeScreen();
  waitForStart();
  resetGame();
}

void loop() {
  handlePlayerMovement();
  updateGameState();
  checkCollision();
  displayGame();
  adjustDifficulty();
  delay(delayTime);
}

void showWelcomeScreen() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(1, 1);
  lcd.print("Press to Start");
}

void waitForStart() {
  while (digitalRead(buttonPin) == LOW) {
    delay(100);
  }
}

void resetGame() {
  lcd.clear();
  arrowPosition = 15;
  score = 0;
  delayTime = 100;
}

void handlePlayerMovement() {
  if (digitalRead(buttonPin) == HIGH && !buttonPressed) {
    playerPosition = 1 - playerPosition;
    buttonPressed = true;
    playTone(1000, 50);  // Phát âm thanh khi di chuyển
  }
  if (digitalRead(buttonPin) == LOW) {
    buttonPressed = false;
  }
}

void updateGameState() {
  if (arrowPosition > 0) {
    arrowPosition -= 1;
  } else {
    arrowPosition = 15;
    arrowRow = random(0, 2);
    score++;
    playTone(2000, 50);  // Phát âm thanh khi tránh được mũi tên
  }
}

void checkCollision() {
  if (arrowPosition == 0 && arrowRow == playerPosition) {
    gameOver();
  }
}

void gameOver() {
  playTone(500, 1000);  // Phát âm thanh khi thua
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
  delay(2000);
  
  // Cập nhật và lưu điểm cao mới
  if (score > highScore) {
    highScore = score;
    EEPROM.put(0, highScore);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New High Score!");
    lcd.setCursor(0, 1);
    lcd.print(highScore);
    delay(2000);
  }
  
  waitForStart();
  resetGame();
}

void displayGame() {
  lcd.clear();
  lcd.setCursor(0, playerPosition);
  lcd.write(byte(0));  // Người chơi
  
  lcd.setCursor(15, arrowRow);
  lcd.write(byte(1));  // Người cầm cung
  
  if (arrowPosition > 0) {
    lcd.setCursor(arrowPosition, arrowRow);
    lcd.write(byte(2));  // Mũi tên
  }
  
  lcd.setCursor(13, 0);
  lcd.print(score);
}

void adjustDifficulty() {
  if (score % 5 == 0 && score > 0) {
    if (delayTime > 70) {
      delayTime -= 1;
    }
  }
}

void playTone(int frequency, int duration) {
  tone(buzzerPin, frequency, duration);
}