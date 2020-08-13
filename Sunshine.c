/**
 * @file 	Sunshine.c
 * @brief 	サンシャインのメインファイル
 * @author 	外川，fucchi, TKR
 * @date 	2020.08.13
 * 
 * @version		v1.0		2013.10.31			外川			新規（ファイルのインクルード）
 * @version		v1.1		2013.11.04			外川			タイマー割り込みの初期化追加
 * @version		v1.2		2013.11.05			外川			内部時計追加
 * @version		v1.3		2013.11.05			外川			ソフトウエアウエイト追加
 * @version		v1.4		2013.11.05			外川			LED点滅処理追加
 * @version		v1.5		2013.11.13			外川			モード追加
 * @version		v1.6		2013.11.13			外川			シリアル通信＋printf追加
 * @version		v1.7		2013.11.17			外川			電源監視追加
 * @version		v1.8		2013.11.22			外川			ジャイロセンサ追加
 * @version		v1.9		2013.12.01			外川			距離センサ追加
 * @version		v2.0		2013.12.03			外川			エンコーダ追加
 * @version		v2.1		2013.12.04			外川			DCM追加
 * @version		v2.1.1		2020.03.25			fucchi			GitHub(maste)スタート
 * @version		v2.2		2020.08.13			TKR				Doxygen対応
 */


//**************************************************
// プロジェクト作成時に生成されたコード （使わないけどとりあえず残しておく）
//**************************************************
 #ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif


//**************************************************
// インクルードファイル（include）
//**************************************************
#include <typedefine.h>						// 定義
#include <iodefine.h>						// I/O
#include <stdio.h>							// 標準入出力
#include <hal.h>							// HAL


//**************************************************
// 定義（define）
//**************************************************
#define		SW_CHATTERING_WAIT			(200)			// スイッチのチャタリング対策用のカウンタ、200[msec]


//**************************************************
// 列挙体（enum）
//**************************************************
/* 動作モード */
typedef enum{
	MODE_0 = 0,					// モード0 ： モードの説明を記載する
	MODE_1,						// モード1 ： モードの説明を記載する
	MODE_2,						// モード2 ： モードの説明を記載する
	MODE_3,						// モード3 ： モードの説明を記載する
	MODE_4,						// モード4 ： モードの説明を記載する
	MODE_5,						// モード5 ： モードの説明を記載する
	MODE_6,						// モード6 ： モードの説明を記載する
	MODE_7,						// モード7 ： モードの説明を記載する
	MODE_8,						// モード8 ： モードの説明を記載する
	MODE_MAX
}enMODE;


//**************************************************
// 構造体（struct）
//**************************************************


//**************************************************
// グローバル変数
//**************************************************
PRIVATE	VUSHORT		uc_Msec;	// 内部時計[msec]
PRIVATE	VUCHAR		uc_Sec;		// 内部時計[sec]
PRIVATE	VUCHAR		uc_Min;		// 内部時計[min]
PRIVATE VULONG		ul_Wait;	// 1msecのwaitに使用する カウンタ[msec]
PRIVATE enMODE		en_Mode;	// 現在のモード
//PRIVATE stDIST_SEN	st_sen[DIST_SEN_NUM];		

//**************************************************
// プロトタイプ宣言（ファイル内で必要なものだけ記述）
//**************************************************

/**
 * @fn		INTC_sys		
 * @brief	MTU0のTGRA割り込み、1msec毎に関数が実行される。
 * @brief	正常にマイコンが動作している場合、1sec周期でLED_SYSが点滅する。
 * @param	なし
 * @return	なし
 * @author 	外川
 * @version	v1.0		2013.11.04			外川			新規
 * @version v1.1		2013.11.05			外川			内部時計追加
 * @version	v1.2		2013.11.05			外川			ソフトウエアウエイト追加
 * @version	v1.3		2013.11.05			外川			システム用LED動作追加	
 */
PUBLIC void INTC_sys( void )
{
	static USHORT	i = 0;
	
	/* システム動作確認用LED  */
	if( i == 500 ){
		LED_SYS = ~LED_SYS;		// LEDをON⇔OFFさせる
		i=0;
	}
	else{
		i++;
	}

	/* ---------- */
	/*  内部時計  */
	/* ---------- */
	uc_Msec++;					// msec
	if( uc_Msec > 999 ){		// msec → sec
		uc_Msec  = 0;
		uc_Sec++;
	}
	if( uc_Sec > 59 ){			// sec → min
		uc_Sec = 0;
		uc_Min++;
	}

	/* ----------------------- */
	/*  S/Wウェイト・カウンタ  */
	/* ----------------------- */
	ul_Wait++;
	ul_Wait %= 6000000;			// 10 min (= 6000000 カウント) で 0 クリア
	
}

