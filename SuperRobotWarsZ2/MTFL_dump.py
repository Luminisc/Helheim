# -*- coding:utf-8 -*-
#用于导出文件头为MTFLz2_2文件的文本
#by Darkness-TX 2023.03.07
import struct
import os

def walk(adr):
	mylist=[]
	for root,dirs,files in os.walk(adr):
		for name in files:
			adrlist=os.path.join(root, name)
			src = open(adrlist,'rb')
			data = src.read()
			src.close()
			if data[:8] == b'MTFLz2_2' and data.find(b'jstr') != -1:
				mylist.append(adrlist)
	return mylist

def byte2int(byte):
	return struct.unpack('L',byte)[0]

def int2byte(num):
	return struct.pack('L',num)

def dumpstr(src):
	bstr = b''
	c = src.read(1)
	while c != b'\x00' and c != b'':
		bstr += c
		c = src.read(1)
	return bstr.decode('932')

def FormatString(string, count, offset):
	res = "○%08d|%08X○%s\n●%08d|%08X●%s\n\n"%(count, offset, string, count, offset, string)
	return res

def main():
	fn_lst = walk('EVENT')
	for fn in fn_lst:
		print(fn)
		src = open(fn,'rb')
		src.seek(8,os.SEEK_SET)
		header_len = byte2int(src.read(4))
		version = src.read(4)
		filesize = byte2int(src.read(4))#加上header_len才是文件大小	Add header_len to the file size
		print('version:%s header_len:0x%X filesize:0x%X'%(version.decode('932'),header_len,filesize+header_len))
		src.seek(header_len,os.SEEK_SET)
		#总块的信息，文件名带SDEMO的file_block就是sdem，带DLGEV的file_block就是dlev
		#0165的文件是kwrb
		#The information of the total block, the file name with SDEMO file_block is sdem, and the file_block with DLGEV is dlev
		#0165's file is kwrb
		file_block_name = src.read(4)
		file_block_size = byte2int(src.read(4))
		file_block_header_size = byte2int(src.read(4))
		file_block_data_size = byte2int(src.read(4))
		print('\t%s block_size:0x%X block_header_size:0x%X block_data_size:0x%X'%(file_block_name.decode('932'),file_block_size,file_block_header_size,file_block_data_size))
		src.seek(header_len + file_block_header_size,os.SEEK_SET)
		#Start traversing the block
		while src.tell() < file_block_size + header_len:
			pos = src.tell()
			block_name = src.read(4)
			block_size = byte2int(src.read(4))
			block_header_size = byte2int(src.read(4))
			block_data_size = byte2int(src.read(4))
			decoded_block_name = block_name.decode('932')
			if decoded_block_name == 'evtb':
				data = src.read(block_data_size)
			elif decoded_block_name == 'evnt':#Text pointer
				block_group_num = byte2int(src.read(4))#Each group length is 0x20
				#data = src.read(block_data_size)
				dst = open(fn+'_point.txt','w',encoding='utf16')
				for i in range(0,block_group_num):
					src.seek(0x10,os.SEEK_CUR)
					data = src.read(4)
					dst.write('%08d|%08X|%08X\n'%(i,src.tell() - 4 - pos - block_header_size,byte2int(data)))
					src.seek(0x0C,os.SEEK_CUR)
				dst.flush()
			elif decoded_block_name == 'lkke':#The address length is fully recorded, only one file has this block
				block_group_num = byte2int(src.read(4))#Number of groups
				dst = open(fn+'_point.txt','w',encoding='utf16')
				for  i in range(0,int(block_data_size / 8)):
					str_off = byte2int(src.read(4))
					str_size = byte2int(src.read(4))
					if str_off != 0xFFFFFFFF:#0xFFFFFFFF followed by group number
						dst.write('%08d|%08X|%08X|%08X\n'%(i,src.tell() - 8 - pos - block_header_size,str_off,str_size))
				dst.flush()
			elif decoded_block_name == 'kywd':
				block_unk1 = byte2int(src.read(4))
				block_unk2 = byte2int(src.read(4))
				data = src.read(block_data_size)
			elif decoded_block_name == 'jstr':#text
				data = src.read(block_data_size)
				dst = open(fn+'.bin','wb')
				for i in range(0,len(data)):
					if data[i] == 0 or data[i] == 0x7A:
						dst.write(struct.pack('B',data[i]))
					else:
						dst.write(struct.pack('B',data[i] ^ 0x7A))
				dst.flush()
				dst.close()
				jstr = open(fn+'.bin','rb')
				jstr.seek(0,os.SEEK_END)
				jstr_size = jstr.tell()
				jstr.seek(0,os.SEEK_SET)
				count = 0
				dst = open(fn+'.txt','w',encoding='utf16')
				while jstr.tell() < jstr_size:
					offset = jstr.tell()
					string = dumpstr(jstr)
					dst.write(FormatString(string.replace('\n','\\n'),count,offset))
					count += 1
				dst.flush()
				jstr.close()
				os.remove(fn+'.bin')
			else:
				print('\tUnknown block name:%s'%(decoded_block_name))
				os.system('pause')
				src.seek(pos + block_header_size,os.SEEK_SET)
				data = src.read(block_data_size)
			print('\t\t%s block_size:0x%X block_header_size:0x%X block_data_size:0x%X'%(decoded_block_name,block_size,block_header_size,block_data_size))
		block_name = src.read(4)
		if block_name.decode('932') != 'ENDo':
			print('\tENDo not read!offset:0x%X'%(src.tell() - 4))
			os.system('pause')

if '__main__' == __name__:
	main()