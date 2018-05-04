/*
���ڽ�ѹ�ļ�ͷΪUFFA���ļ�
made by Darkness-TX
2018.05.03
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include "lzss.h"

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

struct index
{
	char FileName[260];//�ļ���
	unit32 FileSize;//�ļ���С
	unit32 Comp_Size;
	unit32 Decomp_Size;
}Index[4000];

unit32 process_dir(char *dname)
{
	long Handle;
	unit32 i = 0;
	struct _finddata64i32_t FileInfo;
	_chdir(dname);//��ת·��
	if ((Handle = _findfirst("*.UFFA", &FileInfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ��\n");
		system("pause");
		return -1;
	}
	do
	{
		if (FileInfo.name[0] == '.')  //���˱���Ŀ¼�͸�Ŀ¼
			continue;
		sprintf(Index[FileNum].FileName, FileInfo.name);
		Index[FileNum].FileSize = FileInfo.size;
		FileNum++;
	} while (_findnext(Handle, &FileInfo) == 0);
	return FileNum;
}

void DecompFile()
{
	FILE *src, *dst;
	char dstname[260], magic[4];
	unit8 *cdata = NULL, *udata = NULL;
	for (unit32 i = 0; i < FileNum; i++)
	{
		src = fopen(Index[i].FileName, "rb");
		fread(magic, 4, 1, src);
		printf("%s ", Index[i].FileName);
		if (strncmp(magic, "UFFA", 4) == 0)
		{
			fread(&Index[i].Comp_Size, 4, 1, src);
			fread(&Index[i].Decomp_Size, 4, 1, src);
			printf("comp_size:0x%X decomp_size:0x%X\n", Index[i].Comp_Size, Index[i].Decomp_Size);
			sprintf(dstname, "%s.dec", Index[i].FileName);
			fseek(src, 4, SEEK_CUR);
			cdata = malloc(Index[i].Comp_Size);
			udata = malloc(Index[i].Decomp_Size);
			fread(cdata, Index[i].Comp_Size, 1, src);
			lzss_decompress(udata, Index[i].Decomp_Size, cdata, Index[i].Comp_Size);
			free(cdata);
			dst = fopen(dstname, "wb");
			fwrite(udata, Index[i].Decomp_Size, 1, dst);
			free(udata);
			fclose(dst);
		}
		else
		{
			printf("�ļ�ͷ����UFFA\n");
			system("pause");
		}
		fclose(src);
	}
}

int main(int argc, char *argv[])
{
	process_dir(argv[1]);
	DecompFile();
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}