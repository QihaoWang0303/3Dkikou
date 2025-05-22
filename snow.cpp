//=============================================================================
//
// 弾発射処理 [Snow.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "Snow.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "snow.h"
#include "camera.h"
#include <random>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	SNOW_WIDTH		(20.0f)			// 頂点サイズ
#define	SNOW_HEIGHT		(20.0f)			// 頂点サイズ

#define	SNOW_SPEED		(0.5f)			// 弾の移動スピード


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexSnow(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static SNOW						g_Snow[MAX_SNOW];	// 弾ワーク
static int							g_TexNo;				// テクスチャ番号

static char* g_TextureName[] =
{
	"data/TEXTURE/snow002.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSnow(void)
{
	MakeVertexSnow();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// 弾ワークの初期化
	for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
	{
		ZeroMemory(&g_Snow[nCntSnow].material, sizeof(g_Snow[nCntSnow].material));
		g_Snow[nCntSnow].material.Diffuse = { 1.0f, 1.0f, 1.0f, 0.5f };

		g_Snow[nCntSnow].pos = { 0.0f, 0.0f, 0.0f };
		g_Snow[nCntSnow].rot = { 0.0f, 0.0f, 0.0f };
		g_Snow[nCntSnow].scl = { 1.0f, 1.0f, 1.0f };
		g_Snow[nCntSnow].spd = SNOW_SPEED;
		g_Snow[nCntSnow].fWidth = SNOW_WIDTH;
		g_Snow[nCntSnow].fHeight = SNOW_HEIGHT;
		g_Snow[nCntSnow].bUse = FALSE;

	}

	SetSnow();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSnow(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSnow(void)
{

	for (int i = 0; i < MAX_SNOW; i++)
	{
		if (g_Snow[i].bUse)
		{
			// 弾の移動処理
			float spd = g_Snow[i].spd+rand()%3;
			g_Snow[i].pos.y -= spd;
			g_Snow[i].pos.x += RandomFloat(-1.0f, 1.0f); // 随机漂移
			g_Snow[i].pos.z += RandomFloat(-1.0f, 1.0f);


			// 影の位置設定
			//SetPositionShadow(g_Snow[i].shadowIdx, XMFLOAT3(g_Snow[i].pos.x, 0.1f, g_Snow[i].pos.z));


			// フィールドの外に出たら弾を消す処理
			if (g_Snow[i].pos.x < MAP_LEFT
				|| g_Snow[i].pos.x > MAP_RIGHT
				|| g_Snow[i].pos.z < MAP_DOWN
				|| g_Snow[i].pos.z > MAP_TOP
				|| g_Snow[i].pos.y < 0)
			{
				g_Snow[i].bUse = FALSE;
				//ReleaseShadow(g_Snow[i].shadowIdx);
			}

					
				
			int activeCount = 0;
			for (int i = 0; i < MAX_SNOW; i++)
			{
				if (g_Snow[i].bUse) 
				{
					activeCount++;
				}
			}

			if (activeCount < 3000) 
			{
				for (int i = 0; i < MAX_SNOW; i++) {
					if (!g_Snow[i].bUse) 
					{
						SetSnow();
						break; 
					}
				}
			}


		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSnow(void)
{
	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;

	CAMERA* cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_SNOW; i++)
	{
		if (g_Snow[i].bUse)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();


			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// 正方行列（直交行列）を転置行列させて逆行列を作ってる版(速い)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// スケールを反映
			mtxScl = XMMatrixScaling(g_Snow[i].scl.x, g_Snow[i].scl.y, g_Snow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Snow[i].rot.x, g_Snow[i].rot.y + XM_PI, g_Snow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Snow[i].pos.x, g_Snow[i].pos.y, g_Snow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Snow[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_Snow[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);









		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexSnow(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = SNOW_WIDTH;
	float fHeight = SNOW_HEIGHT;

	//// 頂点座標の設定
	//vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	//vertex[1].Position = { fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	//vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	//vertex[3].Position = { fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetSnow()
{
	int nIdxSnow = -1;

	for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
	{
		if (!g_Snow[nCntSnow].bUse)
		{
			g_Snow[nCntSnow].pos = { RandomFloat(-600, 600),  RandomFloat(50, 300), RandomFloat(-600, 600) };
			//g_Snow[nCntSnow].pos = { 0,  100, 0 };
			g_Snow[nCntSnow].rot = {0.0f,3.0f,0.0f};
			g_Snow[nCntSnow].scl = { 0.5f, 0.5f, 0.5f };
			g_Snow[nCntSnow].bUse = TRUE;

			// 影の設定
			//g_Snow[nCntSnow].shadowIdx = CreateShadow(g_Snow[nCntSnow].pos, 0.5f, 0.5f);

			nIdxSnow = nCntSnow;

			//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxSnow;
}

//=============================================================================
// 弾の取得
//=============================================================================
SNOW* GetSnow(void)
{
	return &(g_Snow[0]);
}

// ランダム取得
float RandomFloat(float min, float max)
{
	static std::default_random_engine engine(static_cast<unsigned int>(std::time(0))); 
	std::uniform_real_distribution<float> dist(min, max); 
	return dist(engine);
}