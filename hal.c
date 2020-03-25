// *************************************************************************
//   ロボット名	： 電研ベーシックDCマウス、サンシャイン
//   概要		： サンシャインのHAL（ハードウエア抽象層）ファイル
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.10.31			外川			新規（ファイルのインクルード）
// *************************************************************************/

//**************************************************
// インクルードファイル（include）
//**************************************************
#include <typedefine.h>						// 定義
#include <iodefine.h>						// I/O
#include <hal.h>							// HAL


//**************************************************
// 定義（define）
//**************************************************
#define		BAT_GOOD			(2296)			// 残量減ってきた（黄色）、1セル3.7V以上： 2297 = ( 3700mV * 2セル ) / 4(分圧) / 3300 * 4096 - 1
#define		BAT_LOW				(2109)			// 残量やばい！！（赤色）、1セル3.4V以上： 2109 = ( 3400mV * 2セル ) / 4(分圧) / 3300 * 4096 - 1
#define 	GYRO_REF_NUM		( 100 )			// ジャイロのリファレンス値
#define 	SEN_WAIT_CNT		( 175 )			// センサの発光安定待ち（実験値）
#define		ENC_RESET_VAL		(32768)			// エンコーダの中間値
#define		ENC_R_TSTR			(MTUB.TSTR.BIT.CST1)	// 右エンコーダパルスカウント開始
#define		ENC_L_TSTR			(MTUB.TSTR.BIT.CST2)	// 左エンコーダパルスカウント開始
#define		DCM_ENA				(PORTB.DR.BIT.B3)		// DCMの有効/無効
#define		DCM_R_IN1			(PORTB.DR.BIT.B6)		// DCM右IN1
#define		DCM_R_IN2			(PORTB.DR.BIT.B7)		// DCM右IN2
#define		DCM_L_IN1			(PORTB.DR.BIT.B4)		// DCM左IN1
#define		DCM_L_IN2			(PORTB.DR.BIT.B5)		// DCM左IN2
#define		DCM_R_TIMER			(MTUB.TSTR.BIT.CST3)	// DCM右タイマ開始
#define		DCM_L_TIMER			(MTUB.TSTR.BIT.CST0)	// DCM左タイマ開始
#define		DCM_R_TIORA			(MTU9.TIORH.BIT.IOA)	// DCM右ピン出力設定A
#define		DCM_R_TIORB			(MTU9.TIORH.BIT.IOB)	// DCM右ピン出力設定B
#define		DCM_L_TIORA			(MTU6.TIORH.BIT.IOA)	// DCM左ピン出力設定A
#define		DCM_L_TIORB			(MTU6.TIORH.BIT.IOB)	// DCM左ピン出力設定B
#define		DCM_R_TCNT			(MTU9.TCNT)				// DCM右カウント値
#define		DCM_L_TCNT			(MTU6.TCNT)				// DCM左カウント値
#define		DCM_R_GRA			(MTU9.TGRA)				// DCM右周期
#define		DCM_R_GRB			(MTU9.TGRB)				// DCM右Duty比
#define		DCM_L_GRA			(MTU6.TGRA)				// DCM左周期
#define		DCM_L_GRB			(MTU6.TGRB)				// DCM左Duty比


//**************************************************
// 列挙体（enum）
//**************************************************

//**************************************************
// 構造体（struct）
//**************************************************
/* 距離センサ情報（全センサ共通） */
typedef struct{
	SHORT		s_now;						// LED 点灯中の距離センサの現在値           ( AD 値 )
	SHORT		s_old;						// LED 点灯中の距離センサの1つ前の値        ( AD 値 )
	SHORT		s_limit;					// 距離センサの閾値                         ( AD 値 ) ( この値より大きい場合、壁ありと判断する )
	SHORT		s_ref;						// 区画の中心に置いた時の距離センサの基準値 ( AD 値 )
	SHORT		s_offset;					// LED 消灯中の距離センサの値               ( AD 値 )
	SHORT		s_ctrl;						// 制御有効化する際の閾値                   ( AD 値 ) 主に前壁で使用
	SHORT		s_noCtrl;					// 壁に近すぎるため制御無効化する際の閾値   ( AD 値 ) 主に前壁で使用
}stDIST_SEN;


