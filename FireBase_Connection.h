////////////////////////
//ヘッダーファイル：FireBase_Connection.h
//
//date  : 2021.11.30
//
//用途:
//FireBaseに接続。送信と取得を行いうヘッダーファイル
//Webサイト参考に作成
////////////////////////
#ifndef CLASS_FIREBASE
#define CLASS_FIREBASE

//クラス定義
typedef struct FireBase_Connection{
  char* privateHOST;       //ホストサーバ
  char* privateFB_AUTH;    //シークレットキー
  char* privateUSER_PATH;  //ユーザーパス
  char* privateROOT_AC;    //root認証キー
}Firebase;

/**
 * テスト用関数
 * ゲット確認
 * @param FireBase_Connection* p_this //ポインタ
 * @return bool
 */
bool getFireBase(FireBase_Connection* const p_this);
/**
 * テスト用関数テスト
 * ゲット確認
 * @param FireBase_Connection* p_this //ポインタ
 * @param String key
 * @return bool
 */
bool getFireBase(FireBase_Connection* const p_this, String key);
/**
 * sendFireBase
 * テスト用関数
 * 送信確認
 * @param FireBase_Connection* p_this //ポインタ
 * @param String key                  //保存先名
 * @param String val                  //保存する値
 * @return bool
 */
bool sendFireBase(FireBase_Connection* const p_this, String key, String val);
/**
 * LOCKデータを返す TEST
 * WiFi_Connectionから移動
 * @param FireBase_Connection* p_this //ポインタ
 * @return int
 */
int returnDataLOCK(FireBase_Connection* const p_this);
/**
 * ENDデータを返す TEST
 * WiFi_Connectionから移動
 * @param FireBase_Connection* p_this //ポインタ
 * @return int
 */
int returnDataEND(FireBase_Connection* const p_this);
/**
 * 更新取得
 * WiFi_Connectionから移動
 * @param FireBase_Connection* p_this //ポインタ
 */
int checkServerRespons(FireBase_Connection* const p_this);
/**
 * 更新取得(send)
 * 内部確認用
 * @param FireBase_Connection* p_this //ポインタ
 */
void checkServerResponsSend(FireBase_Connection* const p_this);
#endif
