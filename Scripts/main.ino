// include the library code:
#include <LiquidCrystal.h>
#include "IRremote.h"
//#include "pitches.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(6, 7, 8, 9, 10, 11);

////    data    ///////////
char* questions[] = {"3+4-2=?", "4*3-4=?", "3-7+5+8=?", "1+1=?", "4+3=?", "1+2+3+4+5-3-1=?", "2*2*2*2*2*2*2=?"};
int* answers[] = {5, 8, 9, 2, 7, 11, 128};
long order;
const int goalPoint = 2;
//int melody[] = { NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};// notes in the melody:

////   variable   /////
int ans = -1;
int but = -1;
int receiver = 12;
int buzzerPin = 13;
const int redA = 2;
const int greenA = 3;
const int redB = 4;
const int greenB = 5;

////  game mode    ////////////
/* game mode specification
  0: set question
  1: receive sound volume(judge right)
  2: answer questions
  3: true or false (get points)
  4: finish game
  5: receive one more game
  6: startGame
  7: receive command to start question
*/
int mode = 6;
int Right; // 0: playerA, 1: PlayerB
int pointA = 0;
int pointB = 0;
int delayTime;

////   judge answer right ////////
int lagCount = 0;
float maxA = 0;
float maxB = 0;


////   measure sound    ////
// ポート指定用変数設定
const int analogA = A0; 
//const int digitalA = 4;
const int analogB = A3; 
//const int digitalB = 5;

// 受信データ用変数設定
double dataA, dataB;
double voltA, voltB;
const int border = 6; //sound judge
const int lightRed = 2;

////   receive controler ///////
/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'


/*-----( Function )-----*/
void translateIR() // takes action based on IR code received
// describing Remote IR codes 
{
    switch(results.value)
    {
    case 0xFFA25D: Serial.println("POWER"); break;
    case 0xFFE21D: Serial.println("FUNC/STOP"); break;
    case 0xFF629D: Serial.println("VOL+"); break;
    case 0xFF22DD: Serial.println("FAST BACK");    break;
    case 0xFF02FD: Serial.println("PAUSE");    break;
    case 0xFFC23D: Serial.println("FAST FORWARD");   break;
    case 0xFFE01F: Serial.println("DOWN");    break;
    case 0xFFA857: Serial.println("VOL-");    break;
    case 0xFF906F: Serial.println("UP");    break;
    case 0xFF9867: Serial.println("EQ"); but = 10;    break;
    case 0xFFB04F: Serial.println("ST/REPT"); but = 11;    break; // reset
    case 0xFF6897: Serial.println("0"); but = 0;    break;
    case 0xFF30CF: Serial.println("1"); but = 1;    break;
    case 0xFF18E7: Serial.println("2"); but = 2;    break;
    case 0xFF7A85: Serial.println("3"); but = 3;    break;
    case 0xFF10EF: Serial.println("4"); but = 4;    break;
    case 0xFF38C7: Serial.println("5"); but = 5;    break;
    case 0xFF5AA5: Serial.println("6"); but = 6;    break;
    case 0xFF42BD: Serial.println("7"); but = 7;    break;
    case 0xFF4AB5: Serial.println("8"); but = 8;    break;
    case 0xFF52AD: Serial.println("9"); but = 9;    break;
    case 0xFFFFFFFF: Serial.println(" REPEAT");break;  

    default: 
      Serial.println(" other button   ");
    }// End Case

  delay(10); // Do not get immediate repeat
} //END translateIR

