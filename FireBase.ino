////////////////////////
//関連ファイル：FireBase_Connection.h
//
//date  : 2021.11.30
//
//用途:
//FireBaseに接続。送信と取得を行いう。
//重要：
//WiFiクラスを読み込んでから記述する必要がある。
////////////////////////
#include "FireBase_Connection.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

WiFiClientSecure clients;

//定数
#define roomNo 1

//privateメソッド関数はstaticでスコープ隠蔽
static void firebase_clear(FireBase_Connection* const p_this);

//また、private関数の接頭辞は小文字で始めるなどルールにより
//public/privateを分かりやすくする事が出来ます
static void firebase_clear(FireBase_Connection* const p_this){
  //第一引数に自分自身のインスタンスへのポインタ(this)が格納されるように呼び出すので
  //第一引数のポインタを経由してメンバ変数にアクセスします
  p_this -> privateHOST = "FIREBASSE-HOST-KEY"; //例：test.firebasedatabase.app
  p_this -> privateFB_AUTH = "FIREBASE-SERCRET-PASSWORD";
  p_this -> privateUSER_PATH = "FIREBASE-ROOT-TAG-NAME";
  p_this -> privateROOT_AC =  \
      "-----BEGIN CERTIFICATE-----\n" \
      
      "     WEB-ROOT-RSA-KEY      \n" \
      
      "-----END CERTIFICATE----- \n";    
}//ルートAC認証書を貼り付ける。公開鍵

//コンストラクタは自動生成されないため必ず定義します
void FireBase_construct(FireBase_Connection* const p_this){
  firebase_clear(p_this);
  //LED点灯
  digitalWrite(LED_PIN, HIGH);
  Serial.println("\n\n === FIREBASE === ");
}

/**
 * getFireBase
 * テスト用関数
 * ゲット確認
 * @FireBase_Connection* p_this
 */
bool getFireBase(FireBase_Connection* const p_this){
  bool result = false;
  Serial.println("####Starting connection to server...####");

  //変数ゲット
  char* test_root_ca = p_this -> privateROOT_AC;
  char* host = p_this -> privateHOST;
  char* user_path = p_this -> privateUSER_PATH;
  char* firebase_auth = p_this -> privateFB_AUTH;

  //一度初期化する。
  clients.stop();
  
  //AC認証、接続
  clients.setCACert(test_root_ca);  //これ必ず要る
  
  if(!clients.connect(host, 443)){
    Serial.println("Connection failed!!");
  }else{
    //成功
    Serial.println("Connected to server!");
    
    String req_url_str;
    req_url_str = "GET /";
    req_url_str += String(user_path)+ "/" + roomNo + ".json?auth=";
    req_url_str += String(firebase_auth) + " HTTP/1.1\r\n";
    
    String req_header_str;
    req_header_str = "Host: ";
    req_header_str += String( host ) + "\r\n";
    req_header_str += "Accept: text/event-stream\r\n";
    req_header_str += "Connection: close\r\n";
    req_header_str += "\r\n"; //空白

    Serial.println("####Send Server-Sent Events GET request.####");
    //FirebaseサーバーGETリクエスト送信
    clients.print( req_url_str );
    clients.print( req_header_str );

    Serial.print( req_url_str );
    Serial.print( req_header_str );
    result = true;
  }
  //LED点灯
  digitalWrite(LED_PIN, LOW);
  return result;
}

/**
 * テスト用関数テスト
 * ゲット確認
 * @param FireBase_Connection* p_this //ポインタ
 * @param String key > lock || end
 * @return bool
 */
