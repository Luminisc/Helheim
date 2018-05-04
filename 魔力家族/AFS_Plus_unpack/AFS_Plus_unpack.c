/*
���ڽ���ļ�ͷΪAFS\0���ļ�
made by Darkness-TX
2018.05.03
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <locale.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit8 Header[] = "AFS\0";
unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

struct afs_header
{
	char magic[4];
	unit32 filenum;
}AFS_Header;

struct index
{
	char FileName[260];//�ļ���
	unit32 FileSize;//�ļ���С
	unit32 FileOffset;
}Index[2000];

void ReadIndex(FILE *src)
{
	unit32 i = 0, index_offset = 0, index_size = 0;
	fread(AFS_Header.magic, 4, 1, src);
	fread(&AFS_Header.filenum, 4, 1, src);
	if (strncmp(Header, AFS_Header.magic, 4) != 0)
	{
		printf("�ļ�ͷ����AFS\\0\n");
		system("pause");
		exit(0);
	}
	for (i = 0; i < AFS_Header.filenum; i++)
	{
		fread(&Index[i].FileOffset, 4, 1, src);
		fread(&Index[i].FileSize, 4, 1, src);
	}
	fread(&index_offset, 4, 1, src);
	fread(&index_size, 4, 1, src);
	fseek(src, index_offset, SEEK_SET);
	for (i = 0; i < AFS_Header.filenum; i++)
	{
		fread(Index[i].FileName, 32, 1, src);
		fseek(src, 0x10, SEEK_CUR);
	}
	FileNum = AFS_Header.filenum;
}

void UnpackFile(char *fname)
{
	FILE *src, *dst;
	unit8 *data = NULL;
	wchar_t dstname[260];
	char name[260];
	unit32 i = 0;
	src = fopen(fname, "rb");
	ReadIndex(src);
	sprintf(name, "%s_unpack", fname);
	_mkdir(name);
	_chdir(name);
	for (i = 0; i < FileNum; i++)
	{
		fseek(src, Index[i].FileOffset, SEEK_SET);
		data = malloc(Index[i].FileSize);
		fread(data, Index[i].FileSize, 1, src);
		sprintf(name, "%s_%d.bin", Index[i].FileName, i);
		MultiByteToWideChar(932, 0, name, 260, dstname, 260);
		wprintf(L"%ls offset:0x%X size:0x%X\n", dstname, Index[i].FileOffset, Index[i].FileSize);
		dst = _wfopen(dstname, L"wb");
		fwrite(data, Index[i].FileSize, 1, dst);
		free(data);
		fclose(dst);
	}
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "chs");
	printf("project��Helheim-ħ������\n���ڽ���ļ�ͷΪAFS\\0���ļ������ļ��ϵ������ϡ�\nby Darkness-TX 2018.05.03\n");
	UnpackFile(argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}