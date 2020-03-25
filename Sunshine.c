// *************************************************************************
//   ���{�b�g��	�F �d���x�[�V�b�NDC�}�E�X�A�T���V���C��
//   �T�v		�F �T���V���C���̃��C���t�@�C��
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.10.31			�O��			�V�K�i�t�@�C���̃C���N���[�h�j
// 		v1.1		2013.11.04			�O��			�^�C�}�[���荞�݂̏������ǉ�
// 		v1.2		2013.11.05			�O��			�������v�ǉ�
// 		v1.3		2013.11.05			�O��			�\�t�g�E�G�A�E�G�C�g�ǉ�
// 		v1.4		2013.11.05			�O��			LED�_�ŏ����ǉ�
// 		v1.5		2013.11.13			�O��			���[�h�ǉ�
// 		v1.6		2013.11.13			�O��			�V���A���ʐM�{printf�ǉ�
// 		v1.7		2013.11.17			�O��			�d���Ď��ǉ�
// 		v1.8		2013.11.22			�O��			�W���C���Z���T�ǉ�
// 		v1.9		2013.12.01			�O��			�����Z���T�ǉ�
// 		v2.0		2013.12.03			�O��			�G���R�[�_�ǉ�
// 		v2.1		2013.12.04			�O��			DCM�ǉ�
//		v2.1.1		2020.02.25			fucchi			GitHub�idevelop�j�g�p�J�n�i�\�[�X�ω������j
// *************************************************************************/

//**************************************************
// �v���W�F�N�g�쐬���ɐ������ꂽ�R�[�h �i�g��Ȃ����ǂƂ肠�����c���Ă����j
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
// �C���N���[�h�t�@�C���iinclude�j
//**************************************************
#include <typedefine.h>						// ��`
#include <iodefine.h>						// I/O
#include <stdio.h>							// �W�����o��
#include <hal.h>							// HAL


//**************************************************
// ��`�idefine�j
//**************************************************
#define		SW_CHATTERING_WAIT			(200)			// �X�C�b�`�̃`���^�����O�΍��p�̃J�E���^�A200[msec]


//**************************************************
// �񋓑́ienum�j
//**************************************************
/* ���샂�[�h */
typedef enum{
	MODE_0 = 0,					// ���[�h0 �F ���[�h�̐������L�ڂ���
	MODE_1,						// ���[�h1 �F ���[�h�̐������L�ڂ���
	MODE_2,						// ���[�h2 �F ���[�h�̐������L�ڂ���
	MODE_3,						// ���[�h3 �F ���[�h�̐������L�ڂ���
	MODE_4,						// ���[�h4 �F ���[�h�̐������L�ڂ���
	MODE_5,						// ���[�h5 �F ���[�h�̐������L�ڂ���
	MODE_6,						// ���[�h6 �F ���[�h�̐������L�ڂ���
	MODE_7,						// ���[�h7 �F ���[�h�̐������L�ڂ���
	MODE_8,						// ���[�h8 �F ���[�h�̐������L�ڂ���
	MODE_MAX
}enMODE;


//**************************************************
// �\���́istruct�j
//**************************************************


//**************************************************
// �O���[�o���ϐ�
//**************************************************
PRIVATE	VUSHORT		uc_Msec;	// �������v[msec]
PRIVATE	VUCHAR		uc_Sec;		// �������v[sec]
PRIVATE	VUCHAR		uc_Min;		// �������v[min]
PRIVATE VULONG		ul_Wait;	// 1msec��wait�Ɏg�p���� �J�E���^[msec]
PRIVATE enMODE		en_Mode;	// ���݂̃��[�h
//PRIVATE stDIST_SEN	st_sen[DIST_SEN_NUM];		

//**************************************************
// �v���g�^�C�v�錾�i�t�@�C�����ŕK�v�Ȃ��̂����L�q�j
//**************************************************



// *************************************************************************
//   �@�\		�F ���荞�݊֐��A�V�X�e���p�imsec�^�C�}�j
//   ����		�F �Ȃ�
//   ����		�F MTU0��TGRA���荞�݁A1msec���Ɋ֐������s�����B
//				�F ����Ƀ}�C�R�������삵�Ă���ꍇ�A1sec������LED_SYS���_�ł���B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.04			�O��			�V�K
// 		v1.1		2013.11.05			�O��			�������v�ǉ�
// 		v1.2		2013.11.05			�O��			�\�t�g�E�G�A�E�G�C�g�ǉ�
// 		v1.3		2013.11.05			�O��			�V�X�e���pLED����ǉ�
// *************************************************************************/
PUBLIC void INTC_sys( void )
{
	static USHORT	i = 0;
	
	/* �V�X�e������m�F�pLED  */
	if( i == 500 ){
		LED_SYS = ~LED_SYS;		// LED��ON��OFF������
		i=0;
	}
	else{
		i++;
	}

	/* ---------- */
	/*  �������v  */
	/* ---------- */
	uc_Msec++;					// msec
	if( uc_Msec > 999 ){		// msec �� sec
		uc_Msec  = 0;
		uc_Sec++;
	}
	if( uc_Sec > 59 ){			// sec �� min
		uc_Sec = 0;
		uc_Min++;
	}

	/* ----------------------- */
	/*  S/W�E�F�C�g�E�J�E���^  */
	/* ----------------------- */
	ul_Wait++;
	ul_Wait %= 6000000;			// 10 min (= 6000000 �J�E���g) �� 0 �N���A
	
}


// *************************************************************************
//   �@�\		�F ���荞�݊֐��A�Z���T�p
//   ����		�F �Ȃ�
//   ����		�F MTU4��TGRA���荞�݁A0.25msec���Ɋ֐������s�����B
//				�F ����Ƀ}�C�R�������삵�Ă���ꍇ�A250usec������
//				�F �����Z���T0 �� �����Z���T1 �� �����Z���T2 �� �W���C���Z���T �̏��ŃX�L��������B
//				�F �]���āA1�Z���T�̃X�L����������1msec�ƂȂ�B
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.04			�O��			�V�K
// *************************************************************************/
PUBLIC void INTC_sen( void )
{
	static UCHAR	i = 0;
	
	/* �Z���T����  */
	switch( i ){
		case 0:		// �W���C���Z���T
			GYRO_Pol();			
			break;
		
		case 1:		// �O�ǃZ���T
			DIST_Pol_Front();
			break;
		
		case 2:		// ���ǃZ���T
			DIST_Pol_Side();
			break;
		
		case 3:		// �΂ߕǃZ���T
			// �΂߃Z���T�͎g�p���Ȃ�
			break;
		
	}
	
	i = ( i + 1 ) % 4;			// roop
	
	return;
}


// *************************************************************************
//   �@�\		�F �}�C�R���̃��W�X�^������������B
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.10.31			�O��			�V�K
// 		v1.1		2013.11.11			�O��			�X�C�b�`�ǉ�
// 		v1.2		2013.11.13			�O��			�V���A���ʐM�ǉ�
// *************************************************************************/
PRIVATE void CPU_init(void)
{
	int i;		// loop�p
	
	/* ============== */
	/*  �N���b�N�ݒ�  */
	/* ============== */
	SYSTEM.SCKCR.BIT.ICK 		= 0;		// �V�X�e���N���b�N(ICLK)		�Ӂ~8 (96MHz)
	SYSTEM.SCKCR.BIT.PCK 		= 0;		// ���Ӄ��W���[���N���b�N(PCLK)	�Ӂ~8 (96MHz)

	/* ================== */
	/*  GPIO(�ėp���o��)  */
	/* ================== */
	/* �o�͒l */
	PORT3.DR.BIT.B2				= 0;		// �|�[�g3-2�̏����o��0[V]	(���퓮��m�F�pLED)
	PORT5.DR.BIT.B1				= 0;		// �|�[�g5-1�̏����o��0[V]	(�o�b�e���pLED)
	PORT5.DR.BIT.B2				= 0;		// �|�[�g5-2�̏����o��0[V]	(�o�b�e���pLED)
	PORTA.DR.BIT.B0				= 0;		// �|�[�gA-0�̏����o��0[V]	(�E���[�^PWM�o��)
	PORTB.DR.BIT.B0				= 0;		// �|�[�gB-0�̏����o��0[V]	(�����[�^PWM�o��)
	PORTB.DR.BIT.B3				= 0;		// �|�[�gB-3�̏����o��0[V]	(�E���[�^INA�o��)
	PORTB.DR.BIT.B4				= 0;		// �|�[�gB-4�̏����o��0[V]	(�E���[�^INA�o��)
	PORTB.DR.BIT.B5				= 0;		// �|�[�gB-5�̏����o��0[V]	(�E���[�^INB�o��)
	PORTB.DR.BIT.B6				= 0;		// �|�[�gB-6�̏����o��0[V]	(�����[�^INA�o��)
	PORTB.DR.BIT.B7				= 0;		// �|�[�gB-7�̏����o��0[V]	(�����[�^INB�o��)
	PORTD.DR.BYTE				= 0x00;		// �|�[�gD�̏����o��0[V]	(�f�o�b�O�pLED)
	PORTE.DR.BYTE				= 0x00;		// �|�[�gE�̏����o��0[V]	(�Z���TLED)
	
	/* ���o�͐ݒ� */
	PORT3.DDR.BIT.B2			= 1;		// �|�[�g3-2���o�͂ɐݒ�	(���퓮��m�F�pLED)
	PORT5.DDR.BIT.B1			= 1;		// �|�[�g5-1���o�͂ɐݒ�	(�o�b�e���pLED)
	PORT5.DDR.BIT.B2			= 1;		// �|�[�g5-2���o�͂ɐݒ�	(�o�b�e���pLED)
	PORTA.DDR.BIT.B0			= 1;		// �|�[�gA-0���o�͂ɐݒ�	(�E���[�^PWM�o��)
	PORTB.DDR.BIT.B0			= 1;		// �|�[�gB-0���o�͂ɐݒ�	(�����[�^PWM�o��)
	PORTB.DDR.BIT.B3			= 1;		// �|�[�gB-3���o�͂ɐݒ�	(���[�^ENA�o��)
	PORTB.DDR.BIT.B4			= 1;		// �|�[�gB-4���o�͂ɐݒ�	(�E���[�^INA�o��)
	PORTB.DDR.BIT.B5			= 1;		// �|�[�gB-5���o�͂ɐݒ�	(�E���[�^INB�o��)
	PORTB.DDR.BIT.B6			= 1;		// �|�[�gB-6���o�͂ɐݒ�	(�����[�^INA�o��)
	PORTB.DDR.BIT.B7			= 1;		// �|�[�gB-7���o�͂ɐݒ�	(�����[�^INB�o��)
	PORTD.DDR.BYTE				= 0xff;		// �|�[�gD���o�͂ɐݒ�		(�f�o�b�O�pLED)
	PORTE.DDR.BYTE				= 0x0f;		// �|�[�gE���o�͂ɐݒ�		(�Z���TLED)

	/* ���̓v���A�b�v�ݒ� */
	PORTC.PCR.BIT.B6			= 1;		// �|�[�gC-6�̓v���A�b�v���g�p	(�v�b�V���X�C�b�`�p)
	PORTC.PCR.BIT.B7			= 1;		// �|�[�gC-7�̓v���A�b�v���g�p	(�v�b�V���X�C�b�`�p)
	
	/* ���̓o�b�t�@�R���g���[�����W�X�^�ݒ�i���̓|�[�g�Ɠ����@�\��ڑ����邽�߂ɂ���j */
	PORTC.ICR.BIT.B6			= 1;		// �|�[�gC-6����̓|�[�g�Ɛڑ�����
	PORTC.ICR.BIT.B7			= 1;		// �|�[�gC-7����̓|�[�g�Ɛڑ�����
	PORT3.ICR.BIT.B0			= 1;		// RXD1��P3-1��ڑ�����
	PORTC.ICR.BIT.B0			= 1;		// MTCLKG��PC-0��ڑ�����
	PORTC.ICR.BIT.B1			= 1;		// MTCLKH��PC-1��ڑ�����
	PORTC.ICR.BIT.B2			= 1;		// MTCLKE��PC-2��ڑ�����
	PORTC.ICR.BIT.B3			= 1;		// MTCLKF��PC-3��ڑ�����


	/* ======== */
	/*  �^�C�}  */
	/* ======== */
	/* ����d�͒ጸ�@�\�̐ݒ� */
	SYSTEM.MSTPCRA.BIT.MSTPA8 	= 0;		// MTU-unit1(MTU0�`5 )�̃X�^���o�C����
	SYSTEM.MSTPCRA.BIT.MSTPA9 	= 0;		// MTU-unit2(MTU6�`11)�̃X�^���o�C����
	
	// -----------------------
	//  �V�X�e���p(MTU0)
	// -----------------------
	/* ���荞�݃R���g���[���̐ݒ� */
	ICU.IER[0xE].BIT.IEN2 		= 1;		// MTU0-TGRA�̊��荞�݂̋���
	ICU.IPR[0x51].BIT.IPR 		= 7;		// MTU0-TGRA�̊��荞�݃��x���̐ݒ�

	/* �^�C�}���荞�݂̐ݒ� */
	MTU0.TCR.BIT.CCLR 			= 1;		// TGRA�̃R���y�A�}�b�`��TCNT�N���A
	MTU0.TCR.BIT.TPSC 			= 3;		// PCLK(96MHz)/64 ��1�J�E���g
	MTU0.TIER.BIT.TGIEA			= 1;		// TGRA�Ƃ̃R���y�A�}�b�`�Ŋ��荞�݋���
	MTU0.TGRA 				= 375 * 4;	// 1msec���Ɋ��荞��
	MTU0.TCNT 				= 0;		// �^�C�}�N���A
	

	// -----------------------
	//  �Z���T�p(MTU4)
	// -----------------------
	/* ���荞�݃R���g���[���̐ݒ� */
	ICU.IER[0x10].BIT.IEN6 			= 1;		// MTU4-TGRA�̊��荞�݂̋���
	ICU.IPR[0x59].BIT.IPR 			= 8;		// MTU4-TGRA�̊��荞�݃��x���̐ݒ�

	/* �^�C�}���荞�݂̐ݒ� */
	MTU4.TCR.BIT.CCLR 			= 1;		// TGRA�̃R���y�A�}�b�`��TCNT�N���A
	MTU4.TCR.BIT.TPSC 			= 3;		// PCLK(96MHz)/64 ��1�J�E���g
	MTU4.TIER.BIT.TGIEA			= 1;		// TGRA�Ƃ̃R���y�A�}�b�`�Ŋ��荞�݋���
	MTU4.TGRA 				= 94 * 4;	// 250msec���Ɋ��荞��
	MTU4.TCNT 				= 0;		// �^�C�}�N���A


	// --------------------------
	//  �E���[�^PWM�o�͗p(MTU6)
	// --------------------------
	MTU6.TCR.BIT.CCLR 			= 1;		// TGRA�̃R���y�A�}�b�`��TCNT�N���A
	MTU6.TCR.BIT.TPSC 			= 1;		// PCLK(96MHz)/4 ��1�J�E���g
	MTU6.TMDR.BIT.MD			= 2;		// PWM ���[�h 1
	MTU6.TIORH.BIT.IOA			= 2;		// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 1 �o��
	MTU6.TIORH.BIT.IOB			= 1;		// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	MTU6.TGRA 				= 1200;		// ����(100usec)
	MTU6.TGRB 				= 600;		// onDuty
	MTU6.TCNT 				= 0;		// �^�C�}�N���A

	// --------------------------
	//  �E���[�^�ʑ��v���p(MTU7)
	// --------------------------
	MTU7.TCR.BIT.CCLR 			= 0;		// �R���y�A�}�b�`��TCNT�N���A�֎~
	MTU7.TIER.BIT.TGIEA 			= 0;		// TGRA �Ƃ̃R���y�A�}�b�`�Ŋ��荞�ݖ�����
	MTU7.TMDR.BIT.MD			= 7;		// �ʑ��v�����[�h 4
	MTU7.TGRA 				= 0;		// �n�߂͖��ݒ�
	MTU7.TCNT 				= 32768;	// ������
	
	// --------------------------
	//  �����[�^�ʑ��v���p(MTU8)
	// --------------------------
	MTU8.TCR.BIT.CCLR 			= 0;		// �R���y�A�}�b�`��TCNT�N���A�֎~
	MTU8.TIER.BIT.TGIEA 			= 0;		// TGRA �Ƃ̃R���y�A�}�b�`�Ŋ��荞�ݖ�����
	MTU8.TMDR.BIT.MD			= 7;		// �ʑ��v�����[�h 4
	MTU8.TGRA 				= 0;		// �n�߂͖��ݒ�
	MTU8.TCNT 				= 32768;	// ������
	
	// --------------------------
	//  �����[�^PWM�o�͗p(MTU9)
	// --------------------------
	MTU9.TCR.BIT.CCLR 			= 1;		// TGRA�̃R���y�A�}�b�`��TCNT�N���A
	MTU9.TCR.BIT.TPSC 			= 1;		// PCLK(96MHz)/4 ��1�J�E���g
	MTU9.TMDR.BIT.MD			= 2;		// PWM ���[�h 1
	MTU9.TIORH.BIT.IOA			= 2;		// TIOCA �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 1 �o��
	MTU9.TIORH.BIT.IOB			= 1;		// TIOCB �[�q�̋@�\ : �����o�͂� 0 �o�́B�R���y�A�}�b�`�� 0 �o��
	MTU9.TGRA 				= 1200;		// ����(50usec)
	MTU9.TGRB 				= 500;		// onDuty
	MTU9.TCNT 				= 0;		// �^�C�}�N���A

	// -----------------------
	//  �o�b�e���Ď��p(MTU10)
	// -----------------------
	/* ���荞�݃R���g���[���̐ݒ� */
	ICU.IER[0x14].BIT.IEN2 			= 1;		// MTU10-TGRA�̊��荞�݂̋���
	ICU.IPR[0x64].BIT.IPR 			= 6;		// MTU10-TGRA�̊��荞�݃��x���̐ݒ�

	/* �^�C�}���荞�݂̐ݒ� */
	MTU10.TCR.BIT.CCLR 			= 1;		// TGRA�̃R���y�A�}�b�`��TCNT�N���A
	MTU10.TCR.BIT.TPSC 			= 5;		// PCLK(96MHz)/1024 ��1�J�E���g
	MTU10.TIER.BIT.TGIEA			= 1;		// TGRA�Ƃ̃R���y�A�}�b�`�Ŋ��荞�݋���
	MTU10.TGRA 				= 234 * 4;	// 10msec���Ɋ��荞��
	MTU10.TCNT 				= 0;		// �^�C�}�N���A
	
	
	/* ===== */
	/*  SCI  */
	/* ===== */
	SYSTEM.MSTPCRB.BIT.MSTPB30		= 0;		// SCI1�X�^���o�C����
	SCI1.SCR.BYTE				= 0x00;		// TE=0�ARE=0�ACKE1=0
	SCI1.SMR.BYTE				= 0x00;		// CHR=0�APE=0�ASTOP=0�ACKS=00(P��/1)
	SCI1.BRR				= 100;		// 57600[bps](PCLK(96MHz)
	SCI1.SEMR.BIT.ABCS			= 1;		// ABCS=1
	for( i=0; i<0x800000; i++ );		// 1bit����wait
	SCI1.SCR.BYTE				= 0x70;		// TIE=0�ARIE=1�ATE=1�ARE=1
	/* ���荞�݃R���g���[���̐ݒ� */
	ICU.IER[0x1B].BIT.IEN3 			= 1;		// SCI1-RXI1�̊��荞�݂̋���
	ICU.IPR[0x81].BIT.IPR 			= 5;		// SCI1�̊��荞�݃��x���̐ݒ�
	

	/* ========= */
	/*  A/D�ϊ�  */
	/* ========= */
	SYSTEM.MSTPCRA.BIT.MSTPA17 		= 0;		// AD(12bit)�X�^���o�C����
	S12AD.ADCSR.BIT.CKS 			= 3;		// PCLK�ŕϊ�
}


// *************************************************************************
//   �@�\		�F �}�C�R���̃^�C�}���J�n����B
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.04			�O��			�V�K
// *************************************************************************/
PRIVATE void RX621_staTimer(void)
{
	MTUA.TSTR.BIT.CST0			= 1;		// MTU0�J�E���g�J�n			�V�X�e��
	MTUA.TSTR.BIT.CST4			= 1;		// MTU4�J�E���g�J�n			�Z���T
	MTUB.TSTR.BIT.CST1			= 1;		// MTU7�J�E���g�J�n			�ʑ��v��
	MTUB.TSTR.BIT.CST2			= 1;		// MTU8�J�E���g�J�n			�ʑ��v��
	MTUB.TSTR.BIT.CST4			= 1;		// MTU10�J�E���g�J�n		�o�b�e���Ď�
}


// *************************************************************************
//   �@�\		�F ���Ԑ���̏������B
//   ����		�F �N����A1�񂾂����s����֐��B
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.05			�O��			�V�K
// 		v1.1		2013.11.05			�O��			�\�t�g�E�G�A�E�G�C�g�ǉ�
// *************************************************************************/
PUBLIC void TIME_init( void )
{
	/* ������ */
	uc_Msec = 0;		// �������v[msec]
	uc_Sec  = 0;		// �������v[sec]
	uc_Min  = 0;		// �������v[min]
	ul_Wait = 0;		// 1msec��wait�Ɏg�p���� �J�E���^[msec]
}


// *************************************************************************
//   �@�\		�F �w�肵�� ms �ԁAS/W�E�F�C�g����B
//   ����		�F �Ȃ�
//   ����		�F ul_time: �҂��� ( msec �P�� )�B �ő� 600000(= 10 min) ����
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.05			�O��			�V�K
// *************************************************************************/
PUBLIC void TIME_wait( ULONG ul_time )
{
	ul_Wait = 0;						// 0����J�E���g����A"ul_Wait"��1msec����1���Z�����B

	while( ul_Wait < ul_time );			// �w�莞�Ԍo�߂���܂ŃE�F�C�g
	
	return;
}


// *************************************************************************
//   �@�\		�F �w�肵���t���[�J�E���g�ԁAS/W�E�F�C�g����B
//   ����		�F �Ȃ�
//   ����		�F ul_cnt: �J�E���g�l
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.05			�O��			�V�K
// *************************************************************************/
PUBLIC void TIME_waitFree( ULONG ul_cnt )
{
	while( ul_cnt-- );			// 0�ɂȂ�܂Ńf�B�N�������g
}


// *************************************************************************
//   �@�\		�F ���[�h�����Z�ύX����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PRIVATE void MODE_inc( void )
{
	en_Mode++;		// ���[�h�����Z
	
	/* �ő�l�`�F�b�N */
	if( MODE_MAX == en_Mode ){
		en_Mode = MODE_0;
	}
	
	/* ���[�h�\�� */
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


// *************************************************************************
//   �@�\		�F ���[�h�����s����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.13			�O��			�V�K
// *************************************************************************/
PRIVATE void MODE_exe( void )
{
	/* ���[�h�\�� */
	switch( en_Mode ){
	
		case MODE_0:
			LED8 = LED8_ALL_ON;
			
			while(1){
			
				printf("[�d���d��] %5.2f[mV] \n\r", BAT_getLv() );
				TIME_wait( 500 );
			}
			break;

		case MODE_1:
			LED8 = LED8_ALL_ON;

			while(1){

				printf("   [�W���C���p�x]%5.2f [AD]%d \r", 
					1.2394f * GYRO_getNowAngle(), (USHORT)GYRO_getNowAngle()			// 1.2394 = (1 / 4096[div]) * 3300[mV]  / 0.65 [mV/dps]      dps=deg/sec
				);
				TIME_wait( 50 );
			}
			break;

		case MODE_2:
			LED8 = LED8_ALL_ON;

			while(1){

				printf("   �����Z���T [R_F]%5d [L_F]%5d [R_S]%5d [L_S]%5d \r", 
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

				printf("�G���R�[�_ [R]=%d [L]=%d \r", ENC_R_TCNT, ENC_L_TCNT );
				TIME_wait( 50 );
			}
			break;

		case MODE_4:
			LED8 = LED8_ALL_ON;

			/* ���[�^�쓮 */
			DCM_setDirCw( DCM_R );				// ���v���
			DCM_setDirCcw( DCM_L );				// �����v���
			DCM_setPwmDuty( DCM_R, 180 );		// PWM�ݒ�ADuty20%
			DCM_setPwmDuty( DCM_L, 180 );		// PWM�ݒ�ADuty20%
			DCM_staMotAll();					// ���[�^����J�n
			break;

		case MODE_5:
			LED8 = LED8_ALL_ON;

			/* ���[�^��~ */
			DCM_stopMot( DCM_R );				// ���[�^��~
			DCM_stopMot( DCM_L );				// ���[�^��~
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


// *************************************************************************
//   �@�\		�F ������J�n����
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.11.25			�O��			�V�K
// *************************************************************************/
PRIVATE void SYS_start( void )
{
	UCHAR	i;

	/* LED��ʁ̉���4bit�_�� */
	for( i = 0; i < 5; i++ ){
	
		LED8 = 0x0f;
		TIME_wait(50);
		LED8 = 0xf0;
		TIME_wait(50);
	}
	LED8 = LED8_ALL_OFF;		// �S����
	
	/* �^�C�g���\�� */
	printf(" ����������������������������������������������\r\n");
	printf(" �� Robo Name  : Koban                       ��\r\n");
	printf(" �� Developer  : T.Yoshida                   ��\r\n");
	printf(" �� Version    : Protype1���@                ��\r\n");
	printf(" �� Target     : ����p                      ��\r\n");
	printf(" �� Project By : Hosei Univ. Denken Group    ��\r\n");
	printf(" ����������������������������������������������\r\n");
	
}

	
// *************************************************************************
//   �@�\		�F main����
//   ����		�F �������I�������Ȃ��悤�ɂ���B
//   ����		�F �Ȃ�
//   ����		�F �Ȃ�
//   �Ԃ�l		�F �Ȃ�
// **************************    ��    ��    *******************************
// 		v1.0		2013.10.31			�O��			�V�K
// 		v1.1		2013.11.05			�O��			�^�C�}�[���荞�݂̏������ǉ�
// 		v1.2		2013.11.05			�O��			�^�C�}�[���W���[���̏������ǉ�
// 		v1.3		2013.11.05			�O��			LED����ǉ�
// 		v1.4		2013.11.27			�O��			����J�n�p�֐��쐬
// *************************************************************************/
void main(void)
{
	/* ������ */
	CPU_init();							// [CPU] ���W�X�^/GPIO/AD/TIMER�Ȃ�
	TIME_init();						// [SYS] �^�C�}�[
	HAL_init();							// [HAL] �n�[�h�E�G�A�h���C�o
	RX621_staTimer();					// [CPU] �^�C�}�X�^�[�g
	SYS_start();						// [SYS] ����J�n
		
	LED8 = LED8_ALL_ON;

	/* ���[�^�쓮 */
	DCM_setDirCw( DCM_R );				// ���v���
	DCM_setDirCcw( DCM_L );				// �����v���
	DCM_setPwmDuty( DCM_R, 180 );		// PWM�ݒ�ADuty20%
	DCM_setPwmDuty( DCM_L, 180 );		// PWM�ݒ�ADuty20%
	DCM_staMotAll();					// ���[�^����J�n
	LED8 = LED8_ALL_OFF;
	/* big roop */
	while(1){
		if ( SW_ON == SW_INC_PIN ){
			MODE_inc();								// ���[�h��1�i�߂�
			TIME_wait(SW_CHATTERING_WAIT);			// SW���������܂ő҂�
		}
		else if ( SW_ON == SW_EXE_PIN ){
			MODE_exe();								// ���[�h���s
			TIME_wait(SW_CHATTERING_WAIT);			// SW���������܂ő҂�
		}
	}
}








//**************************************************
// �v���W�F�N�g�쐬���ɐ������ꂽ�R�[�h �i�g��Ȃ����ǂƂ肠�����c���Ă����j
//**************************************************
#ifdef __cplusplus
void abort(void)
{

}

#endif