bool getFireBaseKey(FireBase_Connection* const p_this, String key){
  bool result = false;
  Serial.println("####Starting connection to server...####");

  //変数ゲット
  char* test_root_ca = p_this -> privateROOT_AC;
  char* host = p_this -> privateHOST;
  char* user_path = p_this -> privateUSER_PATH;
  char* firebase_auth = p_this -> privateFB_AUTH;
  
  //AC認証、接続
  clients.setCACert(test_root_ca);  //これ必ず要る
  
  if(!clients.connect(host, 443)){
    Serial.println("Connection failed!!");
  }else{
    //成功
    Serial.println("Connected to server!");
    
    String req_url_str;
    req_url_str = "GET /";
    req_url_str += String(user_path)+ "/" + roomNo + "/" + key + ".json?auth=";
    req_url_str += String(firebase_auth) + " HTTP/1.1\r\n";
    
    String req_header_str;
    req_header_str = "Host: ";
    req_header_str += String( host ) + "\r\n";
    req_header_str += "Accept: text/event-stream\r\n";
    req_header_str += "Connection: close\r\n";
    req_header_str += "\r\n"; //空白

    Serial.println("####Send Server-Sent Events GET request.####");
    //FirebaseサーバーGETリクエスト送信
    clients.print( req_url_str );
    clients.print( req_header_str );

    Serial.print( req_url_str );
    Serial.print( req_header_str );
    result = true;
  }
  return result;
}

/**
 * sendFireBase
 * テスト用関数
 * 送信確認
 * @param FireBase_Connection* p_this //ポインタ
 * @param String key                  //保存先名
 * @param String val                  //保存する値
 */
bool sendFireBase(FireBase_Connection* const p_this, String key, String val){
  bool result = false;
  //変数ゲット
  char* test_root_ca = p_this -> privateROOT_AC;
  char* host = p_this -> privateHOST;
  char* user_path = p_this -> privateUSER_PATH;
  char* firebase_auth = p_this -> privateFB_AUTH;

  //一度クライアントを止める
  clients.stop();

  //AC認証、接続
  clients.setCACert(test_root_ca);  //これ必ず要る
   
  //解除コード以外は弾く
  if(val == "1" || val == "0" || val == "-9"){    
    if(!clients.connect(host, 443)){
      //接続失敗
      Serial.println("Connection failed!");
    }else{
      //接続成功
      Serial.println("Connected to server!");

      //URL作成
      String req_url_str;
      req_url_str = "PATCH /";
      req_url_str += String(user_path);
      req_url_str += "/" + String(roomNo);
      req_url_str += ".json?auth=";
      req_url_str += String(firebase_auth);
      req_url_str += " HTTP/1.1\r\n";
      
      //変更body作成
      String body = "{\"" + key +"\":\"" + val + "\"}";

      //header作成
      String head;
      head = "Host: ";
      head += String( host ) + "\r\n";
      head += "Connection: keep-alive\r\n";
      head += "Content-Length: ";
      head += String( body.length() ) + "\r\n";
      head += "\r\n"; //空行

      //クライアント書き込み
      clients.print(req_url_str);
      clients.print(head);
      clients.print(body);

      //文字出力
      Serial.println(" ===> Send HTTP Patch Request <=== ");
      Serial.print(req_url_str);
      Serial.print(head);
      Serial.print(body);
      Serial.println(); //改行
  
      //チェックメソッド
      checkServerResponsSend();
      result =true;
    }
  }else{
    Serial.println("解除コードではありません。");
  }
  
  delay(100);
  clients.stop();  //絶対に必要
  delay(100);
  
  return result;
}

/**
 * ENDデータを返す debug
 * WiFi_Connectionから移動
 * @param FireBase_Connection* p_this //ポインタ
 * @return int
 *        :err  -2  取得に失敗した時
 *              -9  DBにデータが存在しない時
 */
