//=============================================================================
//
// 木処理 [Item.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "Item.h"
#include "item.h"
#include "player.h"
#include "collision.h"
#include "score.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数

#define	Item_WIDTH			(50.0f)			// 頂点サイズ
#define	Item_HEIGHT			(80.0f)			// 頂点サイズ

#define	MAX_ITEM			(1500)			// 木最大数

#define ITEM_SPACINGX		(30.0f)						// 一つの四角形
#define ITEM_SPACINGZ		(30.0f)						// 一つの四角形

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	BOOL		bUse;			// 使用しているかどうか

} ITEM;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexItem(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static ITEM					g_aItem[MAX_ITEM];	// 木ワーク
static int					g_TexNo;			// テクスチャ番号
static BOOL					g_bAlpaTest;		// アルファテストON/OFF
//static int					g_nAlpha;			// アルファテストの閾値

static char* g_TextureName[] =
{
	"data/TEXTURE/snow01.png",
};


const int ITEMMAZE_WIDTH = 40;
const int ITEMMAZE_HEIGHT = 40;
int itemmaze[ITEMMAZE_HEIGHT][ITEMMAZE_WIDTH] = {
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

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{
	MakeVertexItem();

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

	// 木ワークの初期化
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{
		ZeroMemory(&g_aItem[nCntItem].material, sizeof(g_aItem[nCntItem].material));
		g_aItem[nCntItem].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aItem[nCntItem].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aItem[nCntItem].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aItem[nCntItem].fWidth = Item_WIDTH;
		g_aItem[nCntItem].fHeight = Item_HEIGHT;
		g_aItem[nCntItem].bUse = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	for (int z = 0; z < ITEMMAZE_HEIGHT; z++)
	{
		for (int x = 0; x < ITEMMAZE_WIDTH; x++)
		{
			if (itemmaze[z][x] == 0)
			{
				SetItem(XMFLOAT3(x * ITEM_SPACINGX-600.0f, 0.0f, z * ITEM_SPACINGZ-600.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			}
		}
	}



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitItem(void)
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
void UpdateItem(void)
{

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{
		if (g_aItem[nCntItem].bUse)
		{
			// 影の位置設定
			SetPositionShadow(g_aItem[nCntItem].nIdxShadow, XMFLOAT3(g_aItem[nCntItem].pos.x, 0.1f, g_aItem[nCntItem].pos.z));

			// 移動が終わったらエネミーとの当たり判定
			{
				PLAYER* player = GetPlayer();


					// 生きてるエネミーと当たり判定をする
				if (player[0].use == TRUE)
				{
					BOOL ans = CollisionBC(g_aItem[nCntItem].pos, player[0].pos, 5.0f,player[0].size);
					// 当たっている？
					if (ans == TRUE)
					{
						// 当たった時の処理
						g_aItem[nCntItem].bUse = FALSE;
						AddScore(5);
						ReleaseShadow(g_aItem[nCntItem].nIdxShadow);

					}
				}

			}

		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}


	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_aItem[i].bUse)
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
			mtxScl = XMMatrixScaling(g_aItem[i].scl.x, g_aItem[i].scl.y, g_aItem[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aItem[i].pos.x, g_aItem[i].pos.y, g_aItem[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_aItem[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexItem(void)
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

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 木のパラメータをセット
//=============================================================================
int SetItem(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxItem = -1;

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{
		if (!g_aItem[nCntItem].bUse)
		{
			g_aItem[nCntItem].pos = pos;
			g_aItem[nCntItem].scl = XMFLOAT3(0.2f, 0.2f, 0.2f);
			g_aItem[nCntItem].fWidth = fWidth;
			g_aItem[nCntItem].fHeight = fHeight;
			g_aItem[nCntItem].bUse = TRUE;

			// 影の設定
			g_aItem[nCntItem].nIdxShadow = CreateShadow(g_aItem[nCntItem].pos, 0.5f, 0.5f);

			nIdxItem = nCntItem;

			break;
		}
	}

	return nIdxItem;
}
