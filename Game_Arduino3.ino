#include <LiquidCrystal.h> // include LCD library
#include <Wire.h>
#define I2C_SLAVE_ADDRESS 13
#define PAYLOAD_SIZE 2

// pins to make the lcd screen light up
const int rs = 13; // RS pin
const int en = 12; // Enable pin
const int d4 = 11; // LCD D4 pin
const int d5 = 10; // LCD D5 pin
const int d6 = 9; // LCD D6 pin
const int d7 = 8; // LCD D7 pin
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // LCD screen with the pins to connect and light up text in the screen itself

const int UP_BUTTON = 3; // button to allow the Player to move upward
const int DOWN_BUTTON = 2; // button to allow the Player to move downward
const int BUZZER_PIN = 6; // pin for the buzzer

int UP_BUTTON_STATE; // whether the player pressed the up button or not
int DOWN_BUTTON_STATE; // whether the player pressed the down button or now

int GAME_SCREEN_MODE = 0; // 0 -> main menu, 1 -> game screen, 2 -> game over screen
  
char START_SCREEN[40] = ""; // start screen buffer
char GAME_SCREEN[40] = ""; // game screen buffer
char GAME_OVER_SCREEN[40] = ""; // game over screen buffer

struct Player { // Player's attributes
  unsigned int LIVES_COUNTER = 3; // Player's lives counter
  unsigned int SCORE = 0; // Player's score
  unsigned int PLAYER_X = 1; // Player's x-axis in game
  unsigned int PLAYER_Y = 0; // Player's y-axis in game
};
struct Player PLAYER; // instance of the player itself

struct Enemy { // Enemy's attributes
  unsigned int ENEMY_X = random(5, 15); // Enemy's x-axis in game
  unsigned int ENEMY_Y = random(0, 2); // Enemy's y-axis in game
};
struct Enemy ENEMY[4]; // instance of the enemies themselves

void start_screen(){ // displays the start screen
  lcd.clear();
  lcd.begin(16, 2); // initalizes the LCD screen to have 16 columns and 2 rows
  lcd.setCursor(5, 0);
  sprintf(START_SCREEN, "%s", "Blast");
  lcd.print(START_SCREEN);
  lcd.setCursor(5, 1);
  sprintf(START_SCREEN, "%s", "Runner");
  lcd.print(START_SCREEN);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  sprintf(START_SCREEN, "%s", "Press the up");
  lcd.print(START_SCREEN);
  lcd.setCursor(0, 1);
  sprintf(START_SCREEN, "%s", "button to play");
  lcd.print(START_SCREEN); 
}

void requestEvents(){
  Serial.println(F("---> recieved request"));
  Serial.print(F("sending value : "));
  Serial.println(PLAYER.SCORE);
  Wire.write(PLAYER.SCORE);
}

void send_to_leaderboards(){
  Wire.beginTransmission(I2C_SLAVE_ADDRESS);
  Wire.onRequest(requestEvents);
  Wire.endTransmission();
  PLAYER.LIVES_COUNTER = 3;
  PLAYER.SCORE = 0;
  PLAYER.PLAYER_X = 1;
  PLAYER.PLAYER_Y = 0;
}

void game_over_screen(){
  lcd.clear();
  lcd.setCursor(3, 0);
  sprintf(GAME_OVER_SCREEN, "%s", "Game Over");
  lcd.print(GAME_OVER_SCREEN);
  lcd.setCursor(3, 1);
  sprintf(GAME_OVER_SCREEN, "%s %d", "Score:", PLAYER.SCORE);
  lcd.print(GAME_OVER_SCREEN);
  delay(1000);
}

void game_screen(int DOWN_BUTTON_STATE){ // displays the game screen with the player and the enemies themselves
  if (PLAYER.LIVES_COUNTER == 0){ // if the player ran out of lives then send the player to the game over screen
    GAME_SCREEN_MODE = 2;
    return;
  }
  lcd.clear();
  // renders the player in the game
  lcd.setCursor(PLAYER.PLAYER_X, PLAYER.PLAYER_Y);
  sprintf(GAME_SCREEN, "%s", "#");
  lcd.print(GAME_SCREEN);
  lcd.setCursor(0, 0);
  // check for player input from the up or down buttons
  if (DOWN_BUTTON_STATE == HIGH){
    if (PLAYER.PLAYER_Y == 0){
      lcd.clear();
      PLAYER.PLAYER_Y++;
      lcd.setCursor(PLAYER.PLAYER_X, PLAYER.PLAYER_Y);
      sprintf(GAME_SCREEN, "%s", "#");
      lcd.print(GAME_SCREEN);
      tone(BUZZER_PIN, 120, 120);
      delay(150);
      lcd.setCursor(0, 0);
  	}
  }
  else if (UP_BUTTON_STATE == HIGH){
    if (PLAYER.PLAYER_Y == 1){
      lcd.clear();
      PLAYER.PLAYER_Y--;
      lcd.setCursor(PLAYER.PLAYER_X, PLAYER.PLAYER_Y);
      sprintf(GAME_SCREEN, "%s", "#");
      lcd.print(GAME_SCREEN);
      tone(BUZZER_PIN, 120, 120);
      delay(150);
      lcd.setCursor(0, 0);
  	}
  }
  // renders the enemies themselves updating them to the left until out of bounds of player-enemy collision
  for (int i = 0; i < 4; i++){
    ENEMY[i].ENEMY_X--;
    lcd.setCursor(ENEMY[i].ENEMY_X, ENEMY[i].ENEMY_Y);
    sprintf(GAME_SCREEN, "%s", "*");
    lcd.print(GAME_SCREEN);
    if (ENEMY[i].ENEMY_X == 0){
       ENEMY[i].ENEMY_X = random(random(4, 10), random(10, 14));
       ENEMY[i].ENEMY_Y = random(0, 2);
    }
    else if (PLAYER.PLAYER_X == ENEMY[i].ENEMY_X && PLAYER.PLAYER_Y == ENEMY[i].ENEMY_Y){
      PLAYER.LIVES_COUNTER--;
       ENEMY[i].ENEMY_X = random(random(4, 10), random(10, 14));
       ENEMY[i].ENEMY_Y = random(0, 2);  
    }
  }
  PLAYER.SCORE++;
  if (PLAYER.SCORE < 10){
    delay(1000);
  }
  else if (PLAYER.SCORE < 30){
    delay(500);
  }
  else if (PLAYER.SCORE < 60){
    delay(250);
  }
}

void setup(){
  Wire.begin(I2C_SLAVE_ADDRESS);
  Serial.begin(9600);
  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  randomSeed(analogRead(0));
  start_screen();
}

void loop(){
  UP_BUTTON_STATE = digitalRead(UP_BUTTON);
  DOWN_BUTTON_STATE = digitalRead(DOWN_BUTTON);
  if (GAME_SCREEN_MODE == 0){ // if at the start screen then play sound effect
    tone(BUZZER_PIN, 120, 120);
    delay(2500);
  }
  if (GAME_SCREEN_MODE == 0 && UP_BUTTON_STATE == HIGH){ // if at the start screen and player presses the up button then send the player to the game screen
    GAME_SCREEN_MODE = 1;
  }
  else if (GAME_SCREEN_MODE == 1){ // if player entered the up button at start screen then run the game
    game_screen(DOWN_BUTTON_STATE);
  }
  else if (GAME_SCREEN_MODE == 2){
    game_over_screen();
    if (UP_BUTTON_STATE == HIGH){
      send_to_leaderboards();
      start_screen();
      GAME_SCREEN_MODE = 0;
    }
  }
}