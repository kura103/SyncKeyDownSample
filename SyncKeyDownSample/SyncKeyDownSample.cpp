// TypeSync.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "SyncKeyDown.h"
#include <iostream>

int main()
{
	bool bLoop = true;
	SyncKeyDown skd;
	while (bLoop) {
		//ボタンチェック
		skd.CheckKeyDown();

		//押したボタン表示
		for (int i = 'A'; i <= 'Z'; i++) {
			char input = i;
			if (skd.GetKeyDown(input)) {
				std::cout << input << std::endl;
				if (input == 'Q') {//アプリ終了
					bLoop = false;
				}
			}
		}
		Sleep(10);
	}
    return 0;
}

