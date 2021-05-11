//ヘッダーファイルの読み込み
#include"DxLib.h"		//DxLibを使うのに必要
#include"keyboard.h"	//キーボードの処理

//マクロ定義
#define GAME_TITLE "ゲームタイトル"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720
#define GAME_COLOR 32

#define GAME_ICON_ID	333

#define GAME_WINDOW_BAR		0

//プログラムはWinMainから始まります
//Windowsのプログラミング方法＝（WinAPI）で動いている
//DxLibは、DirectXという、ゲーミングプログラミングを簡単に使える仕組み
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)

{
	SetOutApplicationLogValidFlag(FALSE);					//log.txtを出力しない
	ChangeWindowMode(TRUE);									//ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);							//ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);		//ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);					//ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);						//デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);							//アイコンファイルを読込
	SetWindowStyleMode(GAME_WINDOW_BAR);					//ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);									//ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);									//ウィンドウをずっとアクティブにする

	//DxLibの初期化
	if (DxLib_Init() == -1)			//DXライブラリの初期化処理
	{
		return -1;					//エラーが起きたら直ちに終了
	}

	

	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	//円の中心
	int X = (GAME_WIDTH / 2);
	int Y = (GAME_HEIGHT / 2);
	//円の半径
	int radius = 50;

	//無限ループ
	while (1)
	{

		if (ProcessMessage() != 0) { break; }

		//画面を消去する
		if (ClearDrawScreen() != 0) { break; }

		//キーボード入力の更新
		AllKeyUpdate();

		//キー入力
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


		ScreenFlip();		//ダブルバッファリングした画面を描画

	}

	//DxLib使用の終了処理
	DxLib_End();

	return 0;				//ソフトの終了
}