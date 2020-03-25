// *************************************************************************
//   ���{�b�g��	�F �d���x�[�V�b�NDC�}�E�X�A�T���V���C��
//   �T�v		�F �T���V���C���̃��C���t�@�C��
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.10.31			�O��			�V�K�i�t�@�C���̃C���N���[�h�j
// *************************************************************************/
// ���d�R���p�C���}�~
#ifndef _HAL_H
#define _HAL_H

//**************************************************
// �C���N���[�h�t�@�C���iinclude�j
//**************************************************
#include <typedefine.h>						// ��`
#include <iodefine.h>						// I/O


//**************************************************
// ��`�idefine�j
//**************************************************
/* LED */
#define		LED_SYS			( PORT3.DR.BIT.B2 )			// �r�b�g�A�N�Z�X
#define		LED8			( PORTD.DR.BYTE )			// �o�C�g�A�N�Z�X
#define		LED8_ALL_OFF	( 0x00 )					// �S����
#define		LED8_ALL_ON		( 0xff )					// �S�_��

/* �X�C�b�` */
#define		SW_INC_PIN		(PORTC.PORT.BIT.B6)			// ���[�h�؂�ւ�
#define		SW_EXE_PIN		(PORTC.PORT.BIT.B7)			// ���[�h���s
#define		SW_ON			(OFF)						// Pull-Up�̂��߁AON-OFF���_�����]���Ă���
#define		SW_OFF			(ON)						// Pull-Up�̂��߁AON-OFF���_�����]���Ă���

/* �d���Ď� */
#define		LED_BAT0		( PORT5.DR.BIT.B1 )			// �r�b�g�A�N�Z�X
#define		LED_BAT1		( PORT5.DR.BIT.B2 )			// �r�b�g�A�N�Z�X

/* �����Z���T */
#define		LED_DIST_RF		( PORTE.DR.BIT.B0 )			// �r�b�g�A�N�Z�X
#define		LED_DIST_RS		( PORTE.DR.BIT.B1 )			// �r�b�g�A�N�Z�X
#define		LED_DIST_LF		( PORTE.DR.BIT.B2 )			// �r�b�g�A�N�Z�X
#define		LED_DIST_LS		( PORTE.DR.BIT.B3 )			// �r�b�g�A�N�Z�X

/* �G���R�[�_ */
#define		ENC_R_TCNT			(MTU7.TCNT)		// �E�G���R�[�_�p���X��
#define		ENC_L_TCNT			(MTU8.TCNT)		// ���G���R�[�_�p���X��


//**************************************************
// �񋓑́ienum�j
//**************************************************
/* �����Z���TID  */
typedef enum{
	DIST_SEN_R_FRONT = 0,		// �E�O
	DIST_SEN_L_FRONT,			// ���O
	DIST_SEN_R_SIDE,			// �E��
	DIST_SEN_L_SIDE,			// ����
	DIST_SEN_NUM
}enDIST_SEN_ID;

/* ���[�^ID */
typedef enum{
	DCM_R = 0,					// �E���[�^
	DCM_L,						// �����[�^
	DCM_MAX
}enDCM_ID;


//**************************************************
// �\���́istruct�j
//**************************************************


//**************************************************
// �O���[�o���ϐ�
//**************************************************


//**************************************************
// �v���g�^�C�v�錾�i�t�@�C�����ŕK�v�Ȃ��̂����L�q�j
//**************************************************
/* ���� */
PUBLIC void HAL_init( void );

/* �V���A���ʐM */
PUBLIC void SCI_putch(char data);
PUBLIC void SCI_puts(char *buffer);
PUBLIC void SCI_putsL(char *buffer, int len);
PUBLIC void charput(unsigned char data);
PUBLIC int SCI_chkRecv(void);
PUBLIC char SCI_getch(void);
PUBLIC unsigned char SCI_getch_uc(void);
PUBLIC int SCI_gets(char *buffer);
PUBLIC unsigned char charget(void);

/* �d���Ď� */
PUBLIC void BAT_Pol( void );
PUBLIC FLOAT BAT_getLv(void);

/* �W���C���Z���T */
PUBLIC void GYRO_Pol( void );
PUBLIC void GYRO_SetRef( void );
PUBLIC FLOAT GYRO_getSpeedErr( void );
PUBLIC FLOAT GYRO_getNowAngle( void );

/* �����Z���T */
PUBLIC void DIST_Pol_Front( void );
PUBLIC void DIST_Pol_Side( void );
PUBLIC SHORT DIST_getNowVal( enDIST_SEN_ID en_id );

/* �G���R�[�_ */
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

