#pragma once
//=============================================================================
//
// エネミーモデル処理 [breakwall.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BREAKWALL		(50)					// エネミーの数

#define	BREAKWALL_SIZE		(20.0f)				// 当たり判定の大きさ
#define	BREAKWALL_WIDTH		(5.0f)				// 当たり判定の大きさ
#define	BREAKWALL_HEIGHT		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BREAKWALL
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号

	float				time;				// 線形補間用
	int					tblNo;				// 行動データのテーブル番号
	int					tblMax;				// そのテーブルのデータ数

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBreakwall(void);
void UninitBreakwall(void);
void UpdateBreakwall(void);
void DrawBreakwall(void);

BREAKWALL* GetBreakwall(void);

int SetBreakwall(XMFLOAT3 pos, XMFLOAT3 rot);
