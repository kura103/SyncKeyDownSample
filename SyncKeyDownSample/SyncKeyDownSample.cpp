// TypeSync.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "SyncKeyDown.h"
#include <iostream>

int main()
{
	bool bLoop = true;
	SyncKeyDown skd;
	while (bLoop) {
		//�{�^���`�F�b�N
		skd.CheckKeyDown();

		//�������{�^���\��
		for (int i = 'A'; i <= 'Z'; i++) {
			char input = i;
			if (skd.GetKeyDown(input)) {
				std::cout << input << std::endl;
				if (input == 'Q') {//�A�v���I��
					bLoop = false;
				}
			}
		}
		Sleep(10);
	}
    return 0;
}

