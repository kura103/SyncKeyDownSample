/*
Copyright (c) 2022 kuratomi.masaru

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および/または販売する権利、およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。

*/

#pragma once

#include <windows.h>
class SyncKeyDown
{
	bool	isSync = false;
	int		idx;
	HANDLE	handleMtx = NULL;		//1番目起動判定用ミューテックスハンドル
	HANDLE	handleSharedMem = NULL;
	char*	pMemory = NULL;
	const int dwSharedMemorySize = 1024; //同時起動数によって調整する
	const int keyDataStart = 10;
	const int keySpace = VK_F24 + 1;
public:
	//コンストラクタ
	SyncKeyDown(const wchar_t* inputKeyName = L"SyncKeyDown", const wchar_t* inputMutexKeyName = L"SyncKeyDownMutex") {

		//メモリ上にマッピングオブジェクトの作成
		handleSharedMem = CreateFileMapping(NULL, NULL, PAGE_READWRITE, NULL, sizeof(char) * dwSharedMemorySize, inputKeyName);
		if (NULL == handleSharedMem) {
			return;
		}

		//1番目起動判定用ミューテックスオブジェクト作成
		handleMtx = CreateMutex(NULL, FALSE, inputMutexKeyName);
		if (NULL == handleMtx) {
			return;
		}
		DWORD error_no = GetLastError(); //エラー番号の取得
		bool tryFirst = true;
		if (error_no == ERROR_ALREADY_EXISTS) {
			tryFirst = false;
			Sleep(1);
		}

		//マップビューの作成
		pMemory = (char*)MapViewOfFile(handleSharedMem, FILE_MAP_ALL_ACCESS, NULL, NULL, sizeof(char) * dwSharedMemorySize);
		if (NULL == pMemory) {
			return;
		}
		if (true == tryFirst) {
			//初期化
			memset(pMemory, sizeof(char) * dwSharedMemorySize, 0);
			idx = 0;
		}
		else {
			//カウントアップ
			pMemory[0]+=1;
			idx = pMemory[0];
		}
		isSync = true;
	}
	//デストラクタ
	~SyncKeyDown() {
		if (NULL != pMemory) {
			UnmapViewOfFile(pMemory);
			pMemory = NULL;
		}
		if (NULL != handleSharedMem) {
			CloseHandle(handleSharedMem);
			handleSharedMem = NULL;
		}
		if (NULL != handleMtx) {
			CloseHandle(handleMtx);
			handleMtx = NULL;
		}
	}

	//キーの状態判定
	void CheckKeyDown() {
		for (int i = 1; i <= VK_F24; i++) {
			SHORT sks = GetAsyncKeyState(i);
			if (sks & 1) {
				for (int r = 0; r <= pMemory[0]; r++) {
					int pos = keyDataStart + r * keySpace;
					pMemory[pos + i] = 1;
				}
			}
		}
	}

	//キーの押下を取得
	bool GetKeyDown(int keyNo, bool bReset = true)
	{
		int pos = keyDataStart + idx * keySpace;
		bool bDown = false;
		if (1 == pMemory[pos + keyNo]) {
			bDown = true;
		}
		if (true == bReset) {
			pMemory[pos + keyNo] = 0;
		}
		return bDown;
	}

	//キー押下を同期させるか設定
	void SetSync(bool bsync)
	{
		isSync = bsync;
	}
	//同期状態取得
	bool IsSync()
	{
		return isSync;
	}
};
