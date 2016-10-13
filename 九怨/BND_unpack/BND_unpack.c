/*
���ڽ��ALL.BND��������BND�޷��������ǽ�����BND�ṹ��ͬС��
by Darkness-TX
2015.12.5
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0
char Head[] = "BND\0";

struct FileInfo
{
	unit32 FileID;//�ļ�ID
	unit32 FileOffset;//�ļ�ƫ��
	unit32 FileSize;//�ļ���С
	unit32 NameOffset;//�ļ�������ƫ��
}Index;

void WriteFile(char *fname)
{
	unit8* data;
	unit8 signature[4];
	char FileName[32];
	unit32 i, point = 0;
	FILE *OpenFile, *WriteFile;
	OpenFile = fopen(fname, "rb");
	if (OpenFile == NULL)
	{
		printf("�޷����ļ���");
		system("pause");
		exit(0);
	}
	_mkdir(strcat(fname, "_unpack"));
	_chdir(fname);
	fread(signature, 4, 1, OpenFile);
	if (strcmp(signature, Head) != 0)
	{
		printf("�ļ�ͷ����BND��");
		system("pause");
		exit(0);
	}
	else
	{
		fseek(OpenFile, 0xC, SEEK_SET);
		fread(&FileNum, 4, 1, OpenFile);
	}
	for (i = 0; i < FileNum; i++)
	{
		fread(&Index.FileID, 4, 1, OpenFile);
		fread(&Index.FileOffset, 4, 1, OpenFile);
		fread(&Index.FileSize, 4, 1, OpenFile);
		fread(&Index.NameOffset, 4, 1, OpenFile);
		point = ftell(OpenFile);
		fseek(OpenFile, Index.FileOffset, SEEK_SET);
		data = malloc(Index.FileSize);
		fread(data, Index.FileSize, 1, OpenFile);
		fseek(OpenFile, Index.NameOffset, SEEK_SET);
		fscanf(OpenFile, "%32s",FileName);//fscanf���Լ����/0��β���ַ���
		printf("FileID:0x%X FileOffset:0x%X FileSize:0x%X NameOffset:0x%X %s\n", Index.FileID, Index.FileOffset, Index.FileSize, Index.NameOffset, FileName);
		WriteFile = fopen(FileName, "wb");
		fwrite(data, Index.FileSize, 1, WriteFile);
		free(data);
		fclose(WriteFile);
		fseek(OpenFile, point, SEEK_SET);
	}
	fclose(OpenFile);
}

int main(int argc, char *argv[])
{
	printf("project��Helheim-��Թ\n���ڽ��ALL.BND���϶�ԭ�ļ���������\nby Darkness-TX 2015.12.5\nEmail��1545492547@qq.com\n");
	WriteFile(argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}