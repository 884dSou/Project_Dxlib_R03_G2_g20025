//�w�b�_�[�t�@�C���̓ǂݍ���
#include"DxLib.h"		//DxLib���g���̂ɕK�v
#include"keyboard.h"	//�L�[�{�[�h�̏���

//�}�N����`
#define GAME_TITLE "�Q�[���^�C�g��"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720
#define GAME_COLOR 32

#define GAME_ICON_ID	333

#define GAME_WINDOW_BAR		0

//�񋓌^
enum GAME_SCEAEN {
	GAME_SCENE_TITLE,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCEAEN_CHANGE
};		//�Q�[���̃V�[��

//�V�[�����Ǘ�����ϐ�
GAME_SCEAEN GameScene;				//���݂̃Q�[���V�[��
GAME_SCEAEN OldGameScene;			//�O��̃Q�[���V�[��
GAME_SCEAEN NextGameScene;			//���̃Q�[���V�[��

//��ʂ̐؂�ւ�
BOOL IsFadeOut = FALSE;		//�t�F�[�h�A�E�g
BOOL IsFadeIn = FALSE;		//�t�F�[�h�C��

//�V�[���؂�ւ�
int fadeTimeMill = 2000;						//�؂�ւ��~���b
int fadeTimeMax = fadeTimeMill / 1000 * 60;		//�~���b���t���[���b�ɕϊ�

//�t�F�[�h�A�E�g
int fadeOutCntInit = 0;				//�����l
int fadeOutCnt = fadeOutCntInit;	//�t�F�[�h�A�E�g�̃J�E���^
int fadeOutCntMax = fadeTimeMax;	//�t�F�[�h�A�E�g�̃J�E���^MAX

//�t�F�[�h�A�E�g
int fadeInCntInit = fadeTimeMax;	//�����l
int fadeInCnt = fadeInCntInit;		//�t�F�[�h�C���̃J�E���^
int fadeInCntMax = 0;				//�t�F�[�h�C���̃J�E���^MAX

//�v���g�^�C�v�錾
VOID Title(VOID);		//�^�C�g�����
VOID TitleProc(VOID);	//�^�C�g����ʁi�����j
VOID TitleDraw(VOID);	//�^�C�g����ʁi�`��j

VOID Play(VOID);		//�v���C���
VOID PlayProc(VOID);	//�v���C��ʁi�����j
VOID PlayDraw(VOID);	//�v���C��ʁi�`��j

VOID End(VOID);			//�G���h���
VOID EndProc(VOID);		//�G���h��ʁi�����j
VOID EndDraw(VOID);		//�G���h��ʁi�`��j

VOID Change(VOID);		//�؂�ւ����
VOID ChangeProc(VOID);	//�؂�ւ���ʁi�����j
VOID ChangeDraw(VOID);	//�؂�ւ���ʁi�`��j

VOID ChangeScene(GAME_SCEAEN scene);	//�V�[���؂�ւ�

//�v���O������WinMain����n�܂�܂�
//Windows�̃v���O���~���O���@���iWinAPI�j�œ����Ă���
//DxLib�́ADirectX�Ƃ����A�Q�[�~���O�v���O���~���O���ȒP�Ɏg����d�g��
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)

{
	SetOutApplicationLogValidFlag(FALSE);					//log.txt���o�͂��Ȃ�
	ChangeWindowMode(TRUE);									//�E�B���h�E���[�h�ɐݒ�
	SetMainWindowText(GAME_TITLE);							//�E�B���h�E�̃^�C�g���̕���
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);		//�E�B���h�E�̉𑜓x��ݒ�
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);					//�E�B���h�E�̑傫����ݒ�
	SetBackgroundColor(255, 255, 255);						//�f�t�H���g�̔w�i�̐F
	SetWindowIconID(GAME_ICON_ID);							//�A�C�R���t�@�C����Ǎ�
	SetWindowStyleMode(GAME_WINDOW_BAR);					//�E�B���h�E�o�[�̏��
	SetWaitVSyncFlag(TRUE);									//�f�B�X�v���C�̐���������L���ɂ���
	SetAlwaysRunFlag(TRUE);									//�E�B���h�E�������ƃA�N�e�B�u�ɂ���

	//DxLib�̏�����
	if (DxLib_Init() == -1)			//DX���C�u�����̏���������
	{
		return -1;					//�G���[���N�����璼���ɏI��
	}

	

	//�_�u���o�b�t�@�����O�L����
	SetDrawScreen(DX_SCREEN_BACK);

	//�~�̒��S
	int X = (GAME_WIDTH / 2);
	int Y = (GAME_HEIGHT / 2);
	//�~�̔��a
	int radius = 50;

	//�ŏ��̃V�[���́A�^�C�g����ʂ���
	GameScene = GAME_SCENE_TITLE;

	//�������[�v
	while (1)
	{

		if (ProcessMessage() != 0) { break; }

		//��ʂ���������
		if (ClearDrawScreen() != 0) { break; }

		//�L�[�{�[�h���͂̍X�V
		AllKeyUpdate();

		//ESC�L�[�ŋ����I��
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//�ȑO�̃V�[�����擾
		if (GameScene != GAME_SCEAEN_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//�V�[�����Ƃɏ������s��
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//�^�C�g�����
			break;
		case GAME_SCENE_PLAY:
			Play();				//�v���C���
			break;
		case GAME_SCENE_END:
			End();				//�G���h���
			break;
		case GAME_SCEAEN_CHANGE:
			Change();			//�؂�ւ����
			break;
		default:
			break;
		}

		//�V�[����؂�ւ���
		if (OldGameScene != GameScene)
		{
			//���݂̃V�[�����؂�ւ���ʂłȂ���
			if (GameScene != GAME_SCEAEN_CHANGE)
			{
				NextGameScene = GameScene;	//���̃V�[����ۑ�
				GameScene = GAME_SCEAEN_CHANGE;
			}
		}

		//�L�[����
		if (KeyDown(KEY_INPUT_W) == TRUE && Y > radius)
		{
			Y-=2;
		}
		if (KeyDown(KEY_INPUT_S) == TRUE && Y< GAME_HEIGHT - radius)
		{
			Y+=2;
		}
		if (KeyDown(KEY_INPUT_A) == TRUE && X > radius)
		{
			X-=2;
		}
		if (KeyDown(KEY_INPUT_D) == TRUE && X < GAME_WIDTH - radius)
		{
			X+=2;
		}

		DrawCircle(X, Y, radius, GetColor(255, 0, 0), TRUE);


		ScreenFlip();		//�_�u���o�b�t�@�����O������ʂ�`��

	}

	//DxLib�g�p�̏I������
	DxLib_End();

	return 0;				//�\�t�g�̏I��
}


