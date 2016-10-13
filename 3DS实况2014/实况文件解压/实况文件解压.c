#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <direct.h>
#include <zlib.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

unit32 FileNum = 0;//���ļ�������ʼ����Ϊ0

struct index
{
	char FileName[260];//�ļ���
	unit32 FileSize;//�ļ���С
}Index[12000];

struct findex
{
	char FileName[260];//�ļ���
	unit32 FileNameOff;//�ļ���λ��
	unit32 FileOffset;//�ļ�ƫ��
}FIndex[300];

unit32 process_dir(char *dname)
{
	long Handle;
	unit32 i = 0;
	struct _finddata64i32_t FileInfo;
	_chdir(dname);//��ת·��
	if ((Handle = _findfirst("*.*", &FileInfo)) == -1L)
	{
		printf("û���ҵ�ƥ�����Ŀ��\n");
		system("pause");
		exit(0);
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

void decompression(unit32 FileNum, char *dname)
{
	FILE *OpenFile, *WriteFile;
	unit8 *buf, *ubuf, filename[260];
	unit32 i, hflag, filen, noffset, foffset, j;
	unit16 flag;
	uLong flen, ulen;
	_chdir(dname);
	for (i = 0; i < FileNum; i++)
	{
		fopen_s(&OpenFile, Index[i].FileName, "rb");
		if (OpenFile == NULL)
		{
			printf("���ļ�ʧ��\n");
			system("pause");
			exit(0);
		}
		else
		{
			fread(&hflag, 4, 1, OpenFile);
			if (hflag == 0x200)
			{
				fseek(OpenFile, 0x20, SEEK_SET);
				fread(&filen, 4, 1, OpenFile);//�ļ�����֮��Ϊ�ļ���ƫ������ʼλ�ã��ļ�ƫ������ʼλ�ã���Ҫ��0x20
				fread(&noffset, 4, 1, OpenFile);
				noffset += 0x20;
				fread(&foffset, 4, 1, OpenFile);
				foffset += 0x20;
				printf("%s flag:0x%X nameoffset:0x%X fileoffset:0x%X\n", Index[i].FileName, hflag, noffset, foffset);
				fseek(OpenFile, noffset, SEEK_SET);
				for (j = 0; j < filen; j++)
				{
					fread(&FIndex[j].FileNameOff, 4, 1, OpenFile);
					FIndex[j].FileNameOff += 0x20;
				}
				fseek(OpenFile, foffset, SEEK_SET);
				for (j = 0; j < filen; j++)
				{
					fread(&FIndex[j].FileOffset, 4, 1, OpenFile);
					FIndex[j].FileOffset += 0x20;
				}
				for (j = 0; j < filen; j++)
				{
					fseek(OpenFile, FIndex[j].FileNameOff, SEEK_SET);
					fread(&FIndex[j].FileName, 16, 1, OpenFile);
					if (FIndex[j].FileName[15] != '\0')
					{
						fseek(OpenFile, -0x10, SEEK_CUR);
						fread(&FIndex[j].FileName, 32, 1, OpenFile);
						if (FIndex[j].FileName[31] != '\0')
						{
							fseek(OpenFile, -0x20, SEEK_CUR);
							fread(&FIndex[j].FileName, 48, 1, OpenFile);
						}
					}
				}
				sprintf(filename, "%s_unpack", Index[i].FileName);
				_mkdir(filename);
				_chdir(filename);
				for (j = 0; j < filen; j++)
				{
					fseek(OpenFile, FIndex[j].FileOffset, SEEK_SET);
					fread(&hflag, 4, 1, OpenFile);
					if (hflag == 0x10300)
					{
						fread(&flen, 4, 1, OpenFile);
						fread(&ulen, 4, 1, OpenFile);
						fseek(OpenFile, 0x14, SEEK_CUR);
						fopen_s(&WriteFile, FIndex[j].FileName, "wb");
						buf = malloc(flen);
						ubuf = malloc(ulen);
						fread(buf, flen, 1, OpenFile);
						uncompress(ubuf, &ulen, buf, flen);
						fwrite(ubuf, ulen, 1, WriteFile);
						free(buf);
						free(ubuf);
						fclose(WriteFile);
						printf("\t%s flag:0x%X �ļ���С��0x%X ��ѹ��С��0x%X\n", FIndex[j].FileName, hflag, flen + 0x20, ulen);
					}
					else
					{
						printf("\t%s\n", FIndex[j].FileName);
						system("pause");
					}
				}
				_chdir("..");
			}
			else// if (hflag == 0x10300 || hflag == 0x10600)
			{
				fread(&flen, 4, 1, OpenFile);
				fread(&ulen, 4, 1, OpenFile);
				fseek(OpenFile, 0x20, SEEK_SET);
				fread(&flag, 2, 1, OpenFile);
				if (flag == 0xDA78)
				{
					fseek(OpenFile, 0x20, SEEK_SET);
					fopen_s(&WriteFile, strcat(Index[i].FileName, ".dec"), "wb");
					buf = malloc(flen);
					ubuf = malloc(ulen);
					fread(buf, flen, 1, OpenFile);
					uncompress(ubuf, &ulen, buf, flen);
					fwrite(ubuf, ulen, 1, WriteFile);
					free(buf);
					free(ubuf);
					fclose(WriteFile);
					printf("%s �ļ���С��0x%X ��ѹ��С��0x%X\n", Index[i].FileName, Index[i].FileSize, ulen);
				}
				else
				{
					printf("%s �ļ���С��0x%X\n", Index[i].FileName, Index[i].FileSize);
					system("pause");
				}
			}
		}
		fclose(OpenFile);
	}
}

int main(int argc, char *argv[])
{
	printf("project��Helheim-3DS WE 2014\n���ڽ�ѹ����zlibѹ�����ļ������ļ����ϵ������ϣ�\n�ѹ��ؿɴ���һ���ǧ���ļ��Ļ��������ļ����е��ļ��벻Ҫ����һ����ǧ��\nby Darkness-TX 2016.3.11\n");
	process_dir(argv[1]);
	decompression(FileNum, argv[1]);
	printf("����ɣ����ļ���%d\n", FileNum);
	system("pause");
	return 0;
}