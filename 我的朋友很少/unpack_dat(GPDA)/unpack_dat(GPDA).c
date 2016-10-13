/*
���ڽ��dat(GPDA)
by Darkness-TX
2016.9.16
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
char Head[] = "GPDA";

struct FileInfo
{
	unit32 FileID;//�ļ�ID
	unit32 FileOffset;//�ļ�ƫ��
	unit32 FileSize;//�ļ���С
	unit32 NameOffset;//�ļ�������ƫ��
	unit32 NameSize;//�ļ�������
	char FileName[32];//�ļ���
}Index;

void WriteFile(char *fname)
{
	unit8* data;
	unit8 signature[4];
	char FileName[260];
	unit32 i, point = 0;
	FILE *OpenFile, *WriteFile;
	OpenFile = fopen(fname, "rb");
	if (OpenFile == NULL)
	{
		printf("�޷����ļ�%s��",fname);
		system("pause");
		exit(0);
	}
	_mkdir(strcat(fname, "_unpack"));
	_chdir(fname);
	fread(signature, 4, 1, OpenFile);
	if (strncmp(signature, Head,4) != 0)
	{
		printf("�ļ�ͷ����GPDA��");
		system("pause");
		exit(0);
	}
	else
	{
		fseek(OpenFile, 8, SEEK_CUR);
		fread(&FileNum, 4, 1, OpenFile);
	}
	for (i = 0; i < FileNum; i++)
	{
		Index.FileID = i;
		fread(&Index.FileOffset, 4, 1, OpenFile);
		fseek(OpenFile, 4, SEEK_CUR);
		fread(&Index.FileSize, 4, 1, OpenFile);
		fread(&Index.NameOffset, 4, 1, OpenFile);
		point = ftell(OpenFile);
		fseek(OpenFile, Index.FileOffset, SEEK_SET);
		data = malloc(Index.FileSize);
		fread(data, Index.FileSize, 1, OpenFile);
		fseek(OpenFile, Index.NameOffset, SEEK_SET);
		fread(&Index.NameSize, 4, 1, OpenFile);
		fread(Index.FileName, Index.NameSize, 1, OpenFile);
		Index.FileName[Index.NameSize] = '\0';//��ȡ�ļ������޽��������Լ�����
		printf("FileID:%d FileOffset:0x%X FileSize:0x%X NameOffset:0x%X %s\n", Index.FileID, Index.FileOffset, Index.FileSize, Index.NameOffset, Index.FileName);
		sprintf(FileName, "%d_%s", Index.FileID, Index.FileName);//��Ϊ��Щ������ļ�����Ȼ����ͬ�ģ����Խ��ʱ���������ļ��Ḳ�ǵ�ǰ����ͬ���ֵ��ļ���������Ӹ����
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
	printf("project��Helheim-����\n���ڽ��dat(GPDA)���϶�ԭ�ļ���������\nby Darkness-TX 2016.9.16\nEmail��1545492547@qq.com\n");
	WriteFile(argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}