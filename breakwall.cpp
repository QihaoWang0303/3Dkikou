//=============================================================================
//
// Breakwall処理 [Breakwall.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "player.h"
#include "collision.h"
#include "score.h"
#include "breakwall.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BREAKWALL			"data/MODEL/wall_003.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define BREAKWALL_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define BREAKWALL_OFFSET_Y		(7.0f)						// エネミーの足元をあわせる


#define BREAKWALL_SPACINGX		(30.0f)						// 一つの四角形
#define BREAKWALL_SPACINGZ		(30.0f)						// 一つの四角形

const int BREAKWALLMAZE_WIDTH = 40;
const int BREAKWALLMAZE_HEIGHT = 40;
int breakwallmaze[BREAKWALLMAZE_HEIGHT][BREAKWALLMAZE_WIDTH] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 5, 5, 5, 1, 1, 5, 5, 1, 1, 5, 5, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1},
	{1, 5, 0, 0, 1, 1, 5, 5, 1, 1, 5, 5, 1, 1, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 5, 5, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 5, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1},
	{1, 5, 0, 0, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 1, 1, 5, 0, 5, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1},
	{1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 5, 1},
	{1, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 3, 3, 1, 1},
	{1, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 3, 3, 1, 1},
	{1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1},
	{1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 1},
	{1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 5, 5, 0, 0, 0, 0, 5, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 1, 1},
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BREAKWALL			g_Breakwall[MAX_BREAKWALL];				// エネミー

int g_Breakwall_load = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBreakwall(void)
{
	for (int i = 0; i < MAX_BREAKWALL; i++)
	{
		LoadModel(MODEL_BREAKWALL, &g_Breakwall[i].model);
		g_Breakwall[i].load = TRUE;

		g_Breakwall[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Breakwall[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Breakwall[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Breakwall[i].spd = 0.0f;			// 移動スピードクリア
		g_Breakwall[i].size = BREAKWALL_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Breakwall[i].model, &g_Breakwall[i].diffuse[0]);

		XMFLOAT3 pos = g_Breakwall[i].pos;
		pos.y -= (BREAKWALL_OFFSET_Y - 0.1f);
		g_Breakwall[i].shadowIdx = CreateShadow(pos, BREAKWALL_SHADOW_SIZE, BREAKWALL_SHADOW_SIZE);

		g_Breakwall[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Breakwall[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Breakwall[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット


		g_Breakwall[i].use = FALSE;		// TRUE:生きてる

	}


	for (int z = 0; z < BREAKWALLMAZE_HEIGHT; z++)
	{
		for (int x = 0; x < BREAKWALLMAZE_WIDTH; x++)
		{
			if (breakwallmaze[z][x] == 3)
			{
				SetBreakwall({ x * BREAKWALL_SPACINGX-600.0f, BREAKWALL_OFFSET_Y, z * BREAKWALL_SPACINGZ-600.0f }, { 0.0f, 0.0f, 0.0f });

			}

		}
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBreakwall(void)
{

	for (int i = 0; i < MAX_BREAKWALL; i++)
	{
		if (g_Breakwall[i].load)
		{
			UnloadModel(&g_Breakwall[i].model);
			g_Breakwall[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBreakwall(void)
{


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBreakwall(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_BREAKWALL; i++)
	{
		if (g_Breakwall[i].use == FALSE) continue;
		//SetFuchi(1);

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Breakwall[i].scl.x, g_Breakwall[i].scl.y, g_Breakwall[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Breakwall[i].rot.x, g_Breakwall[i].rot.y + XM_PI, g_Breakwall[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Breakwall[i].pos.x, g_Breakwall[i].pos.y, g_Breakwall[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Breakwall[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Breakwall[i].model);
	}

	//SetFuchi(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
BREAKWALL* GetBreakwall()
{
	return &g_Breakwall[0];
}

//=============================================================================
// Breakwallの設定
//=============================================================================
int SetBreakwall(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxBreakwall = -1;

	for (int nCntBreakwall = 0; nCntBreakwall < MAX_BREAKWALL; nCntBreakwall++)
	{
		if (!g_Breakwall[nCntBreakwall].use)
		{
			g_Breakwall[nCntBreakwall].pos = pos;
			g_Breakwall[nCntBreakwall].rot = rot;
			g_Breakwall[nCntBreakwall].scl = { 1.5f, 10.0f, 2.5f };
			g_Breakwall[nCntBreakwall].use = TRUE;

			// 影の設定
			//g_Breakwall [nCntBreakwall] .shadowIdx = CreateShadow(g_Breakwall[nCntBreakwall].pos, 0.5f, 0.5f);

			nIdxBreakwall = nCntBreakwall;


			break;
		}
	}

	return nIdxBreakwall;
}