int returnDataEND(FireBase_Connection* const p_this){
  //変数
  boolean flg2 = false;   //ボタン判定
  boolean flg3 = false;   //ボタン判定DATA
  int idx2 = 0;           //配列インデックス
  int idx3 = 0;           //配列インデックスDATA
  int gIdx = 0;           //配列インデックスdigit
  
  int result = -1;        //戻り値
  
  //定数
  const char key2[] = {
      '\"','d', 'a', 't', 'a','\"',':','}'};   //検索文字(key:end)
  
  const int digit[] = {1000,100,10,1};  //桁付け 
  
  //配列の文字
  const int keysize2 = sizeof(key2) / sizeof(char);   //検索文字サイズ(key:end)
  
  //文字数
  const int mozisuu2 = 4;  //桁数
  
  const int wStart2 = (keysize2 - 1) - 1; //最初の文字(end)
  const int wEnd2   = keysize2 - 1;       //最後の文字(end)

  //取得
  while( clients.available() ){
    char c = clients.read();  //文字取得
    
    if(c == '\r'){
      Serial.print("\\r");  //キャリッジリターン
    }
    if(c == '\n'){
      Serial.print("\\n");  //キャリッジリターン
    }
    
    //検索シークエンス
    /////////////
    //key : end
    //c:取得文字 idx2:キーワードインデックス flg2:ステータス
    //result:戻り値 (追加必要)
    /////////////
    if(c == key2[idx2] || flg2){  //検索文字の比較をしている
      idx2++;
      //欲しいデータだけをを取得
      if(idx2 > wStart2 && key2[wEnd2] != c){
        //初期化
        if(result == -1){
          result = 0;
        }
        //データ追加
        int temp = numChange(c);  //user関数
        //エラーでないか判断
        if(temp != -2 && temp != 10 && gIdx < 4){
          result += temp * digit[gIdx++];
        }else if(temp == 10){
          result = -9;
          gIdx = 4;
        }
      }
      //検索内容に一致し最後の文字まで見るため(lock)
      if(wStart2 == idx2){
        flg2 = true;
      }else if(keysize2 + mozisuu2 == idx2){
        flg2 = false;
      }
    }else{
      idx2=0;
    }
    Serial.print(c) ; //表示
  }
  return result;
}

/**
 * 確認用メソッド
 * @FireBase_Connection* p_this
 * @return int result 
 *        :err  -2  取得に失敗した時
 *              -9  DBにデータが存在しない時
 */
