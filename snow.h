#pragma once
#include <DirectXMath.h>
#include "main.h"
#include "renderer.h"

//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_SNOW		(3000)	// 弾最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		spd;			// 移動量
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			shadowIdx;		// 影ID
	BOOL		bUse;			// 使用しているかどうか


} SNOW;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSnow(void);
void UninitSnow(void);
void UpdateSnow(void);
void DrawSnow(void);

int SetSnow(void);

SNOW* GetSnow(void);

float RandomFloat(float min, float max);