/**
 * @fn		INTC_sen
 * @brief	MTU4のTGRA割り込み、0.25msec毎に関数が実行される。
 * @brief	正常にマイコンが動作している場合、250usec周期で
 * @brief	距離センサ0 → 距離センサ1 → 距離センサ2 → ジャイロセンサ の順でスキャンする。
 * @brief	従って、1センサのスキャン周期は1msecとなる。
 * @param	なし
 * @return	なし
 * @author 	外川
 * @version	v1.0		2013.11.04			外川			新規
 */
PUBLIC void INTC_sen( void )
{
	static UCHAR	i = 0;
	
	/* センサ処理  */
	switch( i ){
		case 0:		// ジャイロセンサ
			GYRO_Pol();			
			break;
		
		case 1:		// 前壁センサ
			DIST_Pol_Front();
			break;
		
		case 2:		// 横壁センサ
			DIST_Pol_Side();
			break;
		
		case 3:		// 斜め壁センサ
			// 斜めセンサは使用しない
			break;
		
	}
	
	i = ( i + 1 ) % 4;			// roop
	
	return;
}

 /**
 * @fn		CPU_init		
 * @brief	マイコンのレジスタを初期化する	 	
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.10.31			外川			新規
 * @version	v1.1		2013.11.11			外川			スイッチ追加
 * @version v1.2		2013.11.13			外川			シリアル通信追加
 */
