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
	bool dirtyBit;  // 0 -> false， 1 -> indicate dirty, write back
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


//Buffer类，应在API中定义，由index_manager和record_manager进行调用。
//要点：
//1.如果修改了blockInfo中的cBlock，一定记得更新byteNum和dirtyBit的值。
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
	//获得表头信息，传入数据库名，表名。
	FILEINFO* get_file_info(string DB_Name, string Table_Name);
	//获得一个block信息，传入数据库名，表名，块号。
	BLOCKINFO* get_file_block(string DB_Name, string Table_Name, int blockNum);
	//增加一个表的blcok，这个函数在执行插入操作的时候，最后一个block放不下，
	//需要新增加block的时候调用，该函数会返回一个空的block。输入的块号是当前
	//最大块号加一。
	BLOCKINFO* inc_file_block(string DB_Name, string Table_Name, int blockNum);
	//关闭数据库，在退出数据库的时候调用
	void close_database(string DB_Name);
	//在删除表点之前需要先调用delete_file将该表的内存清理掉
	void delete_file(string DB_Name, string Table_Name);

	//下面是内部函数，外部不需要直接调用
	//关闭文件
	void close_file(string DB_Name, FILEINFO* fip);
	//找到一个块
	BLOCKINFO* findBlock(string DB_Name, string Table_Name);
	//找到一个文件
	FILEINFO* findFile(string DB_Name, string Table_Name);
};

#endif
