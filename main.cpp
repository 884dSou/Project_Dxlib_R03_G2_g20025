
//ヘッダファイルの読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include"keyboard.h"	//キーボードの処理
#include"FPS.h"			//FPSの処理

//構造体

//画像の構造体
struct IMAGE
{
	int handle = -1;	//画像のハンドル（管理番号）
	char path[255];		//画像の場所（パス）

	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ

	BOOL IsDraw = FALSE;	//画像が描画できる？
};


//キャラクタの構造体
struct CHARCTOR
{
	IMAGE img;

	RECT coll;			//当たり判定の領域（四角）
	int speed =1;
	int scean = 0;		//当たったものによって変わる
};

//動画の構造体
struct MOVIE
{
	IMAGE img;

	int Volume = 255;	//ボリューム（最小）1〜255（最大）
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;
	char path[255];

	int Volume = -1;	//ボリューム（MIN 0〜255 MAX）
	int playType = -1;
};

//シーンを管理する変数
GAME_SCEAEN GameScene;				//現在のゲームシーン
GAME_SCEAEN OldGameScene;			//前回のゲームシーン
GAME_SCEAEN NextGameScene;			//次のゲームシーン

//プレイ背景の動画
MOVIE playMovie;

//プレイヤー
CHARCTOR player;

//敵（障害物）
CHARCTOR enemy;

//ゴール
CHARCTOR gaol;

//画像の読み込み
IMAGE TitleLogo;
IMAGE TitleEnter;
IMAGE EndClear;
IMAGE EndOver;

//音楽
AUDIO TitleBGM;
AUDIO PlayBGM;
AUDIO EndBGM;

AUDIO playerSE;

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

//ClickEnter
int ClickEnterCnt = 0;
int ClickEnterCntMax = 60;
BOOL ClickEnterBrink = FALSE;

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

VOID CollUpdatePlayer(CHARCTOR* chara);	//当たり判定の領域を更新
VOID CollUpdate(CHARCTOR* chara);	//当たり判定

BOOL OnCollrect(RECT a, RECT b);	//矩形と矩形の当たり判定

BOOL GameLoad(VOID);	//ゲームデータの読み込み
VOID GameInit(VOID);

BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);	//音楽の読み込み
BOOL LoadImg(IMAGE* img, const char* path);	//画像の読み込み

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
	if (!GameLoad())
	{
		//ゲームのデータを読み込み
		DxLib_End();	//DxLib終了
		return -1;		//異常終了
	}

	//ゲームの初期化
	GameInit();

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
	DeleteGraph(playMovie.img.handle);
	DeleteGraph(player.img.handle);
	DeleteGraph(gaol.img.handle);
	DeleteGraph(enemy.img.handle);

	DeleteSoundMem(TitleBGM.handle);
	DeleteSoundMem(PlayBGM.handle);
	DeleteSoundMem(EndBGM.handle);
	
	DeleteSoundMem(playerSE.handle);

	DeleteGraph(TitleLogo.handle);
	DeleteGraph(TitleEnter.handle);
	DeleteGraph(EndClear.handle);
	DeleteGraph(EndOver.handle);

	//DxLib使用の終了処理
	DxLib_End();

	return 0;				//ソフトの終了
}

