//=============================================================================
//
// ���f������ [player.cpp]
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
// �}�N����`
//*****************************************************************************
//#define	MODEL_PLAYER		"data/MODEL/player.obj"			// �ǂݍ��ރ��f����

#define	MODEL_PLAYER		"data/MODEL/mikusan/miku_body.obj"	// �ǂݍ��ރ��f����

#define	MODEL_PLAYER_HEAD	"data/MODEL/mikusan/miku_head.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LARM	"data/MODEL/mikusan/miku_arm_l.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_RARM	"data/MODEL/mikusan/miku_arm_r.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LLEG	"data/MODEL/mikusan/miku_leg_l.obj"	// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_RLEG	"data/MODEL/mikusan/miku_leg_r.obj"	// �ǂݍ��ރ��f����



#define	VALUE_MOVE			(4.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(7.0f)							// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(5)								// �v���C���[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p


// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �v���C���[�̍��E�p�[�c�𓮂����Ă���A�j���f�[�^
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
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { -520.0f, PLAYER_OFFSET_Y, -550.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	g_Player.use = TRUE;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�



	// �K�w�A�j���[�V�����p�̏���������
	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;		// �� �����ɐe�̃A�h���X������
		//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
		//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Parts[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Parts[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Parts[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = 0;
	}

	g_Parts[0].use = TRUE;
	g_Parts[0].parent = &g_Player;	// �e���Z�b�g
	g_Parts[0].tblNo = 0;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[0].tblMax = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[0].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[0].model);

	g_Parts[1].use = TRUE;
	g_Parts[1].parent = &g_Player;	// �e���Z�b�g
	g_Parts[1].tblNo = 1;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[1].tblMax = sizeof(move_tbl_larm) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[1].load = 1;
	LoadModel(MODEL_PLAYER_LARM, &g_Parts[1].model);

	g_Parts[2].use = TRUE;
	g_Parts[2].parent = &g_Player;	// �e���Z�b�g
	g_Parts[2].tblNo = 2;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[2].tblMax = sizeof(move_tbl_rarm) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[2].load = 1;
	LoadModel(MODEL_PLAYER_RARM, &g_Parts[2].model);

	g_Parts[3].use = TRUE;
	g_Parts[3].parent = &g_Player;	// �e���Z�b�g
	g_Parts[3].tblNo = 3;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[3].tblMax = sizeof(move_tbl_lleg) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[3].load = 1;
	LoadModel(MODEL_PLAYER_LLEG, &g_Parts[3].model);

	g_Parts[4].use = TRUE;
	g_Parts[4].parent = &g_Player;	// �e���Z�b�g
	g_Parts[4].tblNo = 4;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[4].tblMax = sizeof(move_tbl_rleg) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[4].load = 1;
	LoadModel(MODEL_PLAYER_RLEG, &g_Parts[4].model);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}


	// �p�[�c�̉������
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// �p�[�c�̉������
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();

	// �����O�̍��W��ۑ����Ă����i�ړ����ĂԂ��������͂��̍��W�֖߂��ׁj
	//XMFLOAT3 pos;
	g_Player.oldpos = g_Player.pos;




	// �ړ��������Ⴄ
	if (GetKeyboardPress(DIK_A))
	{	// ���ֈړ�
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_D))
	{	// �E�ֈړ�
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_W))
	{	// ��ֈړ�
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = XM_PI;
	}
	if (GetKeyboardPress(DIK_S))
	{	// ���ֈړ�
		g_Player.spd = VALUE_MOVE;
		g_Player.dir = 0.0f;
	}

	//�Q�[���p�b�h
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


	//	// Key���͂���������ړ���������
	if (g_Player.spd > 0.0f)
	{
		g_Player.rot.y = g_Player.dir + cam->rot.y;

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}

	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// �e���ˏ���
	if (IsMouseLeftTriggered()|| GetKeyboardTrigger(DIK_SPACE))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
		
		XMFLOAT3 pos = cam->pos;   

		XMFLOAT3 direction;
		direction = cam->rot;
		direction.y += 3.14f;
		 
		SetBullet(pos, direction);

		// BGM�Đ�
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

		// BGM�Đ�
		//PlaySound(SOUND_LABEL_BGM_BGMtitle);

	}

	g_Player.spd *= 0.5f;


	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Parts[i].use == TRUE) && (g_Parts[i].tblMax > 0))
		{	// ���`��Ԃ̏���
			int nowNo = (int)g_Parts[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
			INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Parts[i].tblNo];	// �s���e�[�u���̃A�h���X���擾

			XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

			XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
			Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
			Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

			// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
			XMStoreFloat3(&g_Parts[i].pos, nowPos + Pos);

			// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i].rot, nowRot + Rot);

			// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
			XMStoreFloat3(&g_Parts[i].scl, nowScl + Scl);

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[i].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Parts[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			}

		}

	}

	for (int i = 0; i < MAX_WALL; i++)
	{
		// �ړ����I�������wall�Ƃ̓����蔻��
		{

			// wall�̐��������蔻����s��
			WALL* wall = GetWall();

				// �����Ă�wall�Ɠ����蔻�������
			if (wall[i].use == TRUE)
			{
				BOOL ans = CollisionBC( g_Player.pos, wall[i].pos,PLAYER_SIZE, WALL_SIZE);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					g_Player.pos = g_Player.oldpos;
				}
			}

		}

	}

	for (int i = 0; i < MAX_BREAKWALL; i++)
	{
		// �ړ����I�������wall�Ƃ̓����蔻��
		{

			// wall�̐��������蔻����s��
			BREAKWALL* breakwall = GetBreakwall();

			// �����Ă�wall�Ɠ����蔻�������
			if (breakwall[i].use == TRUE)
			{
				BOOL ans = CollisionBC(g_Player.pos, breakwall[i].pos, PLAYER_SIZE, BREAKWALL_SIZE);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					g_Player.pos = g_Player.oldpos;
				}
			}

		}

	}


	{	// �|�C���g���C�g�̃e�X�g
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


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");
	//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{

	if (g_Player.use == FALSE) return;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �܂��͊�{�ƂȂ�a�n�c�x��`�悷��

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	// ������`�悵���Ƃ��ɂ��悤�����}�g���N�X��ۑ����Ă���
	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&g_Player.model);



	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
		if (g_Parts[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_Parts[i].model);

	}


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}