//���������������������^�C�g����ʁ���������������������

/// <summary>
/// �V�[����؂�ւ���֐�
/// </summary>
/// <param name="scene">�V�[��</param>
VOID ChangeScene(GAME_SCEAEN scene)
{
		GameScene = scene;
		IsFadeIn = FALSE;
		IsFadeOut = TRUE;

		return;
}

/// <summary>
/// �^�C�g�����
/// </summary>
VOID Title(VOID)
{
	TitleProc();		//����
	TitleDraw();		//�`��
	return;
}

/// <summary>
/// �^�C�g����ʂ̏���
/// </summary>
VOID TitleProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//�V�[���؂�ւ�
		//���̃V�[���̏������������ōs���Ɗy

		//�v���C��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_PLAY);
	}

	return;
}

/// <summary>
/// �^�C�g����ʂ̕`��
/// </summary>
VOID TitleDraw(VOID)
{
	DrawString(0, 0, "�^�C�g�����", GetColor(0, 0, 0));
	return;
}

//�����������������������v���C��ʁ���������������������
/// <summary>
/// �v���C���
/// </summary>
VOID Play(VOID)
{
	PlayProc();		//����
	PlayDraw();		//�`��
	return;
}

/// <summary>
/// �v���C��ʂ̏���
/// </summary>
VOID PlayProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//�V�[���؂�ւ�

		//�G���h��ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_END);
	}
	return;
}

/// <summary>
/// �v���C��ʂ̕`��
/// </summary>
VOID PlayDraw(VOID)
{
	DrawString(0, 0, "�v���C���", GetColor(0, 0, 0));
	return;
}

//���������������������G���h��ʁ���������������������
/// <summary>
/// �G���h���
/// </summary>
VOID End(VOID)
{
	EndProc();		//����
	EndDraw();		//�`��
	return;
}

/// <summary>
/// �G���h��ʂ̏���
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//�V�[���؂�ւ�

		//�^�C�g����ʂɐ؂�ւ�
		ChangeScene(GAME_SCENE_TITLE);
	}
	return;
}

/// <summary>
/// �G���h��ʂ̕`��
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "�G���h���", GetColor(0, 0, 0));
	return;
}

//���������������������؂�ւ���ʁ�������������������
/// <summary>
/// �؂�ւ����
/// </summary>
VOID Change(VOID)
{
	ChangeProc();		//����
	ChangeDraw();		//�`��
	return;
}

/// <summary>
/// �؂�ւ���ʂ̏���
/// </summary>
VOID ChangeProc(VOID)
{
	//�t�F�[�h�C��
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;		//�J�E���^�����炷
		}
		else
		{
			//�t�F�[�h�C���������I�����

			fadeInCnt = fadeInCntInit;	//�J�E���^������
			IsFadeIn = FALSE;			//�t�F�[�h�C�������I��
		}
	}

	//�t�F�[�h�A�E�g
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;		//�J�E���^�����炷
		}
		else
		{
			//�t�F�[�h�A�E�g�������I�����

			fadeOutCnt = fadeOutCntInit;	//�J�E���^������
			IsFadeOut = FALSE;			//�t�F�[�h�A�E�g�����I��
		}
	}

	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//�t�F�[�h�C�����Ă��Ȃ��A�t�F�[�h�A�E�g���Ă��Ȃ��Ƃ�
		GameScene = NextGameScene;		//���̃V�[���ɐ؂�ւ�
		OldGameScene = GameScene;		//�ȑO�̃Q�[���V�[���X�V
	}

	return;
}

/// <summary>
/// �؂�ւ���ʂ̕`��
/// </summary>
VOID ChangeDraw(VOID)
{
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();			//�^�C�g����ʂ̕`��
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();				//�v���C��ʂ̕`��
		break;
	case GAME_SCENE_END:
		EndDraw();				//�G���h��ʂ̕`��
		break;
	default:
		break;
	}

	//�t�F�[�h�C��
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMax) * 255);
	}
	//�t�F�[�h�A�E�g
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);
	}

	//�l�p��`��
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//�������I��
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 0, "�؂�ւ����", GetColor(0, 0, 0));
	return;
}

