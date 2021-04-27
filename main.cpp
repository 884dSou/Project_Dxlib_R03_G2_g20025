//�w�b�_�[�t�@�C���̓ǂݍ���
#include"DxLib.h"	//DxLib���g���̂ɕK�v

//�}�N����`
#define GAME_TITLE "�Q�[���^�C�g��"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720
#define GAME_COLOR 32

#define GAME_ICON_ID	333

//�v���O������WinMain����n�܂�܂�
//Windows�̃v���O���~���O���@���iWinAPI�j�œ����Ă���
//DxLib�́ADirectX�Ƃ����A�Q�[�~���O�v���O���~���O���ȒP�Ɏg����d�g��
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)

{
	ChangeWindowMode(TRUE);
	SetMainWindowText(GAME_TITLE);
	SetGraphMode(GAME_HEIGHT, GAME_HEIGHT, GAME_COLOR);
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);
	SetBackgroundColor(255, 255, 255);
	SetWindowIconID(GAME_ICON_ID);

	//DxLib�̏�����
	if(DxLib_Init() == -1)			//DX���C�u�����̏���������
	{
		return -1;					//�G���[���N�����璼���ɏI��
	}
	//
	while(1)
	{

		if (CheckHitKeyAll() != 0)
		{
			break;
		}

		if (ProcessMessage() != 0)
		{
			break;
		}
	}

	//DxLib�g�p�̏I������
	DxLib_End();

	return 0;				//�\�t�g�̏I��
}