PRIVATE void CPU_init(void)
{
	int i;		// loop用
	
	/* ============== */
	/*  クロック設定  */
	/* ============== */
	SYSTEM.SCKCR.BIT.ICK 		= 0;		// システムクロック(ICLK)		φ×8 (96MHz)
	SYSTEM.SCKCR.BIT.PCK 		= 0;		// 周辺モジュールクロック(PCLK)	φ×8 (96MHz)

	/* ================== */
	/*  GPIO(汎用入出力)  */
	/* ================== */
	/* 出力値 */
	PORT3.DR.BIT.B2				= 0;		// ポート3-2の初期出力0[V]	(正常動作確認用LED)
	PORT5.DR.BIT.B1				= 0;		// ポート5-1の初期出力0[V]	(バッテリ用LED)
	PORT5.DR.BIT.B2				= 0;		// ポート5-2の初期出力0[V]	(バッテリ用LED)
	PORTA.DR.BIT.B0				= 0;		// ポートA-0の初期出力0[V]	(右モータPWM出力)
	PORTB.DR.BIT.B0				= 0;		// ポートB-0の初期出力0[V]	(左モータPWM出力)
	PORTB.DR.BIT.B3				= 0;		// ポートB-3の初期出力0[V]	(右モータINA出力)
	PORTB.DR.BIT.B4				= 0;		// ポートB-4の初期出力0[V]	(右モータINA出力)
	PORTB.DR.BIT.B5				= 0;		// ポートB-5の初期出力0[V]	(右モータINB出力)
	PORTB.DR.BIT.B6				= 0;		// ポートB-6の初期出力0[V]	(左モータINA出力)
	PORTB.DR.BIT.B7				= 0;		// ポートB-7の初期出力0[V]	(左モータINB出力)
	PORTD.DR.BYTE				= 0x00;		// ポートDの初期出力0[V]	(デバッグ用LED)
	PORTE.DR.BYTE				= 0x00;		// ポートEの初期出力0[V]	(センサLED)
	
	/* 入出力設定 */
	PORT3.DDR.BIT.B2			= 1;		// ポート3-2を出力に設定	(正常動作確認用LED)
	PORT5.DDR.BIT.B1			= 1;		// ポート5-1を出力に設定	(バッテリ用LED)
	PORT5.DDR.BIT.B2			= 1;		// ポート5-2を出力に設定	(バッテリ用LED)
	PORTA.DDR.BIT.B0			= 1;		// ポートA-0を出力に設定	(右モータPWM出力)
	PORTB.DDR.BIT.B0			= 1;		// ポートB-0を出力に設定	(左モータPWM出力)
	PORTB.DDR.BIT.B3			= 1;		// ポートB-3を出力に設定	(モータENA出力)
	PORTB.DDR.BIT.B4			= 1;		// ポートB-4を出力に設定	(右モータINA出力)
	PORTB.DDR.BIT.B5			= 1;		// ポートB-5を出力に設定	(右モータINB出力)
	PORTB.DDR.BIT.B6			= 1;		// ポートB-6を出力に設定	(左モータINA出力)
	PORTB.DDR.BIT.B7			= 1;		// ポートB-7を出力に設定	(左モータINB出力)
	PORTD.DDR.BYTE				= 0xff;		// ポートDを出力に設定		(デバッグ用LED)
	PORTE.DDR.BYTE				= 0x0f;		// ポートEを出力に設定		(センサLED)

	/* 入力プルアップ設定 */
	PORTC.PCR.BIT.B6			= 1;		// ポートC-6はプルアップを使用	(プッシュスイッチ用)
	PORTC.PCR.BIT.B7			= 1;		// ポートC-7はプルアップを使用	(プッシュスイッチ用)
	
	/* 入力バッファコントロールレジスタ設定（入力ポートと内部機能を接続するためにある） */
	PORTC.ICR.BIT.B6			= 1;		// ポートC-6を入力ポートと接続する
	PORTC.ICR.BIT.B7			= 1;		// ポートC-7を入力ポートと接続する
	PORT3.ICR.BIT.B0			= 1;		// RXD1とP3-1を接続する
	PORTC.ICR.BIT.B0			= 1;		// MTCLKGとPC-0を接続する
	PORTC.ICR.BIT.B1			= 1;		// MTCLKHとPC-1を接続する
	PORTC.ICR.BIT.B2			= 1;		// MTCLKEとPC-2を接続する
	PORTC.ICR.BIT.B3			= 1;		// MTCLKFとPC-3を接続する


	/* ======== */
	/*  タイマ  */
	/* ======== */
	/* 消費電力低減機能の設定 */
	SYSTEM.MSTPCRA.BIT.MSTPA8 	= 0;		// MTU-unit1(MTU0〜5 )のスタンバイ解除
	SYSTEM.MSTPCRA.BIT.MSTPA9 	= 0;		// MTU-unit2(MTU6〜11)のスタンバイ解除
	
	// -----------------------
	//  システム用(MTU0)
	// -----------------------
	/* 割り込みコントローラの設定 */
	ICU.IER[0xE].BIT.IEN2 		= 1;		// MTU0-TGRAの割り込みの許可
	ICU.IPR[0x51].BIT.IPR 		= 7;		// MTU0-TGRAの割り込みレベルの設定

	/* タイマ割り込みの設定 */
	MTU0.TCR.BIT.CCLR 			= 1;		// TGRAのコンペアマッチでTCNTクリア
	MTU0.TCR.BIT.TPSC 			= 3;		// PCLK(96MHz)/64 で1カウント
	MTU0.TIER.BIT.TGIEA			= 1;		// TGRAとのコンペアマッチで割り込み許可
	MTU0.TGRA 				= 375 * 4;	// 1msec毎に割り込み
	MTU0.TCNT 				= 0;		// タイマクリア
	

	// -----------------------
	//  センサ用(MTU4)
	// -----------------------
	/* 割り込みコントローラの設定 */
	ICU.IER[0x10].BIT.IEN6 			= 1;		// MTU4-TGRAの割り込みの許可
	ICU.IPR[0x59].BIT.IPR 			= 8;		// MTU4-TGRAの割り込みレベルの設定

	/* タイマ割り込みの設定 */
	MTU4.TCR.BIT.CCLR 			= 1;		// TGRAのコンペアマッチでTCNTクリア
	MTU4.TCR.BIT.TPSC 			= 3;		// PCLK(96MHz)/64 で1カウント
	MTU4.TIER.BIT.TGIEA			= 1;		// TGRAとのコンペアマッチで割り込み許可
	MTU4.TGRA 				= 94 * 4;	// 250msec毎に割り込み
	MTU4.TCNT 				= 0;		// タイマクリア


	// --------------------------
	//  右モータPWM出力用(MTU6)
	// --------------------------
	MTU6.TCR.BIT.CCLR 			= 1;		// TGRAのコンペアマッチでTCNTクリア
	MTU6.TCR.BIT.TPSC 			= 1;		// PCLK(96MHz)/4 で1カウント
	MTU6.TMDR.BIT.MD			= 2;		// PWM モード 1
	MTU6.TIORH.BIT.IOA			= 2;		// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 1 出力
	MTU6.TIORH.BIT.IOB			= 1;		// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	MTU6.TGRA 				= 1200;		// 周期(100usec)
	MTU6.TGRB 				= 600;		// onDuty
	MTU6.TCNT 				= 0;		// タイマクリア

	// --------------------------
	//  右モータ位相計測用(MTU7)
	// --------------------------
	MTU7.TCR.BIT.CCLR 			= 0;		// コンペアマッチでTCNTクリア禁止
	MTU7.TIER.BIT.TGIEA 			= 0;		// TGRA とのコンペアマッチで割り込み未発生
	MTU7.TMDR.BIT.MD			= 7;		// 位相計測モード 4
	MTU7.TGRA 				= 0;		// 始めは未設定
	MTU7.TCNT 				= 32768;	// 初期化
	
	// --------------------------
	//  左モータ位相計測用(MTU8)
	// --------------------------
	MTU8.TCR.BIT.CCLR 			= 0;		// コンペアマッチでTCNTクリア禁止
	MTU8.TIER.BIT.TGIEA 			= 0;		// TGRA とのコンペアマッチで割り込み未発生
	MTU8.TMDR.BIT.MD			= 7;		// 位相計測モード 4
	MTU8.TGRA 				= 0;		// 始めは未設定
	MTU8.TCNT 				= 32768;	// 初期化
	
	// --------------------------
	//  左モータPWM出力用(MTU9)
	// --------------------------
	MTU9.TCR.BIT.CCLR 			= 1;		// TGRAのコンペアマッチでTCNTクリア
	MTU9.TCR.BIT.TPSC 			= 1;		// PCLK(96MHz)/4 で1カウント
	MTU9.TMDR.BIT.MD			= 2;		// PWM モード 1
	MTU9.TIORH.BIT.IOA			= 2;		// TIOCA 端子の機能 : 初期出力は 0 出力。コンペアマッチで 1 出力
	MTU9.TIORH.BIT.IOB			= 1;		// TIOCB 端子の機能 : 初期出力は 0 出力。コンペアマッチで 0 出力
	MTU9.TGRA 				= 1200;		// 周期(50usec)
	MTU9.TGRB 				= 500;		// onDuty
	MTU9.TCNT 				= 0;		// タイマクリア

	// -----------------------
	//  バッテリ監視用(MTU10)
	// -----------------------
	/* 割り込みコントローラの設定 */
	ICU.IER[0x14].BIT.IEN2 			= 1;		// MTU10-TGRAの割り込みの許可
	ICU.IPR[0x64].BIT.IPR 			= 6;		// MTU10-TGRAの割り込みレベルの設定

	/* タイマ割り込みの設定 */
	MTU10.TCR.BIT.CCLR 			= 1;		// TGRAのコンペアマッチでTCNTクリア
	MTU10.TCR.BIT.TPSC 			= 5;		// PCLK(96MHz)/1024 で1カウント
	MTU10.TIER.BIT.TGIEA			= 1;		// TGRAとのコンペアマッチで割り込み許可
	MTU10.TGRA 				= 234 * 4;	// 10msec毎に割り込み
	MTU10.TCNT 				= 0;		// タイマクリア
	
	
	/* ===== */
	/*  SCI  */
	/* ===== */
	SYSTEM.MSTPCRB.BIT.MSTPB30		= 0;		// SCI1スタンバイ解除
	SCI1.SCR.BYTE				= 0x00;		// TE=0、RE=0、CKE1=0
	SCI1.SMR.BYTE				= 0x00;		// CHR=0、PE=0、STOP=0、CKS=00(Pφ/1)
	SCI1.BRR				= 100;		// 57600[bps](PCLK(96MHz)
	SCI1.SEMR.BIT.ABCS			= 1;		// ABCS=1
	for( i=0; i<0x800000; i++ );		// 1bit時間wait
	SCI1.SCR.BYTE				= 0x70;		// TIE=0、RIE=1、TE=1、RE=1
	/* 割り込みコントローラの設定 */
	ICU.IER[0x1B].BIT.IEN3 			= 1;		// SCI1-RXI1の割り込みの許可
	ICU.IPR[0x81].BIT.IPR 			= 5;		// SCI1の割り込みレベルの設定
	

	/* ========= */
	/*  A/D変換  */
	/* ========= */
	SYSTEM.MSTPCRA.BIT.MSTPA17 		= 0;		// AD(12bit)スタンバイ解除
	S12AD.ADCSR.BIT.CKS 			= 3;		// PCLKで変換
}


 /**
 * @fn		RX621_staTimer
 * @brief 	マイコンのタイマーを開始する
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.04			外川			新規
 */