//**************************************************
// グローバル変数
//**************************************************
/* バッテリ監視 */
PRIVATE USHORT	us_BatLvAve = 4095;							// バッテリ平均値（AD変換の最大値で初期化）

/* ジャイロセンサ */
PRIVATE SHORT s_GyroVal; 					  				// ジャイロセンサの現在値
PRIVATE SHORT s_GyroValBuf[8];								// ジャイロセンサのバッファ値
PUBLIC FLOAT  f_NowAngle;			 						// ジャイロセンサの現在角度
PRIVATE LONG  l_GyroRef; 									// ジャイロセンサの基準値

/* 距離センサ */
PRIVATE stDIST_SEN	st_sen[DIST_SEN_NUM];					// 距離センサ


//**************************************************
// プロトタイプ宣言（ファイル内で必要なものだけ記述）
//**************************************************




// *************************************************************************
//   機能		： HALを初期化する。
//   注意		： なし
//   メモ		： 内部変数などをクリアする。
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.27			外川			新規
// *************************************************************************/
PUBLIC void HAL_init( void )
{
	/* ジャイロセンサ */
	f_NowAngle = 0;				// ジャイロセンサの現在角度
	l_GyroRef  = 0;				// ジャイロセンサの基準値

	/* 距離センサ */
	memset( st_sen, 0, sizeof(st_sen) );				// 距離センサ(全センサ共通)
	
	/* エンコーダ */
	ENC_Sta();
	
	/* DCM*/
	DCM_ENA = ON;

}


// *************************************************************************
//   機能		： 1文字出力
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC void SCI_putch(char data)
{
	while(1) {
		if (SCI1.SSR.BYTE & 0x80) {			// 送信バッファの空きチェック
			SCI1.TDR = data;
			SCI1.SSR.BYTE = SCI1.SSR.BYTE & 0x40;
			break;
		}
	}
}


// *************************************************************************
//   機能		： 文字列出力
//   注意		： なし
//   メモ		： "\n"のみで"CR+LF"出力を行う。
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC void SCI_puts(char *buffer)
{
	char data;
	
	/* nullまで出力 */
	while( (data = *( buffer++ ) ) != 0 ){
		
		/* データの値に応じて出力を変える */
		if (data == 0x0a) {
			SCI_putch(0x0d);		// CR出力
			SCI_putch(0x0a);		// LF出力
		} else {
			SCI_putch(data);		// 1文字出力
		}
	}
}


// *************************************************************************
//   機能		： 文字列出力
//   注意		： なし
//   メモ		： 文字列長さ指定付き"/\n"のみ "CR+LF"出力を行う。
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC void SCI_putsL(char *buffer, int len)
{
	int i;
	char data;
	
	for( i=0; i<len; i++ ){
		data=*(buffer++);
		
		/* データの値に応じて出力を変える */
		if (data == 0x0a) {
			SCI_putch(0x0d);		// CR出力
			SCI_putch(0x0a);		// LF出力
		} else {
			SCI_putch(data);		// 1文字出力
		}
	}
}


// *************************************************************************
//   機能		： 1文字出力用ラッパー関数
//   注意		： なし
//   メモ		： printfなどの低レベル出力に使用される
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC void charput(unsigned char data)
{
	SCI_putch(data);
}