void standBuzzer(){
  digitalWrite(buzzerPin, HIGH);
  delay(400);
  digitalWrite(buzzerPin, LOW);
  delay(600);
  digitalWrite(buzzerPin, HIGH);
  delay(400);
  digitalWrite(buzzerPin, LOW);
  delay(600);
  digitalWrite(buzzerPin, HIGH);
  delay(400);
  digitalWrite(buzzerPin, LOW);
  delay(600);
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

void soundBut(){
  digitalWrite(buzzerPin, HIGH);
  delay(50);
  digitalWrite(buzzerPin, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode"); 
  irrecv.enableIRIn();

  lcd.begin(16, 2);
  lcd.clear();               // LCD画面をクリア
  lcd.setCursor(0, 0);       // カーソルの位置を指定

  randomSeed(analogRead(0)); 
  
  pinMode(buzzerPin, OUTPUT); //active buzzer

  pinMode(redA, OUTPUT);
  pinMode(greenA, OUTPUT);
  pinMode(redB, OUTPUT);
  pinMode(greenB, OUTPUT);
}

void loop() {
  /* game mode specification
  0: set question
  1: receive sound volume(judge right)
  2: answer questions
  3: true or false (get points)
  4: finish game
  5: receive one more game
  6: startGame
  7: receive command to start question
  */

  if(mode == 6){
    lcd.clear();
    lcd.print("-- QuicK Buzz --");       // 文字の表示
    lcd.setCursor(0, 1);
    lcd.print("EQ to start");

    mode = 7; // receive EQ mode
  } else if (mode == 7){
    if (irrecv.decode(&results)){
      soundBut();
      translateIR(); 
      if(but == 10){
        mode = 0;
        irrecv.resume(); // receive the next value
        lcd.clear();
        lcd.print("- QuicK Buzz -");
        lcd.setCursor(0, 1);
        lcd.print("Question...");

        standBuzzer();

        //////// initialize variables  /////////
        but = -1;
      } else irrecv.resume(); // receive the next value
    }
  } 
  else if(mode == 0){

    // decide question number
    order = random(sizeof(questions)/sizeof(int));
    //Serial.println(sizeof(questions)/sizeof(int));
    //Serial.println(order);

    lcd.clear();
    lcd.print(questions[order]);       // 文字の表示
    lcd.setCursor(0, 1); 
    lcd.print("Clap Your Hands!!");

    mode = 1;
    delay(100);

  }

  if(mode == 1){
    // 音センサからの情報取得
    dataA = analogRead(analogA);   // アナログデータ
    voltA = (dataA * 5.0*10 /3.3) / 1024 ;
    maxA = max(maxA, voltA);

    dataB = analogRead(analogB);   // アナログデータ
    voltB = (dataB * 5.0*10 /3.3) / 1024 ;
    maxB = max(maxB, voltB);

    lagCount += 1;

    if(lagCount == 50){// 1/20秒ごとに判定する
      if(maxA > lightRed){
        digitalWrite(redA, HIGH);
      } if(maxB > lightRed){
        digitalWrite(redB, HIGH);
      } 

      if(maxA > border && maxA > maxB) {
        //Serial.println("PlayerA");
        lcd.clear();
        lcd.print(questions[order]); 
        lcd.setCursor(0, 1); 
        lcd.print("PlayerA: ANSWER!");
        Right = 0;
        mode = 2;

        digitalWrite(greenA, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(300);
        digitalWrite(buzzerPin, LOW);

      } else if(maxB > border && maxA <= maxB) {
        //Serial.println("PlayerB");
        lcd.clear();
        lcd.print(questions[order]);  
        lcd.setCursor(0, 1); 
        lcd.print("PlayerB: ANSWER!");
        Right = 1;
        mode = 2;

        digitalWrite(greenB, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(300);
        digitalWrite(buzzerPin, LOW);

      } else {
        Serial.println("ERROR! ");
      }
      maxA = 0;
      maxB = 0;
      lagCount = 0;  

    }
    delay(10);

  } else if(mode == 2){

    if (irrecv.decode(&results)){ // have we received an IR signal?
      soundBut();
      translateIR(); 
      if(but >= 0 && but <= 9){
        ans = but;
      } else if(but == 10){ //enter button
        mode = 3;
        but = -1;
        digitalWrite(2, LOW);
        digitalWrite(3, LOW);

      } else if(but == 11){ //reset
        ans = -1;
      } 
      irrecv.resume(); // receive the next value
    }

  } else if(mode == 3){
    // judge answer
    if(ans == answers[order]) {
      lcd.clear();
      delayTime=180;

      if(Right == 0){ // playerA
        pointA += 1; 
        lcd.print("PlayerA: Point!");

        digitalWrite(redA, HIGH);
        digitalWrite(greenA, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(delayTime);
        digitalWrite(redA, LOW);
        digitalWrite(greenA, LOW);
        delay(delayTime);
        digitalWrite(redA, HIGH);
        digitalWrite(greenA, HIGH);
        delay(delayTime);
        digitalWrite(redA, LOW);
        digitalWrite(greenA, LOW);
        digitalWrite(buzzerPin, LOW);
      
      } else {
        pointB += 1;
        lcd.print("PlayerB: Point!");

        digitalWrite(redB, HIGH);
        digitalWrite(greenB, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(delayTime);
        digitalWrite(redB, LOW);
        digitalWrite(greenB, LOW);
        delay(delayTime);
        digitalWrite(redB, HIGH);
        digitalWrite(greenB, HIGH);
        delay(delayTime);
        digitalWrite(redB, LOW);
        digitalWrite(greenB, LOW);
        digitalWrite(buzzerPin, LOW);  
      }
      
      if(pointA >= goalPoint || pointB >= goalPoint) mode = 4;
      else {
        lcd.setCursor(0, 1); 
        lcd.print("Next Question...");
        mode = 0;
        delay(500);
        standBuzzer();
      }
      digitalWrite(redA, LOW);
      digitalWrite(greenA, LOW);
      digitalWrite(redB, LOW);
      digitalWrite(greenB, LOW);

    } else{
      lcd.clear();

      if(Right == 0){ // playerA
        lcd.print("PlayerA: Miss..");  
      } else {
        lcd.print("PlayerB: Miss..");  
      }
      digitalWrite(redA, LOW);
      digitalWrite(greenA, LOW);
      digitalWrite(redB, LOW);
      digitalWrite(greenB, LOW);

      //tone(buzzerPin, melody[0], 250);
      mode = 0;
      delay(1000);
      standBuzzer();
    }
    ans = -1; //initialize answer
    
  } else if(mode == 4){
    lcd.clear();
    if(pointA > pointB) {
      lcd.print("Winner: PlayerA!!");
      digitalWrite(redA, HIGH);
      digitalWrite(greenA, HIGH);
    }
    else if(pointA < pointB) {
      lcd.print("Winner: PlayerB!!");
      digitalWrite(redB, HIGH);
      digitalWrite(greenB, HIGH);
    }
    mode = 5;    
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    lcd.setCursor(0, 1); 
    lcd.print("EQ to NewGame");
  } else if (mode == 5){
    if (irrecv.decode(&results)){ 
      soundBut();
      translateIR(); 
      if(but == 10){
        mode = 6;

        //////// initialize variables  /////////
        but = -1;
        pointA = 0;
        pointB = 0;

        digitalWrite(redA, LOW);
        digitalWrite(greenA, LOW);
        digitalWrite(redB, LOW);
        digitalWrite(greenB, LOW);
      }
      irrecv.resume(); // receive the next value
    }    
  } 

/*
  // 点灯パターンの数
  int max_pattern = sizeof(PATTERNS)/sizeof(PATTERNS[0]);

  for (uint8_t i=0; i<max_pattern; i++) {
    // 8ビット分のデータをシフトレジスタへ送る
    shiftOut(SER, SRCLK, LSBFIRST, PATTERNS[i]); 

    // シフトレジスタの状態をストレージレジスタへ反映させる
    digitalWrite(RCLK,  LOW);
    digitalWrite(RCLK,  HIGH);

    delay(200);
  }
  */

}

