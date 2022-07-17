/*
���ڽ�ѹSTSC2�ļ�
made by Darkness-TX
2022.07.14
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <zlib.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;
typedef unsigned __int64 unit64;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

struct stsc2_header
{
	unit8 magic[16];//STSC2.0.7
	unit32 block_length;//���С
}STSC2_Header;

struct index
{
	char FileName[MAX_PATH];//�ļ���
}Index[5000];

unit32 process_dir(char* dname)
{
	long Handle;
	unit32 i = 0;
	struct _finddata64i32_t FileInfo;
	_chdir(dname);//��ת·��
	if ((Handle = _findfirst("*.bin", &FileInfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ���뽫ԭ�ļ���׺����Ϊ.bin\n");
		system("pause");
		return -1;
	}
	do
	{
		if (FileInfo.name[0] == '.')  //���˱���Ŀ¼�͸�Ŀ¼
			continue;
		sprintf(Index[FileNum].FileName, FileInfo.name);
		FileNum++;
	} while (_findnext(Handle, &FileInfo) == 0);
	return FileNum;
}

void Decomp(char* dname)
{
	unit8* data = NULL, *udata = NULL, namebuff[MAX_PATH];
	FILE* src = NULL, * dst = NULL;
	unit32 i = 0, csize = 0, usize = 0;
	_chdir(dname);
	for (i = 0; i < FileNum; i++)
	{
		src = fopen(Index[i].FileName, "rb");
		if (src == NULL)
		{
			printf("��%sʧ��", Index[i].FileName);
			system("pause");
			exit(0);
		}
		fread(STSC2_Header.magic, 16, 1, src);
		if (strncmp(STSC2_Header.magic, "STSC2.0.7", 9) != 0)
		{
			printf("%s���ļ�ͷ����STSC2.0.7\n", Index[i].FileName);
			system("pause");
			exit(0);
		}
		fread(&STSC2_Header.block_length, 4, 1, src);
		sprintf(namebuff, "%s.dec", Index[i].FileName);
		dst = fopen(namebuff, "wb");
		data = malloc(STSC2_Header.block_length);
		fseek(src, 0, SEEK_SET);
		fread(data, STSC2_Header.block_length, 1, src);
		fwrite(data, STSC2_Header.block_length, 1, dst);
		free(data);
		fseek(src, 0, SEEK_END);
		csize = ftell(src) - STSC2_Header.block_length;
		usize = csize * 2;
		fseek(src, STSC2_Header.block_length, SEEK_SET);
		data = malloc(csize);
		udata = malloc(usize);
		fread(data, csize, 1, src);
		fclose(src);
		uncompress(udata, &usize, data, csize);
		printf("%s block_length:0x%X comp_size:0x%X decomp_size:0x%X\n", Index[i].FileName, STSC2_Header.block_length, csize, usize);
		free(data);
		fwrite(udata, usize, 1, dst);
		free(udata);
		fclose(dst);
	}
}

int main(int argc, char* argv[])
{
	printf("project��Helheim-DATE A LIVE ��Dystopia\n���ڽ�ѹSTSC2�ļ�����bin�ļ��Ž��ļ��У������ļ��е�������\nby Darkness-TX 2022.07.14\n\n");
	process_dir(argv[1]);
	Decomp(argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}