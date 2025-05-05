#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char  unit8;
typedef unsigned short unit16;
typedef unsigned int   unit32;

typedef struct info_s
{
	unit32 buffsize;
	unit32 distance;
	struct info_s *next;
}Nodeinfo, *Linkinfo;

//sub_88292CC
unit32 get_start_offset(unit8* data)
{
	unit8* offset = data, buff = 0;
	int size = 0, buff_size = 0, buff2 = 0;
	do
	{
		buff = *offset;
		size <<= 7;
		size |= buff;
		offset++;
	} while ((size & 1) == 0);
	size >>= 1;
	if (size <= 0)
	{
		printf("��ѹ��С����size:0x%X\nsceKernelDcacheWritebackInvalidateAll()\n", size);
		system("pause");
		return -1;
	}
	do
	{
		buff = *offset;
		buff2 <<= 7;
		buff2 |= buff;
		offset++;
	} while ((buff2 & 1) == 0);
	buff2 >>= 1;
	buff_size = ((buff2 >> 1) & 0x0F) + 8;
	buff_size = 1 << buff_size;
	//buff_size��0x100Ϊ��ʹ�С����������ļ���Ҫ+8������ʵ�ֿ�make_buff_size
	if (buff_size >= size)
	{
		if ((buff2 & 0x21) != 1)
		{
			if (buff2 & 0x40)
			{
				//����buff2���沢�����õ�����֪�������ɶ����
				//��Ȼ����(buff2 & 0x21) != 1��û���ֹ�
				buff2 = 0;
				do
				{
					buff = *offset;
					buff2 <<= 7;
					buff2 |= buff;
					offset++;
				} while ((buff2 & 1) == 0);
			}
		}
	}
	else
	{
		if (buff2 & 0x40)
		{
			buff2 = 0;
			do
			{
				buff = *offset;
				buff2 <<= 7;
				buff2 |= buff;
				offset++;
			} while ((buff2 & 1) == 0);
		}
	}
	//�ԣ���������������ֱ������ոյļ����ˣ�ֻ�ܵ����������������ˣ�
	//��Ȼ�����ܵ������㷨����һ���㷨�ļ򻯰汾�������㷨���������������������
	//�������������Ǳ���ģ����ļ�����ֱ�Ӷ�ȡ0x01ǿ�ƽ���������Ȼ���Ǽ򻯰汾�ɣ�
	buff2 = 0;
	do
	{
		buff = *offset;
		buff2 <<= 7;
		buff2 |= buff;
		offset++;
	} while ((buff2 & 1) == 0);
	return offset - data;
	/*
	unit8* a0 = data;
	unit32 v0 = 0;
	unit32 v1 = 0;
	do
	{
		v0 = *a0;
		v1 <<= 7;
		v1 |= v0;
		a0 += 1;
	} while ((v1 & 1) == 0);
	unit32 s0 = v1 >> 1;
	if (s0 <= 0)
	{
		printf("sceKernelDcacheWritebackInvalidateAll\n");
		system("pause");
		return -1;
	}
	v1 = 0;
	do
	{
		v0 = *a0;
		v1 <<= 7;
		v1 |= v0;
		a0 += 1;
	} while ((v1 & 1) == 0);
	unit32 a1 = v1 >> 1;
	//ext v0,a1,0x1,0x4
	//addiu v0,v0,0x8
	v0 = ((a1 >> 1) & 0x0F) + 8;
	v1 = 1;
	v0 = v1 << v0;
	if (v0 >= s0)
	{
		v0 = a1 & 0x21;
		if (v0 != v1)
		{
			v0 = a1 & 0x40;
			if (v0)
			{
				v1 = 0;
				do
				{
					v0 = *a0;
					v1 <<= 7;
					v1 |= v0;
					a0 += 1;
				} while ((v1 & 1) == 0);
			}
		}
	}
	else
	{
		v0 = a1 & 0x40;
		if (v0)
		{
			v1 = 0;
			do
			{
				v0 = *a0;
				v1 <<= 7;
				v1 |= v0;
				a0 += 1;
			} while ((v1 & 1) == 0);
		}
	}
	v1 = 0;
	do
	{
		v0 = *a0;
		v1 <<= 7;
		v1 |= v0;
		a0 += 1;
	} while ((v1 & 1) == 0);
	return a0 - data;
	*/
}

