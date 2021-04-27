//ヘッダーファイルの読み込み
#include"DxLib.h"	//DxLibを使うのに必要

//マクロ定義
#define GAME_TITLE "ゲームタイトル"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720
#define GAME_COLOR 32

#define GAME_ICON_ID	333

//プログラムはWinMainから始まります
//Windowsのプログラミング方法＝（WinAPI）で動いている
//DxLibは、DirectXという、ゲーミングプログラミングを簡単に使える仕組み
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

	//DxLibの初期化
	if(DxLib_Init() == -1)			//DXライブラリの初期化処理
	{
		return -1;					//エラーが起きたら直ちに終了
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

	//DxLib使用の終了処理
	DxLib_End();

	return 0;				//ソフトの終了
}