PRIVATE void RX621_staTimer(void)
{
	MTUA.TSTR.BIT.CST0			= 1;		// MTU0カウント開始			システム
	MTUA.TSTR.BIT.CST4			= 1;		// MTU4カウント開始			センサ
	MTUB.TSTR.BIT.CST1			= 1;		// MTU7カウント開始			位相計測
	MTUB.TSTR.BIT.CST2			= 1;		// MTU8カウント開始			位相計測
	MTUB.TSTR.BIT.CST4			= 1;		// MTU10カウント開始		バッテリ監視
}


 /**
 * @fn		TIME_init
 * @brief 	時間制御の初期化．起動後に1回だけ実行する関数．
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.05			外川			新規
 * @version	v1.1		2013.11.05			外川			ソフトウエアウエイト追加
 */
PUBLIC void TIME_init( void )
{
	/* 初期化 */
	uc_Msec = 0;		// 内部時計[msec]
	uc_Sec  = 0;		// 内部時計[sec]
	uc_Min  = 0;		// 内部時計[min]
	ul_Wait = 0;		// 1msecのwaitに使用する カウンタ[msec]
}


 /**
 * @fn		TIME_wait( ULONG ul_time )
 * @brief 	指定した ms 間、S/Wウェイトする
 * @param	ul_time	待つ時間 [msec]，最大　600000(=10min)
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.05			外川			新規
 */