void PutNum(char data){
	char NUM[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	char numdata = NUM[data>>4];
	SCI_putch(numdata);
	numdata=NUM[data&0x0f];
	SCI_putch(numdata);
}
// *************************************************************************
//   機能		： 入力バッファチェック
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC int SCI_chkRecv(void)
{
	/* データの受信チェック */
	if (SCI1.SSR.BYTE & 0x40) {
		return 1;		// 受信データあり
	}	else {
		return 0;		// 受信データなし
	}
}


// *************************************************************************
//   機能		： 1文字入力（char型版）
//   注意		： なし
//   メモ		： エコーバックなし。
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC char SCI_getch(void)
{
	char data;
	
	while(1){
		/* データの受信チェック */
		if ( SCI_chkRecv() ) {
			data = SCI1.RDR;						// データ受信
			SCI1.SSR.BYTE = SCI1.SSR.BYTE & 0x80;
			break;
		}
	}
	return data;
}

// *************************************************************************
//   機能		： 1文字入力（unsigned char型版）
//   注意		： なし
//   メモ		： エコーバックなし。
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC unsigned char SCI_getch_uc(void)
{
	unsigned char data;
	while(1){
		/* データの受信チェック */
		if ( SCI_chkRecv() ) {
			data = SCI1.RDR;						// データ受信
			SCI1.SSR.BYTE = SCI1.SSR.BYTE & 0x80;
			break;
		}
	}
	return data;
}


// *************************************************************************
//   機能		： 文字列入力
//   注意		： なし
//   メモ		： CRコードまで/最大255文字/エコーバックあり
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC int SCI_gets(char *buffer)
{
	char data;
	int  i = 0;

	while(1){
		data = SCI_getch();		// 1文字入力
		*buffer = data;
		SCI_putch(data);		// 1文字出力(エコーバック)
		buffer++;
		i++;
		if (i > 255)      break;	// 最大文字数に到達
		if (data == 0x0D) break;	// CRコードまで受信した
	}
	*buffer = (unsigned char)0;		// null
	
	return i;						// 入力文字数を返却
}


// *************************************************************************
//   機能		： 1文字入力用ラッパー関数
//   注意		： なし
//   メモ		： scanfなどの低レベル出力に使用される
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC unsigned char charget(void)
{
	return SCI_getch_uc();
}


// *************************************************************************
//   機能		： バッテリ電圧を取得する
//   注意		： なし
//   メモ		： 直前5回の平均値
//   引数		： なし
//   返り値		： 電圧[mV]
// **************************    履    歴    *******************************
// 		v1.0		2013.11.13			外川			新規
// *************************************************************************/
PUBLIC FLOAT BAT_getLv(void)
{
	FLOAT f_val = (FLOAT)( us_BatLvAve + 1 );		// 値は0から始まるから1を加算
	
	return ( f_val / 4096 * 3300 * 4.0f );
}


// *************************************************************************
//   機能		： バッテリ監視用ポーリング関数
//   注意		： なし
//   メモ		： 割り込みから実行される
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.16			外川			新規
// *************************************************************************/
PUBLIC void BAT_Pol( void )
{
	static USHORT 	us_batLv[5] = { 4095, 4095, 4095, 4095, 4095 };		// バッテリレベル（AD変換の最大値で初期化）

	/* ================================================== */
	/*  平均値を取得するため、データのバッファ処理を行う  */
	/* ================================================== */
	/* バッファをシフト */
	us_batLv[1] = us_batLv[0];
	us_batLv[2] = us_batLv[1];
	us_batLv[3] = us_batLv[2];
	us_batLv[4] = us_batLv[3];

	/* 最新の値を格納 */
	S12AD.ADANS.BIT.ANS 		= 0x80;		// AN7 変換対象設定
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD変換開始
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD変換待ち
	us_batLv[0] = S12AD.ADDR7;				// AN7 変換データ取得

	/* 電圧平均化 */
	us_BatLvAve = ( us_batLv[0] + us_batLv[1] + us_batLv[2] + us_batLv[3] + us_batLv[4] ) / 5;
	
	/*  残量に応じてLEDを表示  */
	/* ======================= */
	if( us_BatLvAve < BAT_LOW ) {			// 残量やばい！！（赤色）
		LED_BAT0 = OFF;
		LED_BAT1 = ON;
	}
	else if( us_BatLvAve < BAT_GOOD ) {		// 残量減ってきた（黄色）
		LED_BAT0 = ON;
		LED_BAT1 = ON;
	}
	else{									// 残量問題なし（緑色）
		LED_BAT0 = ON;
		LED_BAT1 = OFF;
	}
}


// *************************************************************************
//   機能		： ジャイロセンサのリファレンス値（基準の値）を設定する
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.26			外川			新規
// *************************************************************************/
PUBLIC void GYRO_SetRef( void )
{
	USHORT i;
	ULONG ul_ref = 0;
	
	/* データサンプリング */
	for( i=0; i<GYRO_REF_NUM; i++){			// 100回サンプリングした平均値を基準の値とする。
		ul_ref += (ULONG)s_GyroVal;
		TIME_wait(1);
	}
	
	/* 基準値算出（平均値） */
	l_GyroRef = ul_ref / GYRO_REF_NUM * 100;		// 精度を100倍にする
}


// *************************************************************************
//   機能		： ジャイロの角速度に関する制御偏差を取得する
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.26			外川			新規
// *************************************************************************/
PUBLIC FLOAT GYRO_getSpeedErr( void )
{
	LONG  l_val = (LONG)s_GyroVal * 100 ;				// 100倍の精度にする
	LONG  l_err = l_val - l_GyroRef ;
	FLOAT f_res;
	
	/* 角速度の偏差算出 */
	if( ( l_err < -10 * 100 ) || ( 10 * 100 < l_err ) ){
		f_res = (FLOAT)l_err * 1.2394 / 100;		// 1.2394 = (1 / 4096[div]) * 3300[mV]  / 0.65 [mV/dps]  dps=deg/sec
													// 100倍の精度
	}
	else{
		f_res = 0;									// [deg/s]
	}
	
	return f_res;
}


// *************************************************************************
//   機能		： ジャイロの現在の角度を取得する
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.26			外川			新規
// *************************************************************************/
PUBLIC FLOAT GYRO_getNowAngle( void )
{
	return f_NowAngle;
}


// *************************************************************************
//   機能		： ジャイロセンサ用ポーリング関数
//   注意		： なし
//   メモ		： 割り込みから実行される
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.11.26			外川			新規
// *************************************************************************/
PUBLIC void GYRO_Pol( void )
{
	FLOAT f_speed;
	
	/* バッファシフト（[7]に新しいデータを入れるため、[0]のデータを捨てて、1つずつ詰める） */
	s_GyroValBuf[0]	= s_GyroValBuf[1];
	s_GyroValBuf[1]	= s_GyroValBuf[2];
	s_GyroValBuf[2]	= s_GyroValBuf[3];
	s_GyroValBuf[3]	= s_GyroValBuf[4];
	s_GyroValBuf[4]	= s_GyroValBuf[5];
	s_GyroValBuf[5]	= s_GyroValBuf[6];
	s_GyroValBuf[6]	= s_GyroValBuf[7];
	
	/* 最新のジャイロセンサ値を取得 */
	S12AD.ADANS.BIT.ANS 		= 0x40;		// AN6 変換対象設定
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD変換開始
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD変換待ち
	s_GyroValBuf[7]	= (SHORT)S12AD.ADDR6;
	
	/* ジャイロの値を平滑する（平滑数は8つ） */
	s_GyroVal = ( s_GyroValBuf[0] + s_GyroValBuf[1] + s_GyroValBuf[2] + s_GyroValBuf[3] +
				  s_GyroValBuf[4] + s_GyroValBuf[5] + s_GyroValBuf[6] + s_GyroValBuf[7] ) / 8;
	
	/* 現在の角度を更新する */
	f_speed = GYRO_getSpeedErr();			// 角速度取得 (0.001sec毎の角速度)
	f_NowAngle += f_speed / 1000;			// 角度設定   (0.001sec毎に加算するため)
}


// *************************************************************************
//   機能		： 距離センサの値を取得する
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.02			外川			新規
// *************************************************************************/
PUBLIC SHORT DIST_getNowVal( enDIST_SEN_ID en_id )
{
	return st_sen[en_id].s_now;
}


// *************************************************************************
//   機能		： 距離センサ用（前壁）ポーリング関数
//   注意		： なし
//   メモ		： 割り込みから実行される
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.02			外川			新規
// *************************************************************************/
PUBLIC void DIST_Pol_Front( void )
{
	/* 無発光時の値取得 */
	S12AD.ADANS.BIT.ANS 		= 0x05;		// AN0/2 変換対象設定
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD変換開始
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD変換待ち
	st_sen[DIST_SEN_R_FRONT].s_offset = (SHORT)S12AD.ADDR0;
	st_sen[DIST_SEN_L_FRONT].s_offset = (SHORT)S12AD.ADDR2;

	/* 前壁LED点灯 */
	LED_DIST_RF = ON;
	LED_DIST_LF = ON;
	
	/* 発光安定待ち */
	TIME_waitFree( SEN_WAIT_CNT );

	/* 発光時の値と無発光時の値で差分を取得 */
	S12AD.ADANS.BIT.ANS 		= 0x05;		// AN0/2 変換対象設定
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD変換開始
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD変換待ち
	st_sen[DIST_SEN_R_FRONT].s_old = st_sen[DIST_SEN_R_FRONT].s_now;		// バッファリング
	st_sen[DIST_SEN_L_FRONT].s_old = st_sen[DIST_SEN_L_FRONT].s_now;		// バッファリング
	st_sen[DIST_SEN_R_FRONT].s_now = (SHORT)S12AD.ADDR0 - st_sen[DIST_SEN_R_FRONT].s_offset;		// 現在値書き換え
	st_sen[DIST_SEN_L_FRONT].s_now = (SHORT)S12AD.ADDR2 - st_sen[DIST_SEN_L_FRONT].s_offset;		// 現在値書き換え
	SCI_puts("RightLight:");
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now>>8);
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now&0xff);
	SCI_puts("\n");
	SCI_puts("LeftLight :");
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now>>8);
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now&0xff);
	SCI_puts("\n");
	SCI_putch(0x1b);
	SCI_puts("[2J");
	SCI_putch(0x1b);
	SCI_puts("[0;0H");
	/* 前壁LED消灯 */
	LED_DIST_RF = OFF;
	LED_DIST_LF = OFF;
}