unit32 get_size(unit8* data)
{
	unit8 *offset = data, buff = 0;
	int size = 0;
	do
	{
		buff = *offset;
		size <<= 7;
		size |= buff;
		offset++;
	} while ((size & 1) == 0);
	return size >> 1;
	/*
	unit8* a0 = data;
	unit32 v0 = 0;
	unit32 v1 = 0;
	do
	{
		v0 = *a0;
		v1 <<= 7;
		v1 |= v0;
		a0 += 1;
	} while ((v1 & 1) == 0);
	unit32 s0 = v1 >> 1;
	return s0;
	*/
}

//sub_88294BC
unit32 LZdecompress(unit8* cdata, unit8* udata, unit32 size)
{
	unit8 *src = cdata, *dst = udata, *window = udata;
	unit8 buff = 0, buff2 = 0, sign = 0;
	int buffsize = 0, round = 0, distance = 0;
	do
	{
		//��������
		sign = *src;
		src++;
		buffsize = sign & 0x0F;
		round = sign >> 4;
		if (buffsize == 0)//�����λ����ʾ��ʼ���ݵ�buffsize
		{
			do
			{
				buff = *src;
				buffsize <<= 7;
				buffsize |= buff;
				src++;
			} while ((buffsize & 1) == 0);
			buffsize >>= 1;
		}
		if (round == 0)//�����λ����ʾ������ѹ�Ļ���
		{
			do
			{
				buff = *src;
				round <<= 7;
				round |= buff;
				src++;
			} while ((round & 1) == 0);
			round >>= 1;
		}
		do
		{
			*dst = *src;
			buffsize--;
			src++;
			dst++;
		} while (buffsize != 0);
		if ((unit32)(dst - udata) >= size)
			return 0;
		//��ʼ����
		do
		{
			sign = *src;
			src++;
			distance = sign & 0x0F;
			buffsize = sign >> 4;
			//��ѹ���Ǿ���LZѹ���ģ�ǰ�����,���ȣ������������ڴ�С�ƺ�û�����ƣ�ǰ�������Էǳ���
			if ((distance & 1) == 0)//�����λ
			{
				do
				{
					buff = *src;
					distance <<= 7;
					distance |= buff;
					src++;
				} while ((distance & 1) == 0);
			}
			distance >>= 1;
			if (buffsize == 0)//�����λ
			{
				do
				{
					buff = *src;
					buffsize <<= 7;
					buffsize |= buff;
					src++;
				} while ((buffsize & 1) == 0);
				buffsize >>= 1;
			}
			buffsize++;
			window = dst - distance - 1;
			if (window < udata)
				return 1;
			if ((unit32)(dst + buffsize - udata) >= size)
				buffsize = udata + size - dst;
			do
			{
				*dst = *window;
				window++;
				dst++;
				buffsize--;
			} while (buffsize != 0);
			round--;
		} while (round != 0);
	} while ((unit32)(dst - udata) < size);
	return 0;
	/*
	unit8 *a0 = cdata;
	unit8 *a2 = udata;
	unit32 t0 = 0;
	unit32 t1 = 0;
	unit32 a3 = 0;
	unit32 a1 = 0;
	unit32 t2 = 0;
	while (1)
	{
		t0 = *a0;
		t1 = t0 & 0x0F;
		a0 += 1;
		if (t1 == 0)
		{
			a3 = 0;
			do
			{
				a1 = *a0;
				a3 <<= 7;
				a3 |= a1;
				a0 += 1;
			} while ((a3 & 1) == 0);
			t1 = a3 >> 1;
		}
		t2 = t0 >> 4;
		if (t2 == 0)
		{
			a3 = 0;
			do
			{
				a1 = *a0;
				a3 <<= 7;
				a3 |= a1;
				a0 += 1;
			} while ((a3 & 1) == 0);
			t2 = a3 >> 1;
		}
		do
		{
			a1 = *a0;
			t1--;
			*a2 = a1 & 0xFF;
			a0 += 1;
			a2 += 1;
		} while (t1 != 0);
		if ((unit32)(a2 - udata) >= size)
			return 0;
		while (1)
		{
			t1 = *a0;
			a3 = t1 & 0x0F;
			a0 += 1;
			if ((a3 & 1) == 0)
			{
				do
				{
					a1 = *a0;
					a3 <<= 7;
					a3 |= a1;
					a0 += 1;
				} while ((a3 & 1) == 0);
			}
			a1 = a3 >> 1;
			a3 = t1 >> 4;
			t0 = ~a1;
			if (a3 == 0)
			{
				do
				{
					a1 = *a0;
					a3 <<= 7;
					a3 |= a1;
					a0 += 1;
				} while ((a3 & 1) == 0);
				a3 >>= 1;
			}
			a3 = a3 + 1;
			a1 = (unit32)(a2 + t0);
			if ((unit8 *)a1 < udata)
				return 1;
			if ((unit32)(a2 + a3 - udata) >= size)
			{
				a3 = udata + size - a2;
				a1 = (unit32)(a2 + t0);
			}
			do
			{
				a1 = (unit32)(a2 + t0);
				a1 = *(unit8*)a1;
				a3 = a3 - 1;
				*a2 = a1 & 0xFF;
				a2 += 1;
			} while (a3 != 0);
			t2 = t2 - 1;
			if (t2 == 0)
				break;
		}
		if ((unit32)(a2 - udata) >= size)
			break;
	}
	return 0;
	*/
}

