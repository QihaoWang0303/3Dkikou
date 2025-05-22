#pragma once
#include <DirectXMath.h>
//=============================================================================
//
// item処理 [tree.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

int SetItem(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


