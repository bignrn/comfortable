////////////////////////
//ヘッダーファイル：WiFi_Connection.h
//
//date  : 2021.11.30
//name  : norarun
//
//用途:
//WiFiに接続するための処理をするヘッダーファイル
////////////////////////
#ifndef CLASS_CONECTION
#define CLASS_CONECTION

typedef struct WiFi_Connection{
  char* ssid;   //ルーターのSSID
  char* pass;  //ルーターのパスワード
}W_Connection;

/**
* コネクション
* WiFi接続
*/
void WiFi_Connecting(W_Connection* const w_this);
#endif
