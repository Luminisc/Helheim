/*
���ڷ��pck�ļ�
made by Darkness-TX
2022.07.12
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <zlib.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;
typedef unsigned __int64 unit64;

unit32 PackNum = 0;//���ļ�������ʼ����Ϊ0

struct FS
{
	unit8 signature[20];
	unit32 Length;//��������
}Fsignature;

struct PS
{
	unit8 signature[20];
	unit32 Length;//��������
}Psignature;

struct packindex
{
	char FileName[100];//�ļ���
	unit32 FileSize;//�ļ���С
	unit32 FileOffset;//�ļ�ƫ��
}PackIndex[20000];

void ReadIndex(char* FileName)
{
	unit32 i = 0, s = 0, namestart, namenext, saveoff = 0;
	FILE* fp;
	fopen_s(&fp, FileName, "rb");
	fread(Fsignature.signature, 20, 1, fp);
	if (strncmp("Filename", Fsignature.signature, 8) != 0)//"Filename"Ϊ�ļ���������ʾ
	{
		printf("��֧�ֵ��ļ����ͣ������ļ�ͷ��ʾ�Ƿ�ΪFileName\n");
		system("pause");
		exit(0);
	}
	else
	{
		fread(&Fsignature.Length, 4, 1, fp);
		Fsignature.Length = Fsignature.Length - sizeof(Fsignature.Length) - sizeof(Fsignature.signature) + 7 & ~7;
		fseek(fp, Fsignature.Length, SEEK_CUR);
		fread(Psignature.signature, 20, 1, fp);
		if (strncmp("Pack", Psignature.signature, 4) != 0)//"Pack"Ϊ��������ʾ
		{
			printf("��֧�ֵ��ļ����ͣ��������ʾ�Ƿ�ΪPack\n");
			system("pause");
			exit(0);
		}
		else
		{
			fread(&Psignature.Length, 4, 1, fp);
			Psignature.Length = Psignature.Length - sizeof(Psignature.Length) - sizeof(Psignature.signature) + 7 & ~7;
			fread(&PackNum, 4, 1, fp);
			for (s = 0; s < PackNum; s++)
			{
				fread(&PackIndex[s].FileOffset, 4, 1, fp);
				fread(&PackIndex[s].FileSize, 4, 1, fp);
			}
			fseek(fp, 0x18, SEEK_SET);
			fread(&namestart, 4, 1, fp);
			for (s = 0; s < PackNum; s++)
			{
				fread(&namenext, 4, 1, fp);
				fseek(fp, 0x18 + PackNum * 4 + saveoff, SEEK_SET);
				if (s != PackNum - 1)
				{
					fread(PackIndex[s].FileName, 1, namenext - namestart, fp);
					saveoff += namenext - namestart;
					namestart = namenext;
				}
				else
					fread(PackIndex[s].FileName, 1, Fsignature.Length - namestart, fp);
				fseek(fp, 0x18 + s * 4 + 8, SEEK_SET);
			}
		}
	}
	fclose(fp);
}

void PackFile(char* FileName)
{
	unit8* data, * cdata, dir[260], outname[260], zlib[4], zero = '\0';
	unit32 i, s, j, fsize, csize, indexsize;
	FILE* OpenFile, * src, * dst;
	fopen_s(&OpenFile, FileName, "rb");
	sprintf(outname, "%s.new", FileName);
	fopen_s(&dst, outname, "wb");
	sprintf(dir, "%s_unpack", FileName);
	_chdir(dir);
	indexsize = sizeof(Fsignature.signature) + sizeof(Fsignature.Length) + sizeof(Psignature.signature) + sizeof(Psignature.Length) + Fsignature.Length + Psignature.Length;
	data = malloc(indexsize);
	fread(data, indexsize, 1, OpenFile);
	fwrite(data, indexsize, 1, dst);
	free(data);
	for (s = 0; s < PackNum; s++)
	{
		fseek(OpenFile, PackIndex[s].FileOffset, SEEK_SET);
		fread(zlib, 4, 1, OpenFile);
		PackIndex[s].FileOffset = ftell(dst);
		if (strncmp("ZLIB", zlib, 4) != 0)
		{
			fopen_s(&src, PackIndex[s].FileName, "rb");
			fseek(src, 0, SEEK_END);
			PackIndex[s].FileSize = ftell(src);
			fseek(src, 0, SEEK_SET);
			data = malloc(PackIndex[s].FileSize);
			fread(data, PackIndex[s].FileSize, 1, src);
			fclose(src);
			fwrite(data, PackIndex[s].FileSize, 1, dst);
			free(data);
		}
		else
		{
			fopen_s(&src, PackIndex[s].FileName, "rb");
			fseek(src, 0, SEEK_END);
			fsize = ftell(src);
			fseek(src, 0, SEEK_SET);
			data = malloc(fsize);
			fread(data, fsize, 1, src);
			fclose(src);
			csize = fsize * 2;
			cdata = malloc(csize);
			compress2(cdata, &csize, data, fsize, Z_DEFAULT_COMPRESSION);
			free(data);
			PackIndex[s].FileSize = csize + 0x0C;
			fwrite(zlib, 4, 1, dst);
			fwrite(&fsize, 4, 1, dst);
			fwrite(&csize, 4, 1, dst);
			fwrite(cdata, csize, 1, dst);
			free(cdata);
		}
		//����������if�����ʹ�û�ʹ�ã�Ӱ�첻�󣬵�8�ֽڶ�������У���script.pck��voice.pck���־Ͳ���Ҫ % 0x10����
		if (ftell(dst) % 8 != 0)
		{
			j = (ftell(dst) + 7 & ~7) - ftell(dst);
			for (i = 0; i < j; i++)
				fwrite(&zero, 1, 1, dst);
		}
		if ((ftell(dst) % 0x10) == 0)
		{
			for (j = 0; j < 8; j++)
				fwrite(&zero, 1, 1, dst);
		}
		printf("%s offset:0x%X size:0x%X\n", PackIndex[s].FileName, PackIndex[s].FileOffset, PackIndex[s].FileSize);
	}
	fclose(OpenFile);
	indexsize = sizeof(Fsignature.signature) + sizeof(Fsignature.Length) + sizeof(Psignature.signature) + sizeof(Psignature.Length) + Fsignature.Length + 4;
	fseek(dst, indexsize, SEEK_SET);
	for (s = 0; s < PackNum; s++)
	{
		fwrite(&PackIndex[s].FileOffset, 4, 1, dst);
		fwrite(&PackIndex[s].FileSize, 4, 1, dst);
	}
	fclose(dst);
}

int main(int argc, char* argv[])
{
	printf("project��Helheim-DATE A LIVE ��Dystopia\n���ڷ��pck�ļ�����pck�ļ��ϵ�������\nby Darkness-TX 2022.07.12\n\n");
	ReadIndex(argv[1]);
	PackFile(argv[1]);
	printf("����ɣ����ļ���%d\n", PackNum);
	system("pause");
	return 0;
}