unit32 LZdecompressmod(unit8* cdata, unit8* udata, unit32 size)
{
	unit8* src = cdata, * dst = udata, * window = udata;
	unit8 buff = 0, buff2 = 0, sign = 0;
	int buffsize = 0, round = 0, distance = 0;
	do
	{
		//��������
		sign = *src;
		src++;
		buffsize = sign & 0x0F;
		round = sign >> 4;
		if (buffsize == 0)//�����λ����ʾ��ʼ���ݵ�buffsize
		{
			do
			{
				buff = *src;
				buffsize <<= 7;
				buffsize |= buff;
				src++;
			} while ((buffsize & 1) == 0);
			buffsize >>= 1;
		}
		if (round == 0)//�����λ����ʾ������ѹ�Ļ���
		{
			do
			{
				buff = *src;
				round <<= 7;
				round |= buff;
				src++;
			} while ((round & 1) == 0);
			round >>= 1;
		}
		do
		{
			*dst = *src;
			buffsize--;
			src++;
			dst++;
		} while (buffsize != 0);
		if ((unit32)(dst - udata) >= size)
			return src - cdata;
		//��ʼ����
		do
		{
			sign = *src;
			src++;
			distance = sign & 0x0F;
			buffsize = sign >> 4;
			//��ѹ���Ǿ���LZѹ���ģ�ǰ�����,���ȣ������������ڴ�С�ƺ�û�����ƣ�ǰ�������Էǳ���
			if ((distance & 1) == 0)//�����λ
			{
				do
				{
					buff = *src;
					distance <<= 7;
					distance |= buff;
					src++;
				} while ((distance & 1) == 0);
			}
			distance >>= 1;
			if (buffsize == 0)//�����λ
			{
				do
				{
					buff = *src;
					buffsize <<= 7;
					buffsize |= buff;
					src++;
				} while ((buffsize & 1) == 0);
				buffsize >>= 1;
			}
			buffsize++;
			window = dst - distance - 1;
			if (window < udata)
				return src - cdata;
			if ((unit32)(dst + buffsize - udata) >= size)
				buffsize = udata + size - dst;
			do
			{
				*dst = *window;
				window++;
				dst++;
				buffsize--;
			} while (buffsize != 0);
			round--;
		} while (round != 0);
	} while ((unit32)(dst - udata) < size);
	return src - cdata;
}

unit32 make_size(unit8* data, unit32 size)
{
	unit32 count = 0, i = 3;
	memset(data, 0, 4);
	//��ѹ����ļ���������䵽0x10����ģ�����size���һλ��ȻΪ0
	size <<= 1;
	size |= 1;
	while (size & 0xFFFFFFFF)
	{
		data[i--] = size & 0xFF;
		count++;
		size >>= 8;
		size <<= 1;
	}
	return count;
}

unit32 make_buff_size(unit8* data, unit32 size)
{
	unit32 base = 0x100, count = 0, buff = 0;
	while (base < size)
	{
		base <<= 1;
		buff++;
	}
	buff <<= 1;
	buff |= 1;
	count = make_size(data, buff);
	return count;
}

