/*
Copyright (c) 2022 kuratomi.masaru

�ȉ��ɒ�߂�����ɏ]���A�{�\�t�g�E�F�A����ъ֘A�����̃t�@�C���i�ȉ��u�\�t�g�E�F�A�v�j�̕������擾���邷�ׂĂ̐l�ɑ΂��A�\�t�g�E�F�A�𖳐����Ɉ������Ƃ𖳏��ŋ����܂��B
����ɂ́A�\�t�g�E�F�A�̕������g�p�A���ʁA�ύX�A�����A�f�ځA�Еz�A�T�u���C�Z���X�A�����/�܂��͔̔����錠���A����у\�t�g�E�F�A��񋟂��鑊��ɓ������Ƃ������錠�����������Ɋ܂܂�܂��B
��L�̒��쌠�\������і{�����\�����A�\�t�g�E�F�A�̂��ׂĂ̕����܂��͏d�v�ȕ����ɋL�ڂ�����̂Ƃ��܂��B

�\�t�g�E�F�A�́u����̂܂܁v�ŁA�����ł��邩�Öقł��邩���킸�A����̕ۏ؂��Ȃ��񋟂���܂��B
�����ł����ۏ؂Ƃ́A���i���A����̖ړI�ւ̓K�����A����ь�����N�Q�ɂ��Ă̕ۏ؂��܂݂܂����A����Ɍ��肳�����̂ł͂���܂���B
��҂܂��͒��쌠�҂́A�_��s�ׁA�s�@�s�ׁA�܂��͂���ȊO�ł��낤�ƁA�\�t�g�E�F�A�ɋN���܂��͊֘A���A���邢�̓\�t�g�E�F�A�̎g�p�܂��͂��̑��̈����ɂ���Đ������؂̐����A���Q�A���̑��̋`���ɂ��ĉ���̐ӔC������Ȃ����̂Ƃ��܂��B

*/

#pragma once

#include <windows.h>
class SyncKeyDown
{
	bool	isSync = false;
	int		idx;
	HANDLE	handleMtx = NULL;		//1�ԖڋN������p�~���[�e�b�N�X�n���h��
	HANDLE	handleSharedMem = NULL;
	char*	pMemory = NULL;
	const int dwSharedMemorySize = 1024; //�����N�����ɂ���Ē�������
	const int keyDataStart = 10;
	const int keySpace = VK_F24 + 1;
public:
	//�R���X�g���N�^
	SyncKeyDown(const wchar_t* inputKeyName = L"SyncKeyDown", const wchar_t* inputMutexKeyName = L"SyncKeyDownMutex") {

		//��������Ƀ}�b�s���O�I�u�W�F�N�g�̍쐬
		handleSharedMem = CreateFileMapping(NULL, NULL, PAGE_READWRITE, NULL, sizeof(char) * dwSharedMemorySize, inputKeyName);
		if (NULL == handleSharedMem) {
			return;
		}

		//1�ԖڋN������p�~���[�e�b�N�X�I�u�W�F�N�g�쐬
		handleMtx = CreateMutex(NULL, FALSE, inputMutexKeyName);
		if (NULL == handleMtx) {
			return;
		}
		DWORD error_no = GetLastError(); //�G���[�ԍ��̎擾
		bool tryFirst = true;
		if (error_no == ERROR_ALREADY_EXISTS) {
			tryFirst = false;
			Sleep(1);
		}

		//�}�b�v�r���[�̍쐬
		pMemory = (char*)MapViewOfFile(handleSharedMem, FILE_MAP_ALL_ACCESS, NULL, NULL, sizeof(char) * dwSharedMemorySize);
		if (NULL == pMemory) {
			return;
		}
		if (true == tryFirst) {
			//������
			memset(pMemory, sizeof(char) * dwSharedMemorySize, 0);
			idx = 0;
		}
		else {
			//�J�E���g�A�b�v
			pMemory[0]+=1;
			idx = pMemory[0];
		}
		isSync = true;
	}
	//�f�X�g���N�^
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

	//�L�[�̏�Ԕ���
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

	//�L�[�̉������擾
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

	//�L�[�����𓯊������邩�ݒ�
	void SetSync(bool bsync)
	{
		isSync = bsync;
	}
	//������Ԏ擾
	bool IsSync()
	{
		return isSync;
	}
};
