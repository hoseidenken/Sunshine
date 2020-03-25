// *************************************************************************
//   ロボット名	： 電研ベーシックDCマウス、サンシャイン
//   概要		： サンシャインのメインファイル
//   注意		： なし
//   メモ		： なし
//   引数		： なし
//   返り値		： なし
// **************************    履    歴    *******************************
// 		v1.0		2013.10.31			外川			新規（ファイルのインクルード）
// *************************************************************************/
// 多重コンパイル抑止
#ifndef _HAL_H
#define _HAL_H

//**************************************************
// インクルードファイル（include）
//**************************************************
#include <typedefine.h>						// 定義
#include <iodefine.h>						// I/O


//**************************************************
// 定義（define）
//**************************************************
/* LED */
#define		LED_SYS			( PORT3.DR.BIT.B2 )			// ビットアクセス
#define		LED8			( PORTD.DR.BYTE )			// バイトアクセス
#define		LED8_ALL_OFF	( 0x00 )					// 全消灯
#define		LED8_ALL_ON		( 0xff )					// 全点灯

/* スイッチ */
#define		SW_INC_PIN		(PORTC.PORT.BIT.B6)			// モード切り替え
#define		SW_EXE_PIN		(PORTC.PORT.BIT.B7)			// モード実行
#define		SW_ON			(OFF)						// Pull-Upのため、ON-OFFが論理反転さている
#define		SW_OFF			(ON)						// Pull-Upのため、ON-OFFが論理反転さている

/* 電圧監視 */
#define		LED_BAT0		( PORT5.DR.BIT.B1 )			// ビットアクセス
#define		LED_BAT1		( PORT5.DR.BIT.B2 )			// ビットアクセス

/* 距離センサ */
#define		LED_DIST_RF		( PORTE.DR.BIT.B0 )			// ビットアクセス
#define		LED_DIST_RS		( PORTE.DR.BIT.B1 )			// ビットアクセス
#define		LED_DIST_LF		( PORTE.DR.BIT.B2 )			// ビットアクセス
#define		LED_DIST_LS		( PORTE.DR.BIT.B3 )			// ビットアクセス

/* エンコーダ */
#define		ENC_R_TCNT			(MTU7.TCNT)		// 右エンコーダパルス数
#define		ENC_L_TCNT			(MTU8.TCNT)		// 左エンコーダパルス数


//**************************************************
// 列挙体（enum）
//**************************************************
/* 距離センサID  */
typedef enum{
	DIST_SEN_R_FRONT = 0,		// 右前
	DIST_SEN_L_FRONT,			// 左前
	DIST_SEN_R_SIDE,			// 右横
	DIST_SEN_L_SIDE,			// 左横
	DIST_SEN_NUM
}enDIST_SEN_ID;

/* モータID */
typedef enum{
	DCM_R = 0,					// 右モータ
	DCM_L,						// 左モータ
	DCM_MAX
}enDCM_ID;


//**************************************************
// 構造体（struct）
//**************************************************


//**************************************************
// グローバル変数
//**************************************************


//**************************************************
// プロトタイプ宣言（ファイル内で必要なものだけ記述）
//**************************************************
/* 共通 */
PUBLIC void HAL_init( void );

/* シリアル通信 */
PUBLIC void SCI_putch(char data);
PUBLIC void SCI_puts(char *buffer);
PUBLIC void SCI_putsL(char *buffer, int len);
PUBLIC void charput(unsigned char data);
PUBLIC int SCI_chkRecv(void);
PUBLIC char SCI_getch(void);
PUBLIC unsigned char SCI_getch_uc(void);
PUBLIC int SCI_gets(char *buffer);
PUBLIC unsigned char charget(void);

/* 電圧監視 */
PUBLIC void BAT_Pol( void );
PUBLIC FLOAT BAT_getLv(void);

/* ジャイロセンサ */
PUBLIC void GYRO_Pol( void );
PUBLIC void GYRO_SetRef( void );
PUBLIC FLOAT GYRO_getSpeedErr( void );
PUBLIC FLOAT GYRO_getNowAngle( void );

/* 距離センサ */
PUBLIC void DIST_Pol_Front( void );
PUBLIC void DIST_Pol_Side( void );
PUBLIC SHORT DIST_getNowVal( enDIST_SEN_ID en_id );

/* エンコーダ */
PUBLIC void ENC_Sta( void );
PUBLIC void ENC_Stop( void );
PUBLIC void ENC_Clr( void );
PUBLIC void ENC_GetDiv( LONG* p_r, LONG* p_l );

/* DCM */
PUBLIC void DCM_setDirCw( enDCM_ID en_id );
PUBLIC void DCM_setDirCcw( enDCM_ID en_id );
PUBLIC void DCM_stopMot( enDCM_ID en_id );
PUBLIC void DCM_brakeMot( enDCM_ID en_id );
PUBLIC void DCM_staMot( enDCM_ID en_id );
PUBLIC void DCM_staMotAll( void );
PUBLIC void DCM_setPwmDuty( enDCM_ID en_id, USHORT us_duty10 );


#endif	// _HAL_H