int checkServerRespons(FireBase_Connection* const p_this){
  //変数
  int result = -1;      //戻り値

  ////////////
  //lock
  ////////////
  //変数
  boolean flg = false;                          //ボタン判定
  boolean flg3 = false;                         //ボタン判定
  int idx = 0;                                  //配列インデックス
  int idx3 = 0;                                 //配列インデックス
  //定数
  const char key[] = {
      '\"','l','o','c','k','\"',':','\"','\"'}; //検索文字(key:lock)
  const char keyData[] = {
      '\"','p','a','t','h','\"',':','\"','/','l','o','c','k','\"',',',
      '\"','d', 'a', 't', 'a','\"',':','\"','\"'};   //検索文字(key:data)

  const int keysize = sizeof(key) / sizeof(char);     //検索文字サイズ(key:lock)
  const int keysize3 = sizeof(keyData) / sizeof(char);   //検索文字サイズ(key:data)
  
  const int wStart = (keysize - 1) - 1;   //最初の文字(lock)
  const int wEnd   = keysize - 1;         //最後の文字(lock)
  const int wStart3 = (keysize3 - 1) - 1; //最後の文字(data)
  const int wEnd3   = keysize3 - 1;       //最後の文字(data)
  
  const int mozisuu = 1;  //文字数

  ////////////
  //end
  ////////////
  //変数
  boolean flg2 = false;   //ボタン判定
  boolean flg4 = false;   //ボタン判定DATA
  int idx2 = 0;           //配列インデックス
  int idx4 = 0;           //配列インデックスDATA
  int gIdx = 0;           //配列インデックスdigit
    
  //定数
  const char key2[] = {
      '\"','d', 'a', 't', 'a','\"',':','}'};   //検索文字(key:end)
  const char keyData2[] = {
      '\"','p','a','t','h','\"',':','\"','/','e','n','d','\"',
      ',','\"','d','a','t','a','\"',':','\"','"'};    //検索文字(key:data)
  
  const int digit[] = {1000,100,10,1};  //桁付け 
  
  //配列の文字
  const int keysize2 = sizeof(key2) / sizeof(char);   //検索文字サイズ(key:end)
  const int keysize4 = sizeof(keyData2) / sizeof(char);   //検索文字サイズ(key:end)
  
  //文字数
  const int mozisuu2 = 4;  //桁数
  
  const int wStart2 = (keysize2 - 1) - 1; //最初の文字(end)
  const int wEnd2   = keysize2 - 1;       //最後の文字(end)
  const int wStart4 = (keysize4 - 1) - 1; //最初の文字(data:end)
  const int wEnd4   = keysize4 - 1;       //最後の文字(data:end)
  
  while( clients.available() ){
    char c = clients.read();
    if(c == '\r'){
      Serial.print("\\r");  //キャリッジリターン
    }
    if(c == '\n'){
      Serial.print("\\n");  //キャリッジリターン
    }

    //処理
    //検索シークエンス
    /////////////
    //key : lock
    /////////////
    if(c == key[idx] || flg){  //検索文字の比較をしている flgはデータ取得している時に必要
      idx++;
      //欲しいデータだけをを取得
      if(idx > wStart && key[wEnd] != c){
        if(c == '1'){
          result = 1;
        }else if(c == '0'){
          result = 0;
        }
      }
      //検索内容に一致し最後の文字まで見るため(lock)
      if(wStart == idx){
        flg = true;
      }else if(idx == keysize + mozisuu){
        flg = false;
      }
    }else{
      idx=0;
    }
    /////////////
    //Key : Data lock
    /////////////
    if(c == keyData[idx3] || flg3){
      idx3++;
      //欲しいデータだけをを取得
      if(idx3 > wStart3 && keyData[wEnd3] != c){
        if(c == '1'){
          result = 1;
        }else if(c == '0'){
          result = 0;
        }
      }
      //検索内容に一致し最後の文字まで見るため(lock)
      if(wStart3 == idx3){
        flg3 = true;
      }else if(idx3 == keysize3 + mozisuu){
        flg3 = false;
      }
    }else{
      idx3 = 0;
    }

    //END
    /////////////
    //key : DATA(end)
    /////////////
    if(c == keyData2[idx4] || flg4){
      idx4++;
      //欲しいデータだけをを取得
      if(idx4 > wStart4 && keyData2[wEnd4] != c){
        //初期化
        if(result == -1){
          result = 0;
        }
        //データ追加
        int temp = numChange(c);  //user関数
        //エラーでないか判断
        if(temp != -2 && temp != 10 && gIdx < 4){
          result += temp * digit[gIdx++];
        }else if(temp == 10){
          result = -9;
          gIdx = 4;
        }
      }
      //検索内容に一致し最後の文字まで見るため(lock)
      if(wStart4 == idx4){
        flg4 = true;
      }else if(idx4 == keysize4 + mozisuu2){
        flg4 = false;
      }
    }else{
      idx4 = 0;
    }
    
    Serial.print(c) ; //表示
  }
  return result;
}
/**
 * 送られた文字を数字に変換する
 * @pram char c       検索文字
 * @return int return 結果
 */
int numChange(char c){
  //変数
  int idx = -2;  //インデックス
  //定数
  const char numPad[] = {
      '0','1','2','3','4','5','6','7','8','9','-'};   //キーパッド
  const int lenge = sizeof(numPad) / sizeof(char);     //検索文字サイズ
  
  //変換
  for(int i = 0; i<lenge;i++){
    if(c == numPad[i]){
      idx = i;
    }
  }
  return idx;
}
/**
 * checkServerResponsSend()
 * 内容:sendFireBaseで送ったデータがきちんと送られているか確認するメソッド。
 */
void checkServerResponsSend(){
  //Firebaseサーバからのレスポンスを全て受信し切る事が重要
  Serial.println(" %%%% Firebase server HTTP Response %%%% ");
  while(clients.connected()){
    String resp_str = clients.readStringUntil('\n');//ラインフィールドまで文字列を読み込む
    Serial.println(resp_str);
    
    if(resp_str == "\r"){ //空行検知
      Serial.println("--------Response Headers Received");
      //送信確認用LED点滅x2
      assistLed(2,100,1);
      break;
    }
  }
  //レスポンスヘッダが返ってきた後、body部分が返って来る
  while( clients.available()){
    char c = clients.read();
    Serial.print(c);
  }
  Serial.println("\r\n-------------Body end Received!\n");
}
