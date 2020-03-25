// *************************************************************************
//   ���{�b�g��	�F �d���x�[�V�b�NDC�}�E�X�A�T���V���C��
//   �T�v		�F �T���V���C����HAL�i�n�[�h�E�G�A���ۑw�j�t�@�C��
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.10.31			�O��			�V�K�i�t�@�C���̃C���N���[�h�j
// *************************************************************************/

//**************************************************
// �C���N���[�h�t�@�C���iinclude�j
//**************************************************
#include <typedefine.h>						// ��`
#include <iodefine.h>						// I/O
#include <hal.h>							// HAL


//**************************************************
// ��`�idefine�j
//**************************************************
#define		BAT_GOOD			(2296)			// �c�ʌ����Ă����i���F�j�A1�Z��3.7V�ȏ�F 2297 = ( 3700mV * 2�Z�� ) / 4(����) / 3300 * 4096 - 1
#define		BAT_LOW				(2109)			// �c�ʂ�΂��I�I�i�ԐF�j�A1�Z��3.4V�ȏ�F 2109 = ( 3400mV * 2�Z�� ) / 4(����) / 3300 * 4096 - 1
#define 	GYRO_REF_NUM		( 100 )			// �W���C���̃��t�@�����X�l
#define 	SEN_WAIT_CNT		( 175 )			// �Z���T�̔�������҂��i�����l�j
#define		ENC_RESET_VAL		(32768)			// �G���R�[�_�̒��Ԓl
#define		ENC_R_TSTR			(MTUB.TSTR.BIT.CST1)	// �E�G���R�[�_�p���X�J�E���g�J�n
#define		ENC_L_TSTR			(MTUB.TSTR.BIT.CST2)	// ���G���R�[�_�p���X�J�E���g�J�n
#define		DCM_ENA				(PORTB.DR.BIT.B3)		// DCM�̗L��/����
#define		DCM_R_IN1			(PORTB.DR.BIT.B6)		// DCM�EIN1
#define		DCM_R_IN2			(PORTB.DR.BIT.B7)		// DCM�EIN2
#define		DCM_L_IN1			(PORTB.DR.BIT.B4)		// DCM��IN1
#define		DCM_L_IN2			(PORTB.DR.BIT.B5)		// DCM��IN2
#define		DCM_R_TIMER			(MTUB.TSTR.BIT.CST3)	// DCM�E�^�C�}�J�n
#define		DCM_L_TIMER			(MTUB.TSTR.BIT.CST0)	// DCM���^�C�}�J�n
#define		DCM_R_TIORA			(MTU9.TIORH.BIT.IOA)	// DCM�E�s���o�͐ݒ�A
#define		DCM_R_TIORB			(MTU9.TIORH.BIT.IOB)	// DCM�E�s���o�͐ݒ�B
#define		DCM_L_TIORA			(MTU6.TIORH.BIT.IOA)	// DCM���s���o�͐ݒ�A
#define		DCM_L_TIORB			(MTU6.TIORH.BIT.IOB)	// DCM���s���o�͐ݒ�B
#define		DCM_R_TCNT			(MTU9.TCNT)				// DCM�E�J�E���g�l
#define		DCM_L_TCNT			(MTU6.TCNT)				// DCM���J�E���g�l
#define		DCM_R_GRA			(MTU9.TGRA)				// DCM�E����
#define		DCM_R_GRB			(MTU9.TGRB)				// DCM�EDuty��
#define		DCM_L_GRA			(MTU6.TGRA)				// DCM������
#define		DCM_L_GRB			(MTU6.TGRB)				// DCM��Duty��


//**************************************************
// �񋓑́ienum�j
//**************************************************

//**************************************************
// �\���́istruct�j
//**************************************************
/* �����Z���T���i�S�Z���T���ʁj */
typedef struct{
	SHORT		s_now;						// LED �_�����̋����Z���T�̌��ݒl           ( AD �l )
	SHORT		s_old;						// LED �_�����̋����Z���T��1�O�̒l        ( AD �l )
	SHORT		s_limit;					// �����Z���T��臒l                         ( AD �l ) ( ���̒l���傫���ꍇ�A�ǂ���Ɣ��f���� )
	SHORT		s_ref;						// ���̒��S�ɒu�������̋����Z���T�̊�l ( AD �l )
	SHORT		s_offset;					// LED �������̋����Z���T�̒l               ( AD �l )
	SHORT		s_ctrl;						// ����L��������ۂ�臒l                   ( AD �l ) ��ɑO�ǂŎg�p
	SHORT		s_noCtrl;					// �ǂɋ߂����邽�ߐ��䖳��������ۂ�臒l   ( AD �l ) ��ɑO�ǂŎg�p
}stDIST_SEN;


//**************************************************
// �O���[�o���ϐ�
//**************************************************
/* �o�b�e���Ď� */
PRIVATE USHORT	us_BatLvAve = 4095;							// �o�b�e�����ϒl�iAD�ϊ��̍ő�l�ŏ������j

/* �W���C���Z���T */
PRIVATE SHORT s_GyroVal; 					  				// �W���C���Z���T�̌��ݒl
PRIVATE SHORT s_GyroValBuf[8];								// �W���C���Z���T�̃o�b�t�@�l
PUBLIC FLOAT  f_NowAngle;			 						// �W���C���Z���T�̌��݊p�x
PRIVATE LONG  l_GyroRef; 									// �W���C���Z���T�̊�l

/* �����Z���T */
PRIVATE stDIST_SEN	st_sen[DIST_SEN_NUM];					// �����Z���T


//**************************************************
// �v���g�^�C�v�錾�i�t�@�C�����ŕK�v�Ȃ��̂����L�q�j
//**************************************************




// *************************************************************************
//   �@�\		�F HAL������������B
//   ����		�F �Ȃ�
//   ����		�F �����ϐ��Ȃǂ��N���A����B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.27			�O��			�V�K
// *************************************************************************/
PUBLIC void HAL_init( void )
{
	/* �W���C���Z���T */
	f_NowAngle = 0;				// �W���C���Z���T�̌��݊p�x
	l_GyroRef  = 0;				// �W���C���Z���T�̊�l

	/* �����Z���T */
	memset( st_sen, 0, sizeof(st_sen) );				// �����Z���T(�S�Z���T����)
	
	/* �G���R�[�_ */
	ENC_Sta();
	
	/* DCM*/
	DCM_ENA = ON;

}


// *************************************************************************
//   �@�\		�F 1�����o��
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC void SCI_putch(char data)
{
	while(1) {
		if (SCI1.SSR.BYTE & 0x80) {			// ���M�o�b�t�@�̋󂫃`�F�b�N
			SCI1.TDR = data;
			SCI1.SSR.BYTE = SCI1.SSR.BYTE & 0x40;
			break;
		}
	}
}


// *************************************************************************
//   �@�\		�F ������o��
//   ����		�F �Ȃ�
//   ����		�F "\n"�݂̂�"CR+LF"�o�͂��s���B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC void SCI_puts(char *buffer)
{
	char data;
	
	/* null�܂ŏo�� */
	while( (data = *( buffer++ ) ) != 0 ){
		
		/* �f�[�^�̒l�ɉ����ďo�͂�ς��� */
		if (data == 0x0a) {
			SCI_putch(0x0d);		// CR�o��
			SCI_putch(0x0a);		// LF�o��
		} else {
			SCI_putch(data);		// 1�����o��
		}
	}
}


// *************************************************************************
//   �@�\		�F ������o��
//   ����		�F �Ȃ�
//   ����		�F �����񒷂��w��t��"/\n"�̂� "CR+LF"�o�͂��s���B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC void SCI_putsL(char *buffer, int len)
{
	int i;
	char data;
	
	for( i=0; i<len; i++ ){
		data=*(buffer++);
		
		/* �f�[�^�̒l�ɉ����ďo�͂�ς��� */
		if (data == 0x0a) {
			SCI_putch(0x0d);		// CR�o��
			SCI_putch(0x0a);		// LF�o��
		} else {
			SCI_putch(data);		// 1�����o��
		}
	}
}


// *************************************************************************
//   �@�\		�F 1�����o�͗p���b�p�[�֐�
//   ����		�F �Ȃ�
//   ����		�F printf�Ȃǂ̒჌�x���o�͂Ɏg�p�����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
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
//   �@�\		�F ���̓o�b�t�@�`�F�b�N
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC int SCI_chkRecv(void)
{
	/* �f�[�^�̎�M�`�F�b�N */
	if (SCI1.SSR.BYTE & 0x40) {
		return 1;		// ��M�f�[�^����
	}	else {
		return 0;		// ��M�f�[�^�Ȃ�
	}
}


// *************************************************************************
//   �@�\		�F 1�������́ichar�^�Łj
//   ����		�F �Ȃ�
//   ����		�F �G�R�[�o�b�N�Ȃ��B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC char SCI_getch(void)
{
	char data;
	
	while(1){
		/* �f�[�^�̎�M�`�F�b�N */
		if ( SCI_chkRecv() ) {
			data = SCI1.RDR;						// �f�[�^��M
			SCI1.SSR.BYTE = SCI1.SSR.BYTE & 0x80;
			break;
		}
	}
	return data;
}

// *************************************************************************
//   �@�\		�F 1�������́iunsigned char�^�Łj
//   ����		�F �Ȃ�
//   ����		�F �G�R�[�o�b�N�Ȃ��B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC unsigned char SCI_getch_uc(void)
{
	unsigned char data;
	while(1){
		/* �f�[�^�̎�M�`�F�b�N */
		if ( SCI_chkRecv() ) {
			data = SCI1.RDR;						// �f�[�^��M
			SCI1.SSR.BYTE = SCI1.SSR.BYTE & 0x80;
			break;
		}
	}
	return data;
}


// *************************************************************************
//   �@�\		�F ���������
//   ����		�F �Ȃ�
//   ����		�F CR�R�[�h�܂�/�ő�255����/�G�R�[�o�b�N����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC int SCI_gets(char *buffer)
{
	char data;
	int  i = 0;

	while(1){
		data = SCI_getch();		// 1��������
		*buffer = data;
		SCI_putch(data);		// 1�����o��(�G�R�[�o�b�N)
		buffer++;
		i++;
		if (i > 255)      break;	// �ő啶�����ɓ��B
		if (data == 0x0D) break;	// CR�R�[�h�܂Ŏ�M����
	}
	*buffer = (unsigned char)0;		// null
	
	return i;						// ���͕�������ԋp
}


// *************************************************************************
//   �@�\		�F 1�������͗p���b�p�[�֐�
//   ����		�F �Ȃ�
//   ����		�F scanf�Ȃǂ̒჌�x���o�͂Ɏg�p�����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC unsigned char charget(void)
{
	return SCI_getch_uc();
}


// *************************************************************************
//   �@�\		�F �o�b�e���d�����擾����
//   ����		�F �Ȃ�
//   ����		�F ���O5��̕��ϒl
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �d��[mV]
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PUBLIC FLOAT BAT_getLv(void)
{
	FLOAT f_val = (FLOAT)( us_BatLvAve + 1 );		// �l��0����n�܂邩��1�����Z
	
	return ( f_val / 4096 * 3300 * 4.0f );
}


// *************************************************************************
//   �@�\		�F �o�b�e���Ď��p�|�[�����O�֐�
//   ����		�F �Ȃ�
//   ����		�F ���荞�݂�����s�����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.16			�O��			�V�K
// *************************************************************************/
PUBLIC void BAT_Pol( void )
{
	static USHORT 	us_batLv[5] = { 4095, 4095, 4095, 4095, 4095 };		// �o�b�e�����x���iAD�ϊ��̍ő�l�ŏ������j

	/* ================================================== */
	/*  ���ϒl���擾���邽�߁A�f�[�^�̃o�b�t�@�������s��  */
	/* ================================================== */
	/* �o�b�t�@���V�t�g */
	us_batLv[1] = us_batLv[0];
	us_batLv[2] = us_batLv[1];
	us_batLv[3] = us_batLv[2];
	us_batLv[4] = us_batLv[3];

	/* �ŐV�̒l���i�[ */
	S12AD.ADANS.BIT.ANS 		= 0x80;		// AN7 �ϊ��Ώېݒ�
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD�ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD�ϊ��҂�
	us_batLv[0] = S12AD.ADDR7;				// AN7 �ϊ��f�[�^�擾

	/* �d�����ω� */
	us_BatLvAve = ( us_batLv[0] + us_batLv[1] + us_batLv[2] + us_batLv[3] + us_batLv[4] ) / 5;
	
	/*  �c�ʂɉ�����LED��\��  */
	/* ======================= */
	if( us_BatLvAve < BAT_LOW ) {			// �c�ʂ�΂��I�I�i�ԐF�j
		LED_BAT0 = OFF;
		LED_BAT1 = ON;
	}
	else if( us_BatLvAve < BAT_GOOD ) {		// �c�ʌ����Ă����i���F�j
		LED_BAT0 = ON;
		LED_BAT1 = ON;
	}
	else{									// �c�ʖ��Ȃ��i�ΐF�j
		LED_BAT0 = ON;
		LED_BAT1 = OFF;
	}
}


// *************************************************************************
//   �@�\		�F �W���C���Z���T�̃��t�@�����X�l�i��̒l�j��ݒ肷��
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.26			�O��			�V�K
// *************************************************************************/
PUBLIC void GYRO_SetRef( void )
{
	USHORT i;
	ULONG ul_ref = 0;
	
	/* �f�[�^�T���v�����O */
	for( i=0; i<GYRO_REF_NUM; i++){			// 100��T���v�����O�������ϒl����̒l�Ƃ���B
		ul_ref += (ULONG)s_GyroVal;
		TIME_wait(1);
	}
	
	/* ��l�Z�o�i���ϒl�j */
	l_GyroRef = ul_ref / GYRO_REF_NUM * 100;		// ���x��100�{�ɂ���
}


// *************************************************************************
//   �@�\		�F �W���C���̊p���x�Ɋւ��鐧��΍����擾����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.26			�O��			�V�K
// *************************************************************************/
PUBLIC FLOAT GYRO_getSpeedErr( void )
{
	LONG  l_val = (LONG)s_GyroVal * 100 ;				// 100�{�̐��x�ɂ���
	LONG  l_err = l_val - l_GyroRef ;
	FLOAT f_res;
	
	/* �p���x�̕΍��Z�o */
	if( ( l_err < -10 * 100 ) || ( 10 * 100 < l_err ) ){
		f_res = (FLOAT)l_err * 1.2394 / 100;		// 1.2394 = (1 / 4096[div]) * 3300[mV]  / 0.65 [mV/dps]  dps=deg/sec
													// 100�{�̐��x
	}
	else{
		f_res = 0;									// [deg/s]
	}
	
	return f_res;
}


// *************************************************************************
//   �@�\		�F �W���C���̌��݂̊p�x���擾����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.26			�O��			�V�K
// *************************************************************************/
PUBLIC FLOAT GYRO_getNowAngle( void )
{
	return f_NowAngle;
}


// *************************************************************************
//   �@�\		�F �W���C���Z���T�p�|�[�����O�֐�
//   ����		�F �Ȃ�
//   ����		�F ���荞�݂�����s�����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.26			�O��			�V�K
// *************************************************************************/
PUBLIC void GYRO_Pol( void )
{
	FLOAT f_speed;
	
	/* �o�b�t�@�V�t�g�i[7]�ɐV�����f�[�^�����邽�߁A[0]�̃f�[�^���̂ĂāA1���l�߂�j */
	s_GyroValBuf[0]	= s_GyroValBuf[1];
	s_GyroValBuf[1]	= s_GyroValBuf[2];
	s_GyroValBuf[2]	= s_GyroValBuf[3];
	s_GyroValBuf[3]	= s_GyroValBuf[4];
	s_GyroValBuf[4]	= s_GyroValBuf[5];
	s_GyroValBuf[5]	= s_GyroValBuf[6];
	s_GyroValBuf[6]	= s_GyroValBuf[7];
	
	/* �ŐV�̃W���C���Z���T�l���擾 */
	S12AD.ADANS.BIT.ANS 		= 0x40;		// AN6 �ϊ��Ώېݒ�
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD�ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD�ϊ��҂�
	s_GyroValBuf[7]	= (SHORT)S12AD.ADDR6;
	
	/* �W���C���̒l�𕽊�����i��������8�j */
	s_GyroVal = ( s_GyroValBuf[0] + s_GyroValBuf[1] + s_GyroValBuf[2] + s_GyroValBuf[3] +
				  s_GyroValBuf[4] + s_GyroValBuf[5] + s_GyroValBuf[6] + s_GyroValBuf[7] ) / 8;
	
	/* ���݂̊p�x���X�V���� */
	f_speed = GYRO_getSpeedErr();			// �p���x�擾 (0.001sec���̊p���x)
	f_NowAngle += f_speed / 1000;			// �p�x�ݒ�   (0.001sec���ɉ��Z���邽��)
}


// *************************************************************************
//   �@�\		�F �����Z���T�̒l���擾����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.02			�O��			�V�K
// *************************************************************************/
PUBLIC SHORT DIST_getNowVal( enDIST_SEN_ID en_id )
{
	return st_sen[en_id].s_now;
}


// *************************************************************************
//   �@�\		�F �����Z���T�p�i�O�ǁj�|�[�����O�֐�
//   ����		�F �Ȃ�
//   ����		�F ���荞�݂�����s�����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.02			�O��			�V�K
// *************************************************************************/
PUBLIC void DIST_Pol_Front( void )
{
	/* ���������̒l�擾 */
	S12AD.ADANS.BIT.ANS 		= 0x05;		// AN0/2 �ϊ��Ώېݒ�
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD�ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD�ϊ��҂�
	st_sen[DIST_SEN_R_FRONT].s_offset = (SHORT)S12AD.ADDR0;
	st_sen[DIST_SEN_L_FRONT].s_offset = (SHORT)S12AD.ADDR2;

	/* �O��LED�_�� */
	LED_DIST_RF = ON;
	LED_DIST_LF = ON;
	
	/* ��������҂� */
	TIME_waitFree( SEN_WAIT_CNT );

	/* �������̒l�Ɩ��������̒l�ō������擾 */
	S12AD.ADANS.BIT.ANS 		= 0x05;		// AN0/2 �ϊ��Ώېݒ�
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD�ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD�ϊ��҂�
	st_sen[DIST_SEN_R_FRONT].s_old = st_sen[DIST_SEN_R_FRONT].s_now;		// �o�b�t�@�����O
	st_sen[DIST_SEN_L_FRONT].s_old = st_sen[DIST_SEN_L_FRONT].s_now;		// �o�b�t�@�����O
	st_sen[DIST_SEN_R_FRONT].s_now = (SHORT)S12AD.ADDR0 - st_sen[DIST_SEN_R_FRONT].s_offset;		// ���ݒl��������
	st_sen[DIST_SEN_L_FRONT].s_now = (SHORT)S12AD.ADDR2 - st_sen[DIST_SEN_L_FRONT].s_offset;		// ���ݒl��������
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
	/* �O��LED���� */
	LED_DIST_RF = OFF;
	LED_DIST_LF = OFF;
}


// *************************************************************************
//   �@�\		�F �����Z���T�p�i���ǁj�|�[�����O�֐�
//   ����		�F �Ȃ�
//   ����		�F ���荞�݂�����s�����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.02			�O��			�V�K
// *************************************************************************/
PUBLIC void DIST_Pol_Side( void )
{
	/* ���������̒l�擾 */
	S12AD.ADANS.BIT.ANS 		= 0x0a;		// AN1/3 �ϊ��Ώېݒ�
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD�ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD�ϊ��҂�
	st_sen[DIST_SEN_R_SIDE].s_offset = (SHORT)S12AD.ADDR1;
	st_sen[DIST_SEN_L_SIDE].s_offset = (SHORT)S12AD.ADDR3;

	/* ����LED�_�� */
	LED_DIST_RS = ON;
	LED_DIST_LS = ON;
	
	/* ��������҂� */
	TIME_waitFree( SEN_WAIT_CNT );

	/* �������̒l�Ɩ��������̒l�ō������擾 */
	S12AD.ADANS.BIT.ANS 		= 0x0a;		// AN1/3 �ϊ��Ώېݒ�
	S12AD.ADCSR.BIT.ADST 		= 1;		// AD�ϊ��J�n
	while( S12AD.ADCSR.BIT.ADST == 1);		// AD�ϊ��҂�
	st_sen[DIST_SEN_R_SIDE].s_old = st_sen[DIST_SEN_R_SIDE].s_now;		// �o�b�t�@�����O
	st_sen[DIST_SEN_L_SIDE].s_old = st_sen[DIST_SEN_L_SIDE].s_now;		// �o�b�t�@�����O
	st_sen[DIST_SEN_R_SIDE].s_now = (SHORT)S12AD.ADDR1 - st_sen[DIST_SEN_R_SIDE].s_offset;		// ���ݒl��������
	st_sen[DIST_SEN_L_SIDE].s_now = (SHORT)S12AD.ADDR3 - st_sen[DIST_SEN_L_SIDE].s_offset;		// ���ݒl��������
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
	/* ����LED���� */
	LED_DIST_RS = OFF;
	LED_DIST_LS = OFF;
}


// *************************************************************************
//   �@�\		�F �G���R�[�_�̃J�E���g���J�n����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void ENC_Sta( void )
{
	ENC_R_TSTR = ON;		// �J�E���g�J�n
	ENC_L_TSTR = ON;		// �J�E���g�J�n
}


// *************************************************************************
//   �@�\		�F �G���R�[�_�̃J�E���g���~����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void ENC_Stop( void )
{
	ENC_R_TSTR = OFF;		// �J�E���g��~
	ENC_L_TSTR = OFF;		// �J�E���g��~
}


// *************************************************************************
//   �@�\		�F �G���R�[�_�̃J�E���g���N���A
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PRIVATE void ENC_clr( void )
{
	ENC_R_TCNT = ENC_RESET_VAL;
	ENC_L_TCNT = ENC_RESET_VAL;
}


// *************************************************************************
//   �@�\		�F �G���R�[�_�̃J�E���g�l�i�p���X���j���擾����
//   ����		�F �Ȃ�
//   ����		�F ���Ԓl����̍���
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void ENC_GetDiv( LONG* p_r, LONG* p_l )
{
	LONG l_cntR = (LONG)ENC_R_TCNT;
	LONG l_cntL = (LONG)ENC_L_TCNT;
	
	ENC_clr();		// �J�E���g�l���Z�b�g
	
	*p_r = l_cntR - ENC_RESET_VAL;		// �E���[�^
	*p_l = ENC_RESET_VAL - l_cntL;		// �����[�^
}


// *************************************************************************
//   �@�\		�F DCM�̉�]������CW�i���v���j�ɂ���
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_setDirCw( enDCM_ID en_id )
{
	/* ��]�����ݒ� */
	if( en_id == DCM_R ){			// �E
		DCM_R_IN1 = ON;				// BIN1
		DCM_R_IN2 = OFF;			// BIN2
	}
	else{							// ��
		DCM_L_IN1 = OFF;			// AIN1
		DCM_L_IN2 = ON;				// AIN2
	}
}


// *************************************************************************
//   �@�\		�F DCM�̉�]������CCW�i�����v���j�ɂ���
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_setDirCcw( enDCM_ID en_id )
{
	/* ��]�����ݒ� */
	if( en_id == DCM_R ){			// �E
		DCM_R_IN1 = OFF;			// BIN1
		DCM_R_IN2 = ON;				// BIN2
	}
	else{							// ��
		DCM_L_IN1 = ON;				// AIN1
		DCM_L_IN2 = OFF;			// AIN2
	}
}


// *************************************************************************
//   �@�\		�F DCM���~����
//   ����		�F �Ȃ�
//   ����		�F PWM��HI�o�͒��ɖ{�֐������s����ƁA�s����100%�o�͏�ԂȂ邽�߁A�֐����Ńs�����N���A�iLo�j����B
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_stopMot( enDCM_ID en_id )
{
	/* ��~�ݒ� */
	if( en_id == DCM_R ){			// �E
		DCM_R_IN1 = OFF;			// BIN1
		DCM_R_IN2 = OFF;			// BIN2
		DCM_R_TIMER = OFF;			// �^�C�}��~
		DCM_R_TIORA = 1;			// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
		DCM_R_TIORB = 1;			// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	}
	else{							// ��
		DCM_L_IN1 = OFF;			// AIN1
		DCM_L_IN2 = OFF;			// AIN2
		DCM_L_TIMER = OFF;			// �^�C�}��~
		DCM_L_TIORA = 1;			// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
		DCM_L_TIORB = 1;			// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	}
}


// *************************************************************************
//   �@�\		�F DCM���u���[�L���O����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_brakeMot( enDCM_ID en_id )
{
	/* ��~�ݒ� */
	if( en_id == DCM_R ){			// �E
		DCM_R_IN1 = ON;				// BIN1
		DCM_R_IN2 = ON;				// BIN2
		DCM_R_TIMER = OFF;			// �^�C�}��~
		DCM_R_TIORA = 1;			// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
		DCM_R_TIORB = 1;			// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	}
	else{							// ��
		DCM_L_IN1 = ON;				// AIN1
		DCM_L_IN2 = ON;				// AIN2
		DCM_L_TIMER = OFF;			// �^�C�}��~
		DCM_L_TIORA = 1;			// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	    DCM_L_TIORB = 1;			// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	}
}


// *************************************************************************
//   �@�\		�F DCM�𓮍�J�n����
//   ����		�F �Ȃ�
//   ����		�F ����J�n�O��PWM�Ɖ�]�������w�肵�Ă�������
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_staMot( enDCM_ID en_id )
{
	/* �^�C�}�X�^�[�g */
	if( en_id == DCM_R ){			// �E
		DCM_R_TIORA = 2;			// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 1 �o��
		DCM_R_TIORB = 1;			// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
		DCM_R_TIMER = ON;			// �^�C�}�J�n
	}
	else{							// ��
		DCM_L_TIORA = 2;			// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 1 �o��
	    DCM_L_TIORB = 1;			// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
		DCM_L_TIMER = ON;			// �^�C�}�J�n
	}
}


// *************************************************************************
//   �@�\		�F �SDCM�𓮍�J�n����
//   ����		�F �Ȃ�
//   ����		�F ����J�n�O��PWM�Ɖ�]�������w�肵�Ă�������
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_staMotAll( void )
{
	DCM_staMot(DCM_R);									// �E���[�^ON
	DCM_staMot(DCM_L);									// �����[�^ON
}


// *************************************************************************
//   �@�\		�F DCM��PWM-Duty��ݒ肷��
//   ����		�F ���荞�݊O����ݒ肷��ƁA�_�u���o�b�t�@�łȂ��Ǝ������ɂȂ�ꍇ������B
//   ����		�F ���荞�݃n���h��������s���邱�ƁBDuty0%�̏ꍇ���[�^���~������iPWM�ɂЂ����o��j
//   ����		�F ���[�^ID
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.12.03			�O��			�V�K
// *************************************************************************/
PUBLIC void DCM_setPwmDuty( enDCM_ID en_id, USHORT us_duty10 )
{
	USHORT	us_cycle;							// ����
	USHORT	us_onReg;							// �ݒ肷��ON-duty
	
	/* PWM�ݒ� */
	if( en_id == DCM_R ){				// �E
	
		if( 0 == us_duty10 ){			// Duty0%�ݒ�
			DCM_brakeMot( en_id );
		}
		else if( 1000 <= us_duty10 ){	// Duty100%

			DCM_R_TIMER = OFF;			// �^�C�}��~
			DCM_R_TCNT = 0;				// TCNT �J�E���^���N���A
			DCM_R_GRB = 5000;			// �^�C�}�l�ύX
			DCM_R_TIORA = 6;			// TIOCA �[�q�̋@�\ : �����o�͂� 1 �o�́B�R���y�A�}�b�`�� 1 �o��
			DCM_R_TIORB = 6;			// TIOCB �[�q�̋@�\ : �����o�͂� 1 �o�́B�R���y�A�}�b�`�� 1 �o��
			DCM_R_TIMER = ON;			// �^�C�}�J�n
			us_duty10 = 1000;
		}
		else{
			us_cycle = DCM_R_GRA;		// ����
			us_onReg = (USHORT)( (ULONG)us_cycle * (ULONG)us_duty10 / (ULONG)1000 );	// Duty2Reg �v�Z��
			DCM_R_TIMER = OFF;			// �^�C�}��~
			DCM_R_TCNT = 0;				// TCNT �J�E���^���N���A
			DCM_R_GRB = us_onReg;		// onDuty
			DCM_staMot( en_id );		// ��]�J�n
		}
	}
	else{								// ��

		if( 0 == us_duty10 ){			// Duty0%
			DCM_brakeMot( en_id );
		}
		else if( 1000 <= us_duty10 ){	// Duty100%

			DCM_L_TIMER = OFF;			// �^�C�}��~
			DCM_L_TCNT = 0;				// TCNT �J�E���^���N���A
			DCM_L_GRB = 5000;			// �^�C�}�l�ύX
			DCM_L_TIORA = 6;			// TIOCA �[�q�̋@�\ : �����o�͂� 1 �o�́B�R���y�A�}�b�`�� 1 �o��
			DCM_L_TIORB = 6;			// TIOCB �[�q�̋@�\ : �����o�͂� 1 �o�́B�R���y�A�}�b�`�� 1 �o��
			DCM_L_TIMER = ON;			// �^�C�}�J�n
			us_duty10 = 1000;
		}
		else{
			us_cycle = DCM_L_GRA;		// ����
			us_onReg = (USHORT)( (ULONG)us_cycle * (ULONG)us_duty10 / (ULONG)1000 );	// Duty2Reg �v�Z��
			DCM_L_TIMER = OFF;			// �^�C�}��~
			DCM_L_TCNT = 0;				// TCNT �J�E���^���N���A
			DCM_L_GRB = us_onReg;		// �^�C�}�l�ύX
			DCM_staMot( en_id );		// ��]�J�n
		}
	}
}


