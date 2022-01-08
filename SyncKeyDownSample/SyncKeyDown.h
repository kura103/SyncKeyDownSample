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