PUBLIC void TIME_wait( ULONG ul_time )
{
	ul_Wait = 0;						// 0からカウントする、"ul_Wait"は1msec毎に1加算される。

	while( ul_Wait < ul_time );			// 指定時間経過するまでウェイト
	
	return;
}


 /**
 * @fn		TIME_waitFree( ULONG ul_cnt )
 * @brief 	指定したフリーカウント間，S/Wウェイトする
 * @param	ul_cnt	カウント値
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.05			外川			新規
 */
PUBLIC void TIME_waitFree( ULONG ul_cnt )
{
	while( ul_cnt-- );			// 0になるまでディクリメント
}


 /**
 * @fn		MODE_inc
 * @brief 	モードを加算変更する
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.13			外川			新規
 */
PRIVATE void MODE_inc( void )
{
	en_Mode++;		// モードを加算
	
	/* 最大値チェック */
	if( MODE_MAX == en_Mode ){
		en_Mode = MODE_0;
	}
	
	/* モード表示 */
	switch( en_Mode ){
	
		case MODE_0:
			LED8 = 0x00;
			break;

		case MODE_1:
			LED8 = 0x01;
			break;

		case MODE_2:
			LED8 = 0x02;
			break;

		case MODE_3:
			LED8 = 0x04;
			break;

		case MODE_4:
			LED8 = 0x08;
			break;

		case MODE_5:
			LED8 = 0x10;
			break;

		case MODE_6:
			LED8 = 0x20;
			break;

		case MODE_7:
			LED8 = 0x40;
			break;

		case MODE_8:
			LED8 = 0x80;
			break;

		default:
			break;
	}
}


 /**
 * @fn		MODE_exe
 * @brief 	モードを実行する．各モードの処理をここに記述する．
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.13			外川			新規
 */
