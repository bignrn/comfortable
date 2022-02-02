///////////////
//mainファイル
//
//file  : main
//date  : 2021.11.30
//name  : norarun
//
///////////////
#include <iostream>
#include <String>
#include <ArduinoOTA.h>

//ユーザーヘッダ
#include "WiFi_Connection.h";
#include "FireBase_Connection.h";

//グローバル定数
const int LED_PIN = 27;           //GPIO(Num) LEDピン
const int SW_PIN  = 26;           //GPIO(Num) スイッチピン
const int SIRIAL  = 112500;       //シリアルポート番号
const int SERVO_MAX_WIDHT_MS = 180; //サーボモータの最大角度
const int SERVO_MIN_WIDHT_MS = 0; //サーボモーターの最低角度
const int LIMIT = 5;              //終了時間後の計算・時刻表示の間隔

//サーボモータ定数
const int SERVO_PIN = 25;         //GPIO(Num) サーボモータピン
const int LEDC_CHAN_3 = 3;        //チャンネル
const int LEDC_TIMER_BIT = 8;     //PWMタイマーの制度設定
const int LEDC_SERVO_FREQ = 130;  //サーボ信号の１サイクル　100Hz:ms

//変数
String mSitu = "-2";   //受け取ったデータの履歴
bool  swFlg = true; //スイッチの長押し防止
bool  tFlg = true;    //時間の初期値が入ったか判断

int nowTime = -5;   //現在時刻
int oldTime = -5;   //時間の履歴
int dbEndTime = -5; //DBのデータ保存場所

struct tm timeInfo;//時刻を格納するオブジェクト
char s[20];//文字格納用

//インスタンス、型宣言
WiFi_Connection wifi;
FireBase_Connection firebase;

/**
 * セットアップ
 */