/// <summary>
/// ゲームのデータを読み込み
/// </summary>
/// <returns>読み込めたらTRUE/読み込めなかったらFALSE</returns>
BOOL GameLoad()
{
	//プレイ動画の背景を読み込み
	strcpyDx(playMovie.img.path, ".\\movie\\playMovie.mp4");
	playMovie.img.handle = LoadGraph(playMovie.img.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (playMovie.img.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			playMovie.img.path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		return FALSE;		//エラー終了
	}

	//画像の幅と高さを取得
	GetGraphSize(playMovie.img.handle, &playMovie.img.width, &playMovie.img.height);

	//動画のボリューム
	playMovie.Volume = 0;

	//画像の読み込み
	if (!LoadImg(&player.img ,".\\image\\player.png")) { return FALSE; }	//プレイヤーの画像を読み込み
	if (!LoadImg(&gaol.img , ".\\image\\gaol.png")) { return FALSE; }	//ゴールの画像を読み込み
	if (!LoadImg(&enemy.img , ".\\image\\enemy.png")) { return FALSE; }	//障害物の画像を読み込み

	//ロゴの読み込み
	if (!LoadImg(&TitleLogo, ".\\image\\TitleLogo.png")) { return FALSE; }
	if (!LoadImg(&TitleEnter, ".\\image\\TitleEnter.png")) { return FALSE; }
	if (!LoadImg(&EndClear, ".\\image\\End.png")) { return FALSE; }
	if (!LoadImg(&EndOver, ".\\image\\EndOver.png")) { return FALSE; }

	//音楽を読み込む
	if (!LoadAudio(&TitleBGM, ".\\audio\\chiisanaomochabako.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayBGM, ".\\audio\\natsuyasuminotanken.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&EndBGM, ".\\audio\\hiyokonokakekko.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }

	if (!LoadAudio(&playerSE, ".\\audio\\畳の上を走る.mp3", 255, DX_PLAYTYPE_BACK)) { return FALSE; }

	return TRUE;
}

/// <summary>
/// ゲームデータを初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	//プレイヤーを初期化
	player.img.x = 10;
	player.img.y = GAME_HEIGHT - player.img.height - 10;
	player.speed = 500;
	player.img.IsDraw = TRUE;

	//当たり判定を更新する
	CollUpdatePlayer(&player);

	//障害物の初期化
	enemy.img.x = GAME_WIDTH / 2;
	enemy.img.y = GAME_HEIGHT / 2 - enemy.img.width;
	enemy.speed = 500;
	enemy.img.IsDraw = TRUE;

	//当たり判定の更新
	CollUpdate(&enemy);

	//ゴールを初期化
	gaol.img.x = GAME_WIDTH - gaol.img.width - 10;
	gaol.img.y = 10;
	gaol.speed = 500;
	gaol.img.IsDraw = TRUE;

	//当たり判定を更新する
	CollUpdate(&gaol);

	//タイトルロゴの位置を決める
	TitleLogo.x = GAME_WIDTH / 2 - TitleLogo.width / 2;
	TitleLogo.y = 65;

	//クリックエンターの位置
	TitleEnter.x = GAME_WIDTH / 2 - TitleEnter.width / 2;
	TitleEnter.y = GAME_HEIGHT - TitleEnter.height - 100;

	//クリアの位置
	EndClear.x = GAME_WIDTH / 2 - EndClear.width / 2;
	EndClear.y = GAME_HEIGHT / 2 - EndClear.height / 2;
	
	//ゲームオーバーの位置
	EndOver.x = GAME_WIDTH / 2 - EndOver.width / 2;
	EndOver.y = GAME_HEIGHT / 2 - EndOver.height / 2;

	//クリックエンターの設定
	ClickEnterCnt = 0;
	ClickEnterCntMax = 30;
	ClickEnterBrink = FALSE;
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
		//BGMをとめる
		StopSoundMem(TitleBGM.handle);

		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	//BGMが流れていないとき
	if (CheckSoundMem(TitleBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(TitleBGM.handle, TitleBGM.playType);
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawGraph(TitleLogo.x, TitleLogo.y, TitleLogo.handle, TRUE);
	
	if (ClickEnterCnt < ClickEnterCntMax) { ClickEnterCnt++; }
	else
	{
		if (ClickEnterBrink == TRUE)ClickEnterBrink = FALSE;
		else if (ClickEnterBrink == FALSE)ClickEnterBrink = TRUE;
		
		ClickEnterCnt = 0;
	}

	if(ClickEnterBrink == TRUE)
	{
		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);
	}

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
	//if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	//{
	//	//シーン切り替え

	//	//エンド画面に切り替え
	//	ChangeScene(GAME_SCENE_END);
	//}

	//BGMが流れていないとき
	if (CheckSoundMem(PlayBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(PlayBGM.handle, PlayBGM.playType);
	}

	//プレイヤーの操作
	if (KeyDown(KEY_INPUT_W) == TRUE && player.coll.top > 0)
	{
		player.img.y -= player.speed * fps.DeltaTime;
		
		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}
	if (KeyDown(KEY_INPUT_S) == TRUE && player.coll.bottom < GAME_HEIGHT)
	{
		player.img.y += player.speed * fps.DeltaTime;

		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}
	if (KeyDown(KEY_INPUT_A) == TRUE && player.coll.left > 0)
	{
		player.img.x -= player.speed * fps.DeltaTime;

		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}
	if (KeyDown(KEY_INPUT_D) == TRUE && player.coll.right < GAME_WIDTH)
	{
		player.img.x += player.speed * fps.DeltaTime;
		
		//足音が流れていないとき
		if (CheckSoundMem(playerSE.handle) == 0)
		{
			//動く時の効果音
			PlaySoundMem(playerSE.handle, playerSE.playType);
		}
	}

	//当たり判定を更新する
	CollUpdatePlayer(&player);
	
	//障害物の判定を更新する
	CollUpdate(&enemy);

	//当たり判定を更新する
	CollUpdate(&gaol);

	//プレイヤーがゴールに当たったとき
	if (OnCollrect(player.coll, gaol.coll) == TRUE)
	{
		//BGMをとめる
		StopSoundMem(PlayBGM.handle);

		//クリアした時
		player.scean = 0;

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
		return;		//処理を強制終了
	}
	
	//プレイヤーがゴールに当たったとき
	if (OnCollrect(player.coll, enemy.coll) == TRUE)
	{
		//BGMをとめる
		StopSoundMem(PlayBGM.handle);

		//クリアした時
		player.scean = 1;

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
		return;		//処理を強制終了
	}

	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//動画を描画
	if (GetMovieStateToGraph(playMovie.img.handle) == 0)
	{
		//再生する
		SeekMovieToGraph(playMovie.img.handle, 0);	//シークバーを最初に戻す
		PlayMovieToGraph(playMovie.img.handle);
	}
	//動画を描画(画面を引き延ばす)
	DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, playMovie.img.handle, TRUE);


	//ゴールを描画
	if (gaol.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(gaol.img.x, gaol.img.y, gaol.img.handle, TRUE);

		//デバッグの時は、当たり判定領域を描画
		if (Game_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(gaol.coll.left, gaol.coll.top, gaol.coll.right, gaol.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}
	
	//障害物を描画
	if (enemy.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(enemy.img.x, enemy.img.y, enemy.img.handle, TRUE);

		//デバッグの時は、当たり判定領域を描画
		if (Game_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(enemy.coll.left, enemy.coll.top, enemy.coll.right, enemy.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}

	//プレイヤーを描画
	if (player.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(player.img.x, player.img.y, player.img.handle, TRUE);

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
		//BGMをとめる
		StopSoundMem(EndBGM.handle);

		//シーン切り替え

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}

	//BGMが流れていないとき
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		switch (player.scean)
		{
		case 0:
			//BGMを流す
			PlaySoundMem(EndBGM.handle, EndBGM.playType);
			break;
		default:
			break;
		}
	}

	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	switch (player.scean)
	{
	case 0:
		DrawGraph(EndClear.x, EndClear.y, EndClear.handle, TRUE);
		break;
	default:
		DrawGraph(EndOver.x, EndOver.y, EndOver.handle, TRUE);
		break;
	}
	
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
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width-55;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARCTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// オブジェクトの衝突の確認
/// </summary>
/// <param name="a">当たり判定の領域（a）</param>
/// <param name="b">当たり判定の領域（b）</param>
/// <returns>当たったら「TRUE」当たらなかったら「FALSE」</returns>
BOOL OnCollrect(RECT a, RECT b)
{
	if (
		a.left < b.right &&
		a.right > b.left &&
		a.top < b.bottom &&
		a.bottom > b.top
		)
	{
		//当たっているとき
		return TRUE;
	}
	else
	{
		//当たってないとき
		return FALSE;
	}
}

/// <summary>
/// 音楽の読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//音楽の読み込み
	strcpyDx(audio->path,path);
	audio->handle = LoadSoundMem(audio->path);

	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),
			audio->path,
			"音楽読み込みエラー",
			MB_OK
		);

		return FALSE;
	}

	//その他設定
	audio->Volume = volume;
	audio->playType = playType;

	return TRUE;
}


/// <summary>
/// 画像の読み込み
/// </summary>
/// <param name="img">構造体のアドレス</param>
/// <param name="path">画像のパス</param>
/// <returns></returns>
BOOL LoadImg(IMAGE* img,const char* path)
{
	strcpyDx(img->path,path );
	img->handle = LoadGraph(img->path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (img->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			img->path,			//メッセージタイトル
			"画像読み込みエラー！",	//メッセージ本文
			MB_OK					//ボタン
		);

		return FALSE;		//エラー終了
	}
	
	GetGraphSize(img->handle, &img->width, &img->height);

	return TRUE;
}