PRIVATE void MODE_exe( void )
{
	/* モード表示 */
	switch( en_Mode ){
	
		case MODE_0:
			LED8 = LED8_ALL_ON;
			
			while(1){
			
				printf("[電源電圧] %5.2f[mV] \n\r", BAT_getLv() );
				TIME_wait( 500 );
			}
			break;

		case MODE_1:
			LED8 = LED8_ALL_ON;

			while(1){

				printf("   [ジャイロ角度]%5.2f [AD]%d \r", 
					1.2394f * GYRO_getNowAngle(), (USHORT)GYRO_getNowAngle()			// 1.2394 = (1 / 4096[div]) * 3300[mV]  / 0.65 [mV/dps]      dps=deg/sec
				);
				TIME_wait( 50 );
			}
			break;

		case MODE_2:
			LED8 = LED8_ALL_ON;

			while(1){

				printf("   距離センサ [R_F]%5d [L_F]%5d [R_S]%5d [L_S]%5d \r", 
					(int)DIST_getNowVal(DIST_SEN_R_FRONT),
					(int)DIST_getNowVal(DIST_SEN_L_FRONT),
					(int)DIST_getNowVal(DIST_SEN_R_SIDE),
					(int)DIST_getNowVal(DIST_SEN_L_SIDE)
				);
				TIME_wait( 50 );
			}
			break;

		case MODE_3:
			LED8 = LED8_ALL_ON;

			while(1){

				printf("エンコーダ [R]=%d [L]=%d \r", ENC_R_TCNT, ENC_L_TCNT );
				TIME_wait( 50 );
			}
			break;

		case MODE_4:
			LED8 = LED8_ALL_ON;

			/* モータ駆動 */
			DCM_setDirCw( DCM_R );				// 時計回り
			DCM_setDirCcw( DCM_L );				// 半時計回り
			DCM_setPwmDuty( DCM_R, 180 );		// PWM設定、Duty20%
			DCM_setPwmDuty( DCM_L, 180 );		// PWM設定、Duty20%
			DCM_staMotAll();					// モータ動作開始
			break;

		case MODE_5:
			LED8 = LED8_ALL_ON;

			/* モータ停止 */
			DCM_stopMot( DCM_R );				// モータ停止
			DCM_stopMot( DCM_L );				// モータ停止
			break;

		case MODE_6:
			LED8 = LED8_ALL_ON;
			break;

		case MODE_7:
			LED8 = LED8_ALL_ON;
			break;

		case MODE_8:
			LED8 = LED8_ALL_ON;
			break;

		default:
			break;
	}
}


 /**
 * @fn		SYS_start
 * @brief 	動作を開始する
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.11.25			外川			新規
 */
PRIVATE void SYS_start( void )
{
	UCHAR	i;

	/* LED上位⇔下位4bit点滅 */
	for( i = 0; i < 5; i++ ){
	
		LED8 = 0x0f;
		TIME_wait(50);
		LED8 = 0xf0;
		TIME_wait(50);
	}
	LED8 = LED8_ALL_OFF;		// 全消灯
	
	/* タイトル表示 */
	printf(" ┏━━━━━━━━━━━━━━━━━━━━━┓\r\n");
	printf(" ┃ Robo Name  : Sunshine                    ┃\r\n");
	printf(" ┃ Developer  : ここに開発者の名前          ┃\r\n");
	printf(" ┃ Version    : Protype1号機                ┃\r\n");
	printf(" ┃ Target     : 教育用                      ┃\r\n");
	printf(" ┃ Project By : Hosei Univ. Denken Group    ┃\r\n");
	printf(" ┗━━━━━━━━━━━━━━━━━━━━━┛\r\n");
	
}

	

 /**
 * @fn		main		
 * @brief 	main処理．処理を終了させないように注意する．
 * @param	なし
 * @return	なし
 * @author	外川
 * @version	v1.0		2013.10.31			外川			新規
 * @version v1.1		2013.11.05			外川			タイマー割り込みの初期化追加
 * @version v1.2		2013.11.05			外川			タイマーモジュールの初期化追加
 * @version v1.3		2013.11.05			外川			LED動作追加
 * @version v1.4		2013.11.27			外川			動作開始用関数作成
 */
void main(void)
{
	/* 初期化 */
	CPU_init();							// [CPU] レジスタ/GPIO/AD/TIMERなど
	TIME_init();						// [SYS] タイマー
	HAL_init();							// [HAL] ハードウエアドライバ
	RX621_staTimer();					// [CPU] タイマスタート
	SYS_start();						// [SYS] 動作開始
		
	LED8 = LED8_ALL_ON;

	/* モータ駆動 */
	DCM_setDirCw( DCM_R );				// 時計回り
	DCM_setDirCcw( DCM_L );				// 半時計回り
	DCM_setPwmDuty( DCM_R, 180 );		// PWM設定、Duty20%
	DCM_setPwmDuty( DCM_L, 180 );		// PWM設定、Duty20%
	DCM_staMotAll();					// モータ動作開始
	LED8 = LED8_ALL_OFF;

	/* big roop */
	while(1){
		if ( SW_ON == SW_INC_PIN ){
			MODE_inc();								// モードを1つ進める
			TIME_wait(SW_CHATTERING_WAIT);			// SWが離されるまで待つ
		}
		else if ( SW_ON == SW_EXE_PIN ){
			MODE_exe();								// モード実行
			TIME_wait(SW_CHATTERING_WAIT);			// SWが離されるまで待つ
		}
	}
}








//**************************************************
// プロジェクト作成時に生成されたコード （使わないけどとりあえず残しておく）
//**************************************************
#ifdef __cplusplus
void abort(void)
{

}

#endif

