#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#define BLOCK_LEN 4096
#define MAX_FILE_ACTIVE 5
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#define MAX_BLOCK 40

#define BYTE unsigned char

using namespace std;

typedef struct blockInfo BLOCKINFO;
typedef struct fileInfo FILEINFO;

struct blockInfo {
	int blockNum;  // the block number of the block in the file,
				   // which indicate it when it be newed
	bool dirtyBit;  // 0 -> false�� 1 -> indicate dirty, write back
	BLOCKINFO *fnext;  	// the pointer point to next block
	BLOCKINFO *bnext;
	FILEINFO *file;  // the pointer point to the file, which the block belongs to
	int byteNum;  // the number of BYTES in the block
	BYTE* cBlock;  // the array space for storing the records in the block in buffer
	int iTime;  // it indicate the age of the block in use
	int lock;  // prevent the block from replacing
};

struct fileInfo {
	string fileName;  // the name of the file
	FILEINFO *next;  // the pointer points to the next file
	BLOCKINFO *firstBlock;  // point to the first block within the file
};


//Buffer�࣬Ӧ��API�ж��壬��index_manager��record_manager���е��á�
//Ҫ�㣺
//1.����޸���blockInfo�е�cBlock��һ���ǵø���byteNum��dirtyBit��ֵ��
class Buffer_Manager
{
private:
	FILEINFO *fileHandler;
	BLOCKINFO *blockHandler;
	int blockSize;
	int fileSize;
	char path[100];
	unsigned int Time;
public:
	Buffer_Manager();
	//��ñ�ͷ��Ϣ���������ݿ�����������
	FILEINFO* get_file_info(string DB_Name, string Table_Name);
	//���һ��block��Ϣ���������ݿ�������������š�
	BLOCKINFO* get_file_block(string DB_Name, string Table_Name, int blockNum);
	//����һ�����blcok�����������ִ�в��������ʱ�����һ��block�Ų��£�
	//��Ҫ������block��ʱ����ã��ú����᷵��һ���յ�block������Ŀ���ǵ�ǰ
	//����ż�һ��
	BLOCKINFO* inc_file_block(string DB_Name, string Table_Name, int blockNum);
	//�ر����ݿ⣬���˳����ݿ��ʱ�����
	void close_database(string DB_Name);
	//��ɾ�����֮ǰ��Ҫ�ȵ���delete_file���ñ���ڴ������
	void delete_file(string DB_Name, string Table_Name);

	//�������ڲ��������ⲿ����Ҫֱ�ӵ���
	//�ر��ļ�
	void close_file(string DB_Name, FILEINFO* fip);
	//�ҵ�һ����
	BLOCKINFO* findBlock(string DB_Name, string Table_Name);
	//�ҵ�һ���ļ�
	FILEINFO* findFile(string DB_Name, string Table_Name);
};

#endif
