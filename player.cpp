//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "model.h"
#include "wall.h"
#include "collision.h"
#include "breakwall.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名

#define	MODEL_PLAYER		"data/MODEL/mikusan/miku_body.obj"	// 読み込むモデル名

#define	MODEL_PLAYER_HEAD	"data/MODEL/mikusan/miku_head.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LARM	"data/MODEL/mikusan/miku_arm_l.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RARM	"data/MODEL/mikusan/miku_arm_r.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_LLEG	"data/MODEL/mikusan/miku_leg_l.obj"	// 読み込むモデル名
#define	MODEL_PLAYER_RLEG	"data/MODEL/mikusan/miku_leg_r.obj"	// 読み込むモデル名



#define	VALUE_MOVE			(4.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(5)								// プレイヤーのパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用


// プレイヤーの階層アニメーションデータ
// プレイヤーの左右パーツを動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 4.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI / 4, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 4.0f, 0.0f), XMFLOAT3(0.0f, XM_PI / 4, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },

};

static INTERPOLATION_DATA move_tbl_larm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(2.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(2.0f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },

};

static INTERPOLATION_DATA move_tbl_rarm[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-2.0f, 3.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-2.0f, 3.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },

};

static INTERPOLATION_DATA move_tbl_lleg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(2.0f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(2.0f, -4.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },

};

static INTERPOLATION_DATA move_tbl_rleg[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-2.0f, -4.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-2.0f, -4.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 120 },

};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_head,
	move_tbl_larm,
	move_tbl_rarm,
	move_tbl_lleg,
	move_tbl_rleg,

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { -520.0f, PLAYER_OFFSET_Y, -550.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号



	// 階層アニメーション用の初期化処理
	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
		//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
		//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

			// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Parts[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Parts[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent = &g_Player;	// 親をセット
	g_Parts[0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent = &g_Player;	// 親をセット
	g_Parts[1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[1].tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_LARM, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Player;	// 親をセット
	g_Parts[2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[2].tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[2].load = 1;
	LoadModel(MODEL_PLAYER_RARM, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Player;	// 親をセット
	g_Parts[3].tblNo = 3;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[3].tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[3].load = 1;
	LoadModel(MODEL_PLAYER_LLEG, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Player;	// 親をセット
	g_Parts[4].tblNo = 4;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[4].tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[4].load = 1;
	LoadModel(MODEL_PLAYER_RLEG, &g_Parts[4].model);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}


	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();

	// 動く前の座標を保存しておく（移動してぶつかった時はこの座標へ戻す為）
	//XMFLOAT3 pos;
	g_Player.oldpos = g_Player.pos;




	// 移動させちゃう
	if (GetKeyboardPress(DIK_A))
	{	// 左へ移動
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_D))
	{	// 右へ移動
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_W))
	{	// 上へ移動
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI;
	}
	if (GetKeyboardPress(DIK_S))
	{	// 下へ移動
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 0.0f;
	}

	//ゲームパッド
	if (IsButtonPressed(0, BUTTON_DOWN))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 0.0f;
	}
	else if (IsButtonPressed(0, BUTTON_UP))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI;
	}

	if (IsButtonPressed(0, BUTTON_RIGHT))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = -XM_PI / 2;
	}
	else if (IsButtonPressed(0, BUTTON_LEFT))
	{
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI / 2;
	}


#ifdef _DEBUG
	//if (GetKeyboardPress(DIK_R))
	//{
	//	g_Player.pos.z = g_Player.pos.x = 0.0f;
	//	g_Player.rot.y = g_Player.dir = 0.0f;
	//	g_Player.spd = 0.0f;
	//}
#endif


	//	// Key入力があったら移動処理する
	if (g_Player.spd > 0.0f)
	{
		g_Player.rot.y = g_Player.dir + cam->rot.y;

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// 弾発射処理
	if (IsMouseLeftTriggered()|| GetKeyboardTrigger(DIK_SPACE))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
		
		XMFLOAT3 pos = cam->pos;   

		XMFLOAT3 direction;
		direction = cam->rot;
		direction.y += 3.14f;
		 
		SetBullet(pos, direction);

		// BGM再生
		//PlaySound(SOUND_LABEL_BGM_BGMtitle);

	}

	if (IsButtonTriggered(0, BUTTON_B))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
		
		XMFLOAT3 pos = cam->pos;   

		XMFLOAT3 direction;
		direction = cam->rot;
		direction.y += 3.14f;
		 
		SetBullet(pos, direction);

		// BGM再生
		//PlaySound(SOUND_LABEL_BGM_BGMtitle);

	}

	g_Player.spd *= 0.5f;


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tblMax > 0))
		{	// 線形補間の処理
			int nowNo = (int)g_Parts[i].time;			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i].tblMax;				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
			INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// 行動テーブルのアドレスを取得

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i].time - nowNo;	// 時間部分である少数を取り出している

			Pos *= nowTime;								// 現在の移動量を計算している
			Rot *= nowTime;								// 現在の回転量を計算している
			Scl *= nowTime;								// 現在の拡大率を計算している

			// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
			XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

			// 計算して求めた回転量を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

			// 計算して求めた拡大率を現在の移動テーブルに足している
			XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

			// frameを使て時間経過処理をする
			g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}

		}

	}

	for (int i = 0; i < MAX_WALL; i++)
	{
		// 移動が終わったらwallとの当たり判定
		{

			// wallの数分当たり判定を行う
			WALL* wall = GetWall();

				// 生きてるwallと当たり判定をする
			if (wall[i].use == TRUE)
			{
				BOOL ans = CollisionBC( g_Player.pos, wall[i].pos,PLAYER_SIZE, WALL_SIZE);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					g_Player.pos = g_Player.oldpos;
				}
			}

		}

	}

	for (int i = 0; i < MAX_BREAKWALL; i++)
	{
		// 移動が終わったらwallとの当たり判定
		{

			// wallの数分当たり判定を行う
			BREAKWALL* breakwall = GetBreakwall();

			// 生きてるwallと当たり判定をする
			if (breakwall[i].use == TRUE)
			{
				BOOL ans = CollisionBC(g_Player.pos, breakwall[i].pos, PLAYER_SIZE, BREAKWALL_SIZE);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					g_Player.pos = g_Player.oldpos;
				}
			}

		}

	}


	{	// ポイントライトのテスト
		LIGHT* light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{

	if (g_Player.use == FALSE) return;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// まずは基本となるＢＯＤＹを描画する

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	// 自分を描画したときにしようしたマトリクスを保存しておく
	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Player.model);



	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_Parts[i].model);

	}


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}
