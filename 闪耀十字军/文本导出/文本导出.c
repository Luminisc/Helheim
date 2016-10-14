#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <windows.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

struct index
{
	char FileName[260];//�ļ���
	unit32 FileSize;//�ļ���С
}Index[1000];

unit32 process_dir(char *dname)
{
	long Handle;
	unit32 i = 0;
	struct _finddata64i32_t FileInfo;
	_chdir(dname);//��ת·��
	if ((Handle = _findfirst("*.tm2", &FileInfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ���뽫��׺����Ϊ.LZ\n");
		system("pause");
		return -1;
	}
	do
	{
		if (FileInfo.name[0] == '.')  //���˱���Ŀ¼�͸�Ŀ¼
			continue;
		for (i = 0; i <= 260; i++)
			Index[FileNum].FileName[i] = FileInfo.name[i];
		Index[FileNum].FileSize = FileInfo.size;
		FileNum++;
	} while (_findnext(Handle, &FileInfo) == 0);
	return FileNum;
}

void WriteTxt(unit32 FileNum, char *dname)
{
	unit8 dumptxt[512], *data;
	wchar_t writetxt[512], number[16];
	unit32 i, j, texnum, num = 1;
	FILE *OpenFile, *WriteFile;
	_chdir(dname);
	for (i = 0; i < FileNum; i++)
	{
		fopen_s(&OpenFile, Index[i].FileName, "rb");
		fopen_s(&WriteFile, strcat(Index[i].FileName, ".txt"), "wb");
		fputc(0xff, WriteFile);
		fputc(0xfe, WriteFile);
		if (OpenFile == NULL)
		{
			printf("���ļ�ʧ��\n");
			system("pause");
		}
		else
		{
			for (j = 0; j < Index[i].FileSize;)
			{
				fprintf(OpenFile,"%s",dumptxt);
				texnum = MultiByteToWideChar(932, 0, dumptxt, strlen(dumptxt), 0, 0);
				j = j + texnum + 2;
				fseek(OpenFile, j, SEEK_SET);
				MultiByteToWideChar(932, 0, dumptxt, strlen(dumptxt), writetxt, texnum);
				swprintf(number, 16, L"%08d", num);
				fwprintf(WriteFile, L"��%s��", number);				
				fwprintf(WriteFile, L"%s\r\n", writetxt);
				fwprintf(WriteFile, L"��%s��\r\n", number);
				printf("%s %x %d", dumptxt, writetxt, j);
				fwprintf(WriteFile, L"��%s��\r\n\r\n", number);
				num += 1;
			}
		}
		fclose(OpenFile);
		fclose(WriteFile);
	}
}

int main(int argc, char *argv[])
{
	process_dir(argv[1]);
	WriteTxt(FileNum, argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}