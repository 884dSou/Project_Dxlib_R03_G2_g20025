
//ヘッダファイルの読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include"keyboard.h"	//キーボードの処理
#include"FPS.h"			//FPSの処理

//構造体

//キャラクタの構造体
struct CHARCTOR
{
	int handle = -1;	//画像のハンドル（管理番号）
	char path[255];		//画像の場所（パス）
	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ
	int speed = 3;		//移動速度

	RECT coll;			//当たり判定の領域（四角）
	BOOL IsDraw = FALSE;	//画像が描画できる？
};


//シーンを管理する変数
GAME_SCEAEN GameScene;				//現在のゲームシーン
GAME_SCEAEN OldGameScene;			//前回のゲームシーン
GAME_SCEAEN NextGameScene;			//次のゲームシーン

//プレイヤー
CHARCTOR player;

//ゴール
CHARCTOR gaol;

//画面の切り替え
BOOL IsFadeOut = FALSE;		//フェードアウト
BOOL IsFadeIn = FALSE;		//フェードイン

//シーン切り替え
int fadeTimeMill = 2000;						//切り替えミリ秒
int fadeTimeMax = fadeTimeMill / 1000 * 60;		//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期値
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMax = fadeTimeMax;	//フェードアウトのカウンタMAX

//フェードアウト
int fadeInCntInit = fadeTimeMax;	//初期値
int fadeInCnt = fadeInCntInit;		//フェードインのカウンタ
int fadeInCntMax = 0;				//フェードインのカウンタMAX

//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);	//タイトル画面（処理）
VOID TitleDraw(VOID);	//タイトル画面（描画）

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);	//プレイ画面（処理）
VOID PlayDraw(VOID);	//プレイ画面（描画）

VOID End(VOID);			//エンド画面
VOID EndProc(VOID);		//エンド画面（処理）
VOID EndDraw(VOID);		//エンド画面（描画）

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);	//切り替え画面（処理）
VOID ChangeDraw(VOID);	//切り替え画面（描画）

VOID ChangeScene(GAME_SCEAEN scene);	//シーン切り替え

VOID CollUpdatePlayer(CHARCTOR* chara);
VOID CollUpdate(CHARCTOR* chara);

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


	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化

	//プレイヤーの画像を読み込み
	strcpyDx(player.path, ".\\image\\player.png");
	player.handle = LoadGraph(player.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (player.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			player.path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		DxLib_End();	//強制終了
		return -1;		//エラー終了
	}

	//画像の幅と高さを取得
	GetGraphSize(player.handle, &player.width, &player.height);

	//当たり判定を更新する
	CollUpdatePlayer(&player);

	//プレイヤーを初期化
	player.x = GAME_WIDTH / 2 - player.width / 2;
	player.y = GAME_HEIGHT / 2 - player.height / 2;
	player.speed = 500;
	player.IsDraw = TRUE;

	//ゴールの画像を読み込み
	strcpyDx(gaol.path, ".\\image\\gaol.png");
	gaol.handle = LoadGraph(gaol.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (gaol.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			gaol.path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		DxLib_End();	//強制終了
		return -1;		//エラー終了
	}

	//画像の幅と高さを取得
	GetGraphSize(gaol.handle, &gaol.width, &gaol.height);

	//当たり判定を更新する
	CollUpdate(&gaol);

	//ゴールを初期化
	gaol.x = GAME_WIDTH - gaol.width ;
	gaol.y = 0;
	gaol.speed = 500;
	gaol.IsDraw = TRUE;

	//無限ループ
	while (1)
	{

		if (ProcessMessage() != 0) { break; }

		//画面を消去する
		if (ClearDrawScreen() != 0) { break; }

		//キーボード入力の更新
		AllKeyUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//以前のシーンを取得
		if (GameScene != GAME_SCEAEN_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//タイトル画面
			break;
		case GAME_SCENE_PLAY:
			Play();				//プレイ画面
			break;
		case GAME_SCENE_END:
			End();				//エンド画面
			break;
		case GAME_SCEAEN_CHANGE:
			Change();			//切り替え画面
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え画面でない時
			if (GameScene != GAME_SCEAEN_CHANGE)
			{
				NextGameScene = GameScene;	//次のシーンを保存
				GameScene = GAME_SCEAEN_CHANGE;
			}
		}


		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();		//ダブルバッファリングした画面を描画

	}

	//終わるときの処理
	DeleteGraph(player.handle);
	DeleteGraph(gaol.handle);

	//DxLib使用の終了処理
	DxLib_End();

	return 0;				//ソフトの終了
}


//▼▼▼▼▼▼▼▼▼▼タイトル画面▼▼▼▼▼▼▼▼▼▼▼

/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCEAEN scene)
{
		GameScene = scene;
		IsFadeIn = FALSE;
		IsFadeOut = TRUE;

		return;
}

/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();		//処理
	TitleDraw();		//描画
	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}

//▼▼▼▼▼▼▼▼▼▼▼プレイ画面▼▼▼▼▼▼▼▼▼▼▼
/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();		//処理
	PlayDraw();		//描画
	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーン切り替え

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
	}

	//プレイヤーの操作
	if (KeyDown(KEY_INPUT_W) == TRUE)
	{
		player.y -= player.speed * fps.DeltaTime;
	}
	if (KeyDown(KEY_INPUT_S) == TRUE)
	{
		player.y += player.speed * fps.DeltaTime;
	}
	if (KeyDown(KEY_INPUT_A) == TRUE)
	{
		player.x -= player.speed * fps.DeltaTime;
	}
	if (KeyDown(KEY_INPUT_D) == TRUE)
	{
		player.x += player.speed * fps.DeltaTime;
	}

	//当たり判定を更新する
	CollUpdatePlayer(&player);

	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//ゴールを描画
	if (gaol.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(gaol.x, gaol.y, gaol.handle, TRUE);

		//デバッグの時は、当たり判定領域を描画
		if (Game_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(gaol.coll.left, gaol.coll.top, gaol.coll.right, gaol.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}

	//プレイヤーを描画
	if (player.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(player.x, player.y, player.handle, TRUE);

		//デバッグの時は、当たり判定領域を描画
		if (Game_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(player.coll.left, player.coll.top, player.coll.right, player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}
	
	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}

//▼▼▼▼▼▼▼▼▼▼エンド画面▼▼▼▼▼▼▼▼▼▼▼
/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();		//処理
	EndDraw();		//描画
	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーン切り替え

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);
	}
	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	return;
}

//▼▼▼▼▼▼▼▼▼▼切り替え画面▼▼▼▼▼▼▼▼▼▼
/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();		//処理
	ChangeDraw();		//描画
	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	//フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;		//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;		//カウンタを減らす
		}
		else
		{
			//フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;			//フェードアウト処理終了
		}
	}

	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトしていないとき
		GameScene = NextGameScene;		//次のシーンに切り替え
		OldGameScene = GameScene;		//以前のゲームシーン更新
	}

	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();			//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();				//プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();				//エンド画面の描画
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMax) * 255);
	}
	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 0, "切り替え画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 当たり判定の領域更新（プレイヤー）
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdatePlayer(CHARCTOR* chara)
{
	chara->coll.left = chara->x;
	chara->coll.top = chara->y;
	chara->coll.right = chara->x + chara->width-55;
	chara->coll.bottom = chara->y + chara->height;

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARCTOR* chara)
{
	chara->coll.left = chara->x;
	chara->coll.top = chara->y;
	chara->coll.right = chara->x + chara->width;
	chara->coll.bottom = chara->y + chara->height;

	return;
}