void setup() {
  pinMode(LED_PIN, OUTPUT);          //LEDピンの設定
  pinMode(SW_PIN, INPUT_PULLUP);     //スイッチの設定
  
  //シリアルモニター
  Serial.begin(SIRIAL);
  
  //サーボモータ設定
  ledcSetup(LEDC_CHAN_3, LEDC_SERVO_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(SERVO_PIN, LEDC_CHAN_3);

  //立ち上げ
  standBy();

  //LED点灯
  digitalWrite(LED_PIN, HIGH);
  
  //WiFi接続
  WiFi_construct(&wifi);  //コンストラクタ
  WiFi_Connecting(&wifi);  //WiFiのメソッド
  
  //LED点灯
  digitalWrite(LED_PIN, LOW);
  
  //FireBase
  FireBase_construct(&firebase);//コンストラクタ呼び出し
  if(!getFireBaseKey(&firebase,"end")) ESP.restart();  //ミス再起動 end
  
  //時間取得
  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");//NTPの設定
  oldTime = getTime();
}

/**
 * Loop
 * エラー
 * -1:FireBaseのデータの取得に失敗
 * -2:mSituの値が存在しないとき(-1に関連している)
 * -5:timeの初期化
 * -9:FireBaseのENDの時間が存在しない
 */
void loop() { 
  ////////////////
  //time初期化
  ////////////////
  if(dbEndTime != -1 && dbEndTime != -5 && tFlg){
    if(!getFireBase(&firebase)) ESP.restart();  //ミス再起動
    tFlg = false;
  }else if(dbEndTime == -5 || dbEndTime == -1 && tFlg){
    dbEndTime = returnDataEND(&firebase);   //データ取得
    Serial.printf("****dbEndTime = %d \n",dbEndTime);
  }
  ////////////////
  //サーボモータ　又は、END時間の取得
  ////////////////
  //intを返す方法テスト
  int dbRst;
  if(!tFlg){  //時間取得後に全体のやつに移行された(db)
    dbRst =  checkServerRespons(&firebase);
    if(dbRst != -1){
      Serial.printf("**main-loop***リザルト=>%d\n",dbRst);
    }
    if(dbRst != -1 && (dbRst == 1 || dbRst == 0)){
      mSitu = String(dbRst);        //ステータス記録
      servoMoter(mSitu);    //モーター起動
    }else if(dbRst != -1){
      if(dbEndTime < 0){
        dbEndTime = 0;
      }
      dbEndTime = dbRst;    //時間の取得
    }
  }
  
  ////////////////
  //スイッチ
  ////////////////
  if(swFlg && digitalRead(SW_PIN)==LOW && dbEndTime != -9){
    Serial.println("スイッチの操作を行います。");
    if(mSitu == "1"){
      //debug
      mSitu="0";  //開錠
    }else if(mSitu == "0"){
      mSitu="1";  //施錠
    }else{
      Serial.println("スイッチエラー：切り替えできません。");
    }
    //1,0じゃないときは通らない。
    if(mSitu=="1"||mSitu=="0"){
      /*
       * 送信methode 引数(コンストラクタ、key, val)
       * {" key " : " val "}
       */
      if(!sendFireBase(&firebase,"lock",(String)mSitu)) ESP.restart();  //ミス時再起動
      tFlg = true;  //元に戻る
    }
    swFlg = false;  //スイッチ記憶
  }else if(digitalRead(SW_PIN)==HIGH){
    //長押しの防止
    swFlg = true;   //スイッチ記憶
  }else if(dbEndTime == -9){
    Serial.println("ボタンエラー：不正アクセスです。");
  }

  ////////////////
  //時間取得
  ////////////////  
  nowTime = getTime();
  //時刻表示
  if(nowTime % LIMIT == 0 && oldTime != nowTime){
    Serial.printf("gettime = %d\n",nowTime);
    oldTime = nowTime;
  }

  ////////////////
  //終了時間から5分後に自動で閉める
  //時間が変わっても施錠できるまで繰り返す。(tFlg)<検証まち
  ////////////////
  if(dbEndTime != -9 && nowTime != -5 && !tFlg){
    if(nowTime >= (dbEndTime + LIMIT) && dbEndTime >= 0){
      Serial.println("\n=======================");
      Serial.println("鍵の確認作業に入ります。");
      Serial.println("=======================");

      //施錠処理
      if(mSitu == "0"){
        Serial.println("問題を検出--->施錠作業に移行。");
        mSitu="1";  //施錠　-> dbRst
      }else if(mSitu == "1"){
        mSitu = "1";  //終了フェーズに値を切り替え
      }else if(mSitu != "0" && mSitu != "1"){
        Serial.println("終了時間エラー：切り替えできません。");
      }

      //send
      //１ではないときは通らない。
      if(mSitu == "1" && dbEndTime != -9){
        /*
         * 送信methode 引数(コンストラクタ、key, val)
         * {" key " : " val "}
         */
        if(!sendFireBase(&firebase,"lock",(String)mSitu)) ESP.restart();  //ミス時再起
        tFlg = true;  //DBが元に戻る
      }
      
      //初期値に戻す db:初期値ではない　flg:sendの状態　Situ:鍵を施錠するコード
      if(dbEndTime != -9 && tFlg && mSitu=="1"){
        dbEndTime = -9;
        if(!sendFireBase(&firebase,"end",(String)dbEndTime)) ESP.restart();  //ミス時再起動
      }
    }
  }
  delay(500);
}

/**
 * 起動時に確認
 * 一回のみ実行するはず
 */
boolean standBy(){
  boolean result = true;
  
  //シリアルポート
  Serial.println("\n====================\n");
  Serial.println("ESP-WROOM-32 Start UP...\n");
  Serial.println("=====================\n");
  //起動時のLED点滅
  assistLed(0, 100, 0);
  
  return result;
}

/**
 * サーボモータを動かす
 */
void servoMoter(String flg){  
  //サーボモータ
  if(flg == "1"){
    Serial.println("モータ施錠");
    ledcWrite(LEDC_CHAN_3, 225);
  }else if(flg == "0"){
    Serial.println("モータ解除");
    ledcWrite(LEDC_CHAN_3, 50);
  }
  //ドア解除確認用LED
  if(flg=="0"||flg=="1") assistLed(2, 100, 1);
  
  Serial.println("");
  delay(1000);
}
/**
 * 現在時刻を取得
 */
int getTime(){
  int result;
  getLocalTime(&timeInfo);//tmオブジェクトのtimeInfoに現在時刻を入れ込む
  result = (timeInfo.tm_hour*100) + timeInfo.tm_min;
  return result;
}
/**
 * ledの点滅アシスト
 * @param int     count       //点滅する回数
 * @param int     wait        //待ち時間
 * @param String  pattern      //点滅パターン
 *        0...Long
 *        1...short
 */
void assistLed(int count,int wait,int pattern){  
  switch(pattern){
    case 0 :
      digitalWrite(LED_PIN, HIGH);
      delay(wait);
      digitalWrite(LED_PIN, LOW);
      delay(100);
      Serial.println("****LEDメソッドLONG");
      break;
    case 1 :
      while(count > 0){
        digitalWrite(LED_PIN, HIGH);
        delay(wait);
        digitalWrite(LED_PIN, LOW);
        delay(wait);
        count--;
      }
      Serial.println("****LEDメソッドSHORT");
      break;
  }
}
