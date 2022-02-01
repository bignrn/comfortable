////////////////////////
//関連ファイル：WiFi_Connection.h
//
//date  : 2021.11.30
//
//用途:
//WiFiにつなげる処理を行う。
////////////////////////
#include "WiFi_Connection.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoOTA.h>
WiFiClientSecure client;

//定数
#define RECONE_MAX 15   //リコネクション回数

static void wifi_clear(W_Connection* const w_this);

static void wifi_clear(W_Connection* const w_this){
    //1~3号館
    w_this -> ssid = "YOUR-SSID-KEY";      //ルーターのSSID
    w_this -> pass = "YOUR-PASSWORD-KEY"; //ルーターのパスワード
}

/**
 * コンストラクト
 */
void WiFi_construct(W_Connection* const w_this){
  wifi_clear(w_this);
  Serial.println("\n === WiFi === ");
}

/**
 * コネクション
 * WiFi接続
 */
void WiFi_Connecting(W_Connection* const w_this){
  //変数
  int count = 0;  //改行用カウント

  char* ssid = w_this -> ssid;
  char* pass = w_this -> pass;

  //接続
  WiFi.begin(ssid, pass);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    if(count == RECONE_MAX){
      count = 0;
      Serial.print(" ...reflesh<\n");
      ESP.restart();  //再起動
    }else{
      count++;
      Serial.print(".");
    }
    delay(500);
  }
  Serial.print("\nconnected:");
  Serial.print(WiFi.localIP()); //IPアドレス表示
  Serial.println(); //改行
}
