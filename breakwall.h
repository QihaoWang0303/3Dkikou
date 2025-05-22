#pragma once
//=============================================================================
//
// �G�l�~�[���f������ [breakwall.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_BREAKWALL		(50)					// �G�l�~�[�̐�

#define	BREAKWALL_SIZE		(20.0f)				// �����蔻��̑傫��
#define	BREAKWALL_WIDTH		(5.0f)				// �����蔻��̑傫��
#define	BREAKWALL_HEIGHT		(5.0f)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BREAKWALL
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	float				time;				// ���`��ԗp
	int					tblNo;				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax;				// ���̃e�[�u���̃f�[�^��

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBreakwall(void);
void UninitBreakwall(void);
void UpdateBreakwall(void);
void DrawBreakwall(void);

BREAKWALL* GetBreakwall(void);

int SetBreakwall(XMFLOAT3 pos, XMFLOAT3 rot);