// *************************************************************************
//   機能		： 距離センサ用（横壁）ポーリング関数
//   注意		： なし
//   メモ		： 割り込みから実行される
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.02			外川			新規
// *************************************************************************/
PUBLIC void DIST_Pol_Side( void )
{
	/* 無発光時の値取得 */
	S12AD.ADANS.BIT.ANS 		= 0x0a;		// AN1/3 変換対象設定
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD変換開始
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD変換待ち
	st_sen[DIST_SEN_R_SIDE].s_offset = (SHORT)S12AD.ADDR1;
	st_sen[DIST_SEN_L_SIDE].s_offset = (SHORT)S12AD.ADDR3;

	/* 横壁LED点灯 */
	LED_DIST_RS = ON;
	LED_DIST_LS = ON;
	
	/* 発光安定待ち */
	TIME_waitFree( SEN_WAIT_CNT );

	/* 発光時の値と無発光時の値で差分を取得 */
	S12AD.ADANS.BIT.ANS 		= 0x0a;		// AN1/3 変換対象設定
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD変換開始
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD変換待ち
	st_sen[DIST_SEN_R_SIDE].s_old = st_sen[DIST_SEN_R_SIDE].s_now;		// バッファリング
	st_sen[DIST_SEN_L_SIDE].s_old = st_sen[DIST_SEN_L_SIDE].s_now;		// バッファリング
	st_sen[DIST_SEN_R_SIDE].s_now = (SHORT)S12AD.ADDR1 - st_sen[DIST_SEN_R_SIDE].s_offset;		// 現在値書き換え
	st_sen[DIST_SEN_L_SIDE].s_now = (SHORT)S12AD.ADDR3 - st_sen[DIST_SEN_L_SIDE].s_offset;		// 現在値書き換え
	SCI_puts("RightLight:");
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now>>8);
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now&0xff);
	SCI_puts("\n");
	SCI_puts("LeftLight :");
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now>>8);
	PutNum(st_sen[DIST_SEN_R_FRONT].s_now&0xff);
	SCI_puts("\n");
	SCI_putch(0x1b);
	SCI_puts("[2J");
	SCI_putch(0x1b);
	SCI_puts("[0;0H");
	/* 横壁LED消灯 */
	LED_DIST_RS = OFF;
	LED_DIST_LS = OFF;
}