unit32 get_match_size(unit8* data, unit32 buffsize, unit32 distance)
{
	unit32 count = 0, dstsize = 0;
	dstsize++;
	if(distance > 0x07)
	{
		count = make_size(data, distance);
		if (data[4 - count] >> 4 == 0)
			dstsize += count - 1;
		else
			dstsize += count;
	}
	if (buffsize > 0x0F)
	{
		count = make_size(data, buffsize);
		dstsize += count;
	}
	return dstsize;
}

//������
unit32 FakeLZcompress(unit8* cdata, unit8* udata, unit32 size)
{
	unit8* src = udata, * dst = cdata, sizebuff[4];
	unit32 count = 0, i = 0;
	count = make_size(sizebuff, size);
	*dst = 0x00;
	dst++;
	for (i = 4 - count; i < 4; i++)
	{
		*dst = sizebuff[i];
		dst++;
	}
	*dst = 0x01;
	dst++;
	for (i = 0; i < size; i++)
	{
		*dst = *src;
		dst++;
		src++;
	}
	return dst - cdata;
}

unit32 LZcompress(unit8* cdata, unit8* udata, unit32 size)
{
	/*
	�Աȳ���LZѹ���Ĳ�ͬ����
	1.ǰ�����û���ƣ�ֻҪ��������������
	2.���ƥ���ֽ����������Խ���1�ֽڣ�������û������ƣ����ƥ����2�ֽ�
	3.(����,ǰ�����)��Ԫ�����ռ1�ֽڣ��ɶ�̬��չ���������ƥ������ó�2�ֽ�
	4.����������(round)
	5.�ڵ�ǰ����ɵģ�δƥ���ֽڣ���ƥ���ֽڣ���������������δƥ���ֽ�����ֱ��������һ��ƥ�����ݣ��Ƚ����ߴ����������ѡ���Ž⣬
	�ﵽѡ���ˡ��ֲ������Ž��Ŀ�ģ���̬�滮��˼�롣

	��Ϊ���ϲ�ͬ������һֱ��ǰƥ�䵽��������ʼ������ѹ��Ч����Ӧ�û������ͣ�
	��ֱ���ó���LZѹ���ķ�ʽҲ�Ǽ��ݵģ���Ԫ���ʾ��ʽ��һ�ľ����á�
	���ﾡ����ԭԭ�㷨��
	
	���Ż��ĵط���
	�����㷨���ڱ������ң���������������LZϵ�㷨�õ��Ĺ�ϣͰ��ʽ��MIN_MATCHΪ2���Ǿ���ʼ����int hash[0x10000]�����飬���������ɹ�ϣͰ��ʽ��
	�߲��ұ����ƥ�䳤�ȵ���2��offset����ϣͰ�У�����ʱ�ȶ�ȡ��ϣͰ�ж���û��ƥ�䳤�ȵ���2�ģ�������������offset��������ƥ�䣬���û�������max_match < MIN_MATCH��
	��Ҫע��LZϵѹ�����ص��ǳ���+offset���Գ�����ǰλ�ã����¹�ϣͰ��û�У����ܵ�ǰλ��-1ȴ������ƥ��2�ֽڣ������ϣͰ���߲���ʱ��ѡһ��������ע�⴦�����������
	
	��������ĵ�����δʵ�֣�����ѹ������ĳЩ������ϱ�ԭ�㷨��һЩ��������ѹ��һЩ���ļ���ʱ����ԭѹ���㷨ѹ������Ҫ�ߣ�
	�ص㻹���ڵ����������ѡ��ʽ������Ϊ�����ã�ѡ�񡰾ֲ�������������ֲ������ö��壬ԭ�㷨�жϵķ�Χ̫С��ѡ�񡰾ֲ������ź���ܿ��ܸ�
	��һ�����ֲ���������������п��ܻ��ֱ��ѡ�񡰵�ǰ�����Ž�����Ҳ��Ϊʲô��ѹ��һЩ���ļ���ʱ����ԭѹ���㷨ѹ������Ҫ�ߡ�
	���㷨����ֱ��ѡ�񡰵�ǰ�����������
	*/
	//src:��ѹ������ dst:ѹ������ window:ǰ����Ҵ��� sizebuff:�����û���
	unit8 *src = udata, *dst = cdata, *window = udata, *no_match_offset = NULL, sizebuff[4];
	//dstsize:ѹ�����ݳ��� count:ƥ�䳤�� addr:��ַƫ���� max_match:���ƥ�䳤�� distance:ǰ����� no_match:δƥ���ֽڳ��� round:ƥ��ɹ�����
	unit32 dstsize = 0, count = 0, addr = 0, max_match = 0, distance = 0, no_match = 0, round = 0, i = 0;
	const unit32 MIN_MATCH = 2;//��Сƥ����
	//info:(����,ǰ�����)��Ԫ����������Ԫ�ظ�����Ϊround
	Linkinfo info = malloc(sizeof(Nodeinfo)), q = info;
	info->next = NULL;
	while ((unit32)(src - udata) < size)
	{
		window = src - 1;//ǰ��ƥ��ӵ�ǰλ�õ�ǰһ���ֽڿ�ʼ
		max_match = 0;
		distance = src - window;
		//���С��MIN_MATCH���ֽڲ���Ҫƥ�䣬�ų������ʡ�����һ�ֵ�ǰ��ƥ�伸�������ļ���С�Ĳ��ҹ��̣�����һ���򵥵��Ż�
		if (size - (unit32)(src - udata) >= MIN_MATCH)
		{
			//�����������ʺ�ѹ����ƥ�䴮�㷨�����Ͽ��µ�������ǰ����룬���ܴ���½�����Դ����
			while (window >= udata)
			{
				addr = 0;
				count = 0;
				//Ҫ���ж��Ƿ񳬹���Χ�����ж��Ƿ�ƥ�䣬���Сϸ��֮ǰ�������ˣ����跴�˹���������Խ��ȡֵ(*(src + addr))����ֱ�ӱ�����
				while (((unit32)(src + addr - udata) < size) && (*(window + addr) == *(src + addr)))
				{
					count++;
					addr++;
				}
				//ǰ�����Խ�ӽ�ƥ����ʼ��ַԽ�ã�����ԽС���ɵľ����ʾ�ֽ���Խ�̣����Բ��ܵ���
				//ͬʱ�����get_match_size()����ڵ��ڵ�ǰƥ�䳤�ȣ������ƥ�����ݷ�����ѹ�����ţ���Ҳ��Ϊʲôǰ�����Խ�ӽ�ƥ����ʼ��ַԽ��
				if (count > max_match && get_match_size(sizebuff, count - 1, src - window - 1) < count)
				{
					max_match = count;
					distance = src - window;
				}
				//�������ƥ������ô���������ԭʼ���ݣ���û��Ҫ�ټ���ǰ��ƥ���ˣ�����һ���򵥵��Ż�
				if ((unit32)(src + max_match - udata) == size)
					break;
				window--;
			}
		}
		//��һ���ֽڿ϶��ǲ�ƥ��ģ������״α�Ȼ��max_match < MIN_MATCH��֧
		if (max_match < MIN_MATCH)
		{
			//����ƥ��С��MIN_MATCH����ȴ����ƥ��ﵽMIN_MATCH�����ݣ������ο�ʼ���µ�һ�֣������ǰһ�ֵ�δƥ����ƥ������
			if (info->next != NULL)
			{
				round = 0;
				q = info;
				while (q->next != NULL)
				{
					round++;
					q = q->next;
				}
				//д(����,δƥ�����ݳ���)
				if (round <= 0x0F && no_match <= 0x0F)
				{
					*dst = (round << 4) | no_match;
					dst++;
					dstsize++;
				}
				else if (round <= 0x0F && no_match > 0x0F)
				{
					*dst = round << 4;
					dst++;
					dstsize++;
					count = make_size(sizebuff, no_match);
					for (i = 4 - count; i < 4; i++)
					{
						*dst = sizebuff[i];
						dst++;
						dstsize++;
					}
				}
				else if (round > 0x0F && no_match <= 0x0F)
				{
					*dst = no_match;
					dst++;
					dstsize++;
					count = make_size(sizebuff, round);
					for (i = 4 - count; i < 4; i++)
					{
						*dst = sizebuff[i];
						dst++;
						dstsize++;
					}
				}
				else if (round > 0x0F && no_match > 0x0F)
				{
					*dst = 0;
					dst++;
					dstsize++;
					count = make_size(sizebuff, no_match);
					for (i = 4 - count; i < 4; i++)
					{
						*dst = sizebuff[i];
						dst++;
						dstsize++;
					}
					count = make_size(sizebuff, round);
					for (i = 4 - count; i < 4; i++)
					{
						*dst = sizebuff[i];
						dst++;
						dstsize++;
					}
				}
				//дδƥ������
				while (no_match--)
				{
					*dst = *no_match_offset;
					dst++;
					no_match_offset++;
					dstsize++;
				}
				//дƥ��������Ϣ
				q = info;
				while (q->next != NULL)
				{
					Linkinfo tmp = q;
					q = q->next;
					if (tmp != info)
					{
						tmp->next = NULL;
						free(tmp);
					}
					q->buffsize--;
					q->distance--;
					if (q->buffsize <= 0x0F && q->distance <= 0x07)
					{
						*dst = (q->buffsize << 4) | ((q->distance << 1) | 1);
						dst++;
						dstsize++;
					}
					//q->distance > 0x07������£���ʾq->distance��4λ����ȫΪ0��������(distance & 1) == 0�����ֻ�õ���ȫΪ0һ�֣�
					//���԰�����7�����Ҳ�������������Ǽ��°���1�ֽ�ҲҪʡ����
					else if (q->buffsize <= 0x0F && q->distance > 0x07)
					{
						count = make_size(sizebuff, q->distance);
						if (sizebuff[4 - count] >> 4 == 0)
						{
							*dst = (q->buffsize << 4) | sizebuff[4 - count];
							dst++;
							dstsize++;
							for (i = 4 - count + 1; i < 4; i++)
							{
								*dst = sizebuff[i];
								dst++;
								dstsize++;
							}
						}
						else
						{
							*dst = q->buffsize << 4;
							dst++;
							dstsize++;
							for (i = 4 - count; i < 4; i++)
							{
								*dst = sizebuff[i];
								dst++;
								dstsize++;
							}
						}
					}
					else if (q->buffsize > 0x0F && q->distance <= 0x07)
					{
						*dst = (q->distance << 1) | 1;
						dst++;
						dstsize++;
						count = make_size(sizebuff, q->buffsize);
						for (i = 4 - count; i < 4; i++)
						{
							*dst = sizebuff[i];
							dst++;
							dstsize++;
						}
					}
					else if (q->buffsize > 0x0F && q->distance > 0x07)
					{
						count = make_size(sizebuff, q->distance);
						if (sizebuff[4 - count] >> 4 == 0)
						{
							*dst = sizebuff[4 - count];
							dst++;
							dstsize++;
							for (i = 4 - count + 1; i < 4; i++)
							{
								*dst = sizebuff[i];
								dst++;
								dstsize++;
							}
						}
						else
						{
							*dst = 0;
							dst++;
							dstsize++;
							for (i = 4 - count; i < 4; i++)
							{
								*dst = sizebuff[i];
								dst++;
								dstsize++;
							}
						}
						count = make_size(sizebuff, q->buffsize);
						for (i = 4 - count; i < 4; i++)
						{
							*dst = sizebuff[i];
							dst++;
							dstsize++;
						}
					}
				}
				//����
				info->next = NULL;
				q = info;
				no_match = 0;
			}
			//����ƥ��С��MIN_MATCH����������ռ�ƥ��С��MIN_MATCH�������У�
			//δƥ�仹����ƥ�䵫С��MIN_MATCH�Ķ��Ǽ�1
			no_match++;
			src++;
			no_match_offset = src - no_match;
		}
		else
		{
			Linkinfo p = malloc(sizeof(Nodeinfo));
			p->next = NULL;
			p->buffsize = max_match;
			p->distance = distance;
			q->next = p;
			q = q->next;
			src += max_match;
		}
	}
	//�������һ�ε�ʣ������
	round = 0;
	//������һ��������ƥ���꣬�������Ͳ�Ϊ0
	q = info;
	while (q->next != NULL)
	{
		round++;
		q = q->next;
	}
	//д(����,δƥ�����ݳ���)
	if (round <= 0x0F && no_match <= 0x0F)
	{
		*dst = (round << 4) | no_match;
		dst++;
		dstsize++;
	}
	else if (round <= 0x0F && no_match > 0x0F)
	{
		*dst = round << 4;
		dst++;
		dstsize++;
		count = make_size(sizebuff, no_match);
		for (i = 4 - count; i < 4; i++)
		{
			*dst = sizebuff[i];
			dst++;
			dstsize++;
		}
	}
	else if (round > 0x0F && no_match <= 0x0F)
	{
		*dst = no_match;
		dst++;
		dstsize++;
		count = make_size(sizebuff, round);
		for (i = 4 - count; i < 4; i++)
		{
			*dst = sizebuff[i];
			dst++;
			dstsize++;
		}
	}
	else if (round > 0x0F && no_match > 0x0F)
	{
		*dst = 0;
		dst++;
		dstsize++;
		count = make_size(sizebuff, no_match);
		for (i = 4 - count; i < 4; i++)
		{
			*dst = sizebuff[i];
			dst++;
			dstsize++;
		}
		count = make_size(sizebuff, round);
		for (i = 4 - count; i < 4; i++)
		{
			*dst = sizebuff[i];
			dst++;
			dstsize++;
		}
	}
	//���ʣ�µ����ݴ󲿷ֶ���δƥ�����ݣ���Ȼ����ƥ��������Ϣ����������ȻΪ0��
	//Ϊ0�ᴥ��make_size���ƣ�������Ҫ�ر���
	if (round == 0)
	{
		*dst = 0x01;
		dst++;
		dstsize++;
	}
	//дδƥ������
	while (no_match--)
	{
		*dst = *no_match_offset;
		dst++;
		no_match_offset++;
		dstsize++;
	}
	//дƥ��������Ϣ
	q = info;
	while (q->next != NULL)
	{
		Linkinfo tmp = q;
		q = q->next;
		if (tmp != info)
		{
			tmp->next = NULL;
			free(tmp);
		}
		q->buffsize--;
		q->distance--;
		if (q->buffsize <= 0x0F && q->distance <= 0x07)
		{
			*dst = (q->buffsize << 4) | ((q->distance << 1) | 1);
			dst++;
			dstsize++;
		}
		//q->distance > 0x07������£���ʾq->distance��4λ����ȫΪ0��������(distance & 1) == 0�����ֻ�õ���ȫΪ0һ�֣�
		//���԰�����7�����Ҳ�������������Ǽ��°���1�ֽ�ҲҪʡ����
		else if (q->buffsize <= 0x0F && q->distance > 0x07)
		{
			count = make_size(sizebuff, q->distance);
			if (sizebuff[4 - count] >> 4 == 0)
			{
				*dst = (q->buffsize << 4) | sizebuff[4 - count];
				dst++;
				dstsize++;
				for (i = 4 - count + 1; i < 4; i++)
				{
					*dst = sizebuff[i];
					dst++;
					dstsize++;
				}
			}
			else
			{
				*dst = q->buffsize << 4;
				dst++;
				dstsize++;
				for (i = 4 - count; i < 4; i++)
				{
					*dst = sizebuff[i];
					dst++;
					dstsize++;
				}
			}
		}
		else if (q->buffsize > 0x0F && q->distance <= 0x07)
		{
			*dst = (q->distance << 1) | 1;
			dst++;
			dstsize++;
			count = make_size(sizebuff, q->buffsize);
			for (i = 4 - count; i < 4; i++)
			{
				*dst = sizebuff[i];
				dst++;
				dstsize++;
			}
		}
		else if (q->buffsize > 0x0F && q->distance > 0x07)
		{
			count = make_size(sizebuff, q->distance);
			if (sizebuff[4 - count] >> 4 == 0)
			{
				*dst = sizebuff[4 - count];
				dst++;
				dstsize++;
				for (i = 4 - count + 1; i < 4; i++)
				{
					*dst = sizebuff[i];
					dst++;
					dstsize++;
				}
			}
			else
			{
				*dst = 0;
				dst++;
				dstsize++;
				for (i = 4 - count; i < 4; i++)
				{
					*dst = sizebuff[i];
					dst++;
					dstsize++;
				}
			}
			count = make_size(sizebuff, q->buffsize);
			for (i = 4 - count; i < 4; i++)
			{
				*dst = sizebuff[i];
				dst++;
				dstsize++;
			}
		}
	}
	info->next = NULL;
	return dstsize;
}