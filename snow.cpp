//=============================================================================
//
// �e���ˏ��� [Snow.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	SNOW_WIDTH		(20.0f)			// ���_�T�C�Y
#define	SNOW_HEIGHT		(20.0f)			// ���_�T�C�Y

#define	SNOW_SPEED		(0.5f)			// �e�̈ړ��X�s�[�h


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexSnow(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static SNOW						g_Snow[MAX_SNOW];	// �e���[�N
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char* g_TextureName[] =
{
	"data/TEXTURE/snow002.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSnow(void)
{
	MakeVertexSnow();

	// �e�N�X�`������
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

	// �e���[�N�̏�����
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
// �I������
//=============================================================================
void UninitSnow(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSnow(void)
{

	for (int i = 0; i < MAX_SNOW; i++)
	{
		if (g_Snow[i].bUse)
		{
			// �e�̈ړ�����
			float spd = g_Snow[i].spd+rand()%3;
			g_Snow[i].pos.y -= spd;
			g_Snow[i].pos.x += RandomFloat(-1.0f, 1.0f); // �����Y��
			g_Snow[i].pos.z += RandomFloat(-1.0f, 1.0f);


			// �e�̈ʒu�ݒ�
			//SetPositionShadow(g_Snow[i].shadowIdx, XMFLOAT3(g_Snow[i].pos.x, 0.1f, g_Snow[i].pos.z));


			// �t�B�[���h�̊O�ɏo����e����������
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
// �`�揈��
//=============================================================================
void DrawSnow(void)
{
	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;

	CAMERA* cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_SNOW; i++)
	{
		if (g_Snow[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();


			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Snow[i].scl.x, g_Snow[i].scl.y, g_Snow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Snow[i].rot.x, g_Snow[i].rot.y + XM_PI, g_Snow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Snow[i].pos.x, g_Snow[i].pos.y, g_Snow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Snow[i].mtxWorld, mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Snow[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);









		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexSnow(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = SNOW_WIDTH;
	float fHeight = SNOW_HEIGHT;

	//// ���_���W�̐ݒ�
	//vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	//vertex[1].Position = { fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	//vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	//vertex[3].Position = { fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �e�̃p�����[�^���Z�b�g
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

			// �e�̐ݒ�
			//g_Snow[nCntSnow].shadowIdx = CreateShadow(g_Snow[nCntSnow].pos, 0.5f, 0.5f);

			nIdxSnow = nCntSnow;

			//	PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxSnow;
}

//=============================================================================
// �e�̎擾
//=============================================================================
SNOW* GetSnow(void)
{
	return &(g_Snow[0]);
}

// �����_���擾
float RandomFloat(float min, float max)
{
	static std::default_random_engine engine(static_cast<unsigned int>(std::time(0))); 
	std::uniform_real_distribution<float> dist(min, max); 
	return dist(engine);
}