// *************************************************************************
//   機能		： エンコーダのカウントを開始する
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void ENC_Sta( void )
{
	ENC_R_TSTR = ON;		// カウント開始
	ENC_L_TSTR = ON;		// カウント開始
}


// *************************************************************************
//   機能		： エンコーダのカウントを停止する
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void ENC_Stop( void )
{
	ENC_R_TSTR = OFF;		// カウント停止
	ENC_L_TSTR = OFF;		// カウント停止
}


// *************************************************************************
//   機能		： エンコーダのカウントをクリア
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PRIVATE void ENC_clr( void )
{
	ENC_R_TCNT = ENC_RESET_VAL;
	ENC_L_TCNT = ENC_RESET_VAL;
}


// *************************************************************************
//   機能		： エンコーダのカウント値（パルス数）を取得する
//   注意		： なし
//   メモ		： 中間値からの差分
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void ENC_GetDiv( LONG* p_r, LONG* p_l )
{
	LONG l_cntR = (LONG)ENC_R_TCNT;
	LONG l_cntL = (LONG)ENC_L_TCNT;
	
	ENC_clr();		// カウント値リセット
	
	*p_r = l_cntR - ENC_RESET_VAL;		// 右モータ
	*p_l = ENC_RESET_VAL - l_cntL;		// 左モータ
}


