//FPSのソースファイル

//ヘッダファイル読込
#include "DxLib.h"
#include "FPS.h"

//グローバル変数
extern FPS fps;

//関数

/// <summary>
/// FPS値を計算し、値を更新する
/// </summary>
/// <param name=""></param>
VOID FPSUpdate(VOID)
{
	//一番最初の処理
	if (FALSE == fps.IsInitFlg)
	{
		//測定開始時刻をマイクロ秒単位で取得
		fps.StartTime = GetNowHiPerformanceCount();	//windousが起動してから経過した時間（マイクロ秒）

		fps.IsInitFlg = TRUE;		//フラグを立てる
	}

	//
	fps.NowTime = GetNowHiPerformanceCount();

	//
	fps.DeltaTime = (fps.NowTime - fps.OldTime) / 1000000.0f;

	//今回取得した時間を保存
	fps.OldTime = fps.NowTime;

	//１フレーム目～FPS設定値まではカウントアップ
	if (fps.Count < fps.SampleRate)
	{
		//カウンタを増やす
		fps.Count++;
	}
	else
	{
		//FPS設定値のフレームで平均FPSを計算

		//現在の時刻から、０フレーム目の時間を引き、FPSの数値で割る
		//現在の平均FPS値が出る
		fps.DrawValue = 1000000.0f / ((fps.StartTime) / (float)fps.SampleRate);

		//測定開始時刻をマイクロ秒単位で取得
		fps.StartTime = GetNowHiPerformanceCount();

		//カウンタの初期化
		fps.Count = 0;
	}
	return;
}

/// <summary>
/// 平均FPS値を描画する
/// </summary>
/// <param name=""></param>
VOID FPSDraw(VOID)
{
	//文字列を描画
	DrawFormatString(0, 20, GetColor(0, 0, 0), "FPS;%.lf", fps.DrawValue);

	return;
}

/// <summary>
/// FPSで処理を待つ
/// </summary>
/// <param name=""></param>
VOID FPSWait(VOID)
{
	//現在の時刻　最初の時刻で、現在かかっている時刻を取得する
	LONGLONG resultTime = fps.NowTime = fps.StartTime;
	
	//待つべきミリ秒数(１秒/FPS値　＊　現在のフレーム数)から、現在かかっている時刻を引く
	int waitTime = 1000000.0f / fps.Value * fps.Count - resultTime;

	//マイクロ秒からミリ秒に変換
	waitTime /= 1000.0f;

	//処理が早かったら、処理を待つ
	if(waitTime > 0)
	{
		WaitTimer(waitTime);
	}

	//垂直同期をOFFにしているか？
	if (GetWaitVSyncFlag() == FALSE)
	{
		//FPS最大値ではないとき
		if (fps.Value < GAME_FPS_MAX)
		{
			//１秒枚のFPS値よりも、持つ時間が小さいときは、もっとFPS値を上げてもよい
			//待つ時間10ミリ　<=　1秒/60FPS　=　16.6666ミリ　もう少し早くできる
			if (waitTime > 0 && waitTime <= 1000.0f / fps.Value)
			{
				fps.Count++;
			}
			else
			{
				//FPS値が追いついておらず、遅いときは、FPS値を下げる
				if (fps.Value > GAME_FPS_MIN)
				{
					fps.Value--;
				}
			}
		}
	}
}