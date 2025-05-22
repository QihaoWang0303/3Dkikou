#pragma once
#include <DirectXMath.h>
//=============================================================================
//
// item���� [tree.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

int SetItem(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