// *************************************************************************
//   機能		： DCMの回転方向をCW（時計回り）にする
//   注意		： なし
//   メモ		： なし
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_setDirCw( enDCM_ID en_id )
{
	/* 回転方向設定 */
	if( en_id == DCM_R ){			// 右
		DCM_R_IN1 = ON;				// BIN1
		DCM_R_IN2 = OFF;			// BIN2
	}
	else{							// 左
		DCM_L_IN1 = OFF;			// AIN1
		DCM_L_IN2 = ON;				// AIN2
	}
}


// *************************************************************************
//   機能		： DCMの回転方向をCCW（反時計回り）にする
//   注意		： なし
//   メモ		： なし
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_setDirCcw( enDCM_ID en_id )
{
	/* 回転方向設定 */
	if( en_id == DCM_R ){			// 右
		DCM_R_IN1 = OFF;			// BIN1
		DCM_R_IN2 = ON;				// BIN2
	}
	else{							// 左
		DCM_L_IN1 = ON;				// AIN1
		DCM_L_IN2 = OFF;			// AIN2
	}
}


// *************************************************************************
//   機能		： DCMを停止する
//   注意		： なし
//   メモ		： PWMのHI出力中に本関数を実行すると、ピンが100%出力状態なるため、関数内でピンをクリア（Lo）する。
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_stopMot( enDCM_ID en_id )
{
	/* 停止設定 */
	if( en_id == DCM_R ){			// 右
		DCM_R_IN1 = OFF;			// BIN1
		DCM_R_IN2 = OFF;			// BIN2
		DCM_R_TIMER = OFF;			// タイマ停止
		DCM_R_TIORA = 1;			// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
		DCM_R_TIORB = 1;			// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	}
	else{							// 左
		DCM_L_IN1 = OFF;			// AIN1
		DCM_L_IN2 = OFF;			// AIN2
		DCM_L_TIMER = OFF;			// タイマ停止
		DCM_L_TIORA = 1;			// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
		DCM_L_TIORB = 1;			// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	}
}


// *************************************************************************
//   機能		： DCMをブレーキングする
//   注意		： なし
//   メモ		： なし
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_brakeMot( enDCM_ID en_id )
{
	/* 停止設定 */
	if( en_id == DCM_R ){			// 右
		DCM_R_IN1 = ON;				// BIN1
		DCM_R_IN2 = ON;				// BIN2
		DCM_R_TIMER = OFF;			// タイマ停止
		DCM_R_TIORA = 1;			// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
		DCM_R_TIORB = 1;			// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	}
	else{							// 左
		DCM_L_IN1 = ON;				// AIN1
		DCM_L_IN2 = ON;				// AIN2
		DCM_L_TIMER = OFF;			// タイマ停止
		DCM_L_TIORA = 1;			// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	    DCM_L_TIORB = 1;			// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	}
}


// *************************************************************************
//   機能		： DCMを動作開始する
//   注意		： なし
//   メモ		： 動作開始前にPWMと回転方向を指定しておくこと
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_staMot( enDCM_ID en_id )
{
	/* タイマスタート */
	if( en_id == DCM_R ){			// 右
		DCM_R_TIORA = 2;			// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 1 出力
		DCM_R_TIORB = 1;			// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
		DCM_R_TIMER = ON;			// タイマ開始
	}
	else{							// 左
		DCM_L_TIORA = 2;			// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 1 出力
	    DCM_L_TIORB = 1;			// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
		DCM_L_TIMER = ON;			// タイマ開始
	}
}


// *************************************************************************
//   機能		： 全DCMを動作開始する
//   注意		： なし
//   メモ		： 動作開始前にPWMと回転方向を指定しておくこと
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_staMotAll( void )
{
	DCM_staMot(DCM_R);									// 右モータON
	DCM_staMot(DCM_L);									// 左モータON
}


// *************************************************************************
//   機能		： DCMのPWM-Dutyを設定する
//   注意		： 割り込み外から設定すると、ダブルバッファでないと歯抜けになる場合がある。
//   メモ		： 割り込みハンドラから実行すること。Duty0%の場合モータを停止させる（PWMにひげが出る）
//   引数		： モータID
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.12.03			外川			新規
// *************************************************************************/
PUBLIC void DCM_setPwmDuty( enDCM_ID en_id, USHORT us_duty10 )
{
	USHORT	us_cycle;							// 周期
	USHORT	us_onReg;							// 設定するON-duty
	
	/* PWM設定 */
	if( en_id == DCM_R ){				// 右
	
		if( 0 == us_duty10 ){			// Duty0%設定
			DCM_brakeMot( en_id );
		}
		else if( 1000 <= us_duty10 ){	// Duty100%

			DCM_R_TIMER = OFF;			// タイマ停止
			DCM_R_TCNT = 0;				// TCNT カウンタをクリア
			DCM_R_GRB = 5000;			// タイマ値変更
			DCM_R_TIORA = 6;			// TIOCA 端子の機能 : 初期出力は 1 出力。コンペアマッチで 1 出力
			DCM_R_TIORB = 6;			// TIOCB 端子の機能 : 初期出力は 1 出力。コンペアマッチで 1 出力
			DCM_R_TIMER = ON;			// タイマ開始
			us_duty10 = 1000;
		}
		else{
			us_cycle = DCM_R_GRA;		// 周期
			us_onReg = (USHORT)( (ULONG)us_cycle * (ULONG)us_duty10 / (ULONG)1000 );	// Duty2Reg 計算式
			DCM_R_TIMER = OFF;			// タイマ停止
			DCM_R_TCNT = 0;				// TCNT カウンタをクリア
			DCM_R_GRB = us_onReg;		// onDuty
			DCM_staMot( en_id );		// 回転開始
		}
	}
	else{								// 左

		if( 0 == us_duty10 ){			// Duty0%
			DCM_brakeMot( en_id );
		}
		else if( 1000 <= us_duty10 ){	// Duty100%

			DCM_L_TIMER = OFF;			// タイマ停止
			DCM_L_TCNT = 0;				// TCNT カウンタをクリア
			DCM_L_GRB = 5000;			// タイマ値変更
			DCM_L_TIORA = 6;			// TIOCA 端子の機能 : 初期出力は 1 出力。コンペアマッチで 1 出力
			DCM_L_TIORB = 6;			// TIOCB 端子の機能 : 初期出力は 1 出力。コンペアマッチで 1 出力
			DCM_L_TIMER = ON;			// タイマ開始
			us_duty10 = 1000;
		}
		else{
			us_cycle = DCM_L_GRA;		// 周期
			us_onReg = (USHORT)( (ULONG)us_cycle * (ULONG)us_duty10 / (ULONG)1000 );	// Duty2Reg 計算式
			DCM_L_TIMER = OFF;			// タイマ停止
			DCM_L_TCNT = 0;				// TCNT カウンタをクリア
			DCM_L_GRB = us_onReg;		// タイマ値変更
			DCM_staMot( en_id );		// 回転開始
		}
	}
}


