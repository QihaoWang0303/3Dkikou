#pragma once
#include <DirectXMath.h>
#include "main.h"
#include "renderer.h"

//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_SNOW		(3000)	// �e�ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			shadowIdx;		// �eID
	BOOL		bUse;			// �g�p���Ă��邩�ǂ���


} SNOW;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSnow(void);
void UninitSnow(void);
void UpdateSnow(void);
void DrawSnow(void);

int SetSnow(void);

SNOW* GetSnow(void);

float RandomFloat(float min, float max);
