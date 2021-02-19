#include "indexhead.h"

void IndexHead::WriteHeadToBlock(string dbName, string fileName)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName ,fileName, 0);
	
	bip->byteNum = 4096;
	int bp=0;
	
	//写入'H'表示IndexHead块 
	char* p1 = (char*)(bip->cBlock);
	*p1 = 'H';
	bp += sizeof(char);
	bip->dirtyBit = 1;
	
	//写入第一个空块号
	int* p2 = (int*)(bip->cBlock+bp);
	*p2 = firstEmptyBlock;
	bp += sizeof(int);
	
	//写入根节点块号 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = root;
	bp += sizeof(int);
	
	//写入块数
	p2 = (int*)(bip->cBlock+bp);
	*p2 = numOfBlocks;
	bp += sizeof(int);
	
	//写入叶节点块数 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = numOfLeaves;
	bp += sizeof(int);
	
	//写入第一个叶节点块号  
	p2 = (int*)(bip->cBlock+bp);
	*p2 = firstLeafBlock;
	bp += sizeof(int);
	
}

void IndexHead::ReadHeadFromBlock(string dbName, string fileName)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, 0);
	
	bip->byteNum=4096;
	int bp=0;
	
	char* p1 = (char*)(bip->cBlock);
	if( *p1=='H' )
	{//是IndexHead块 
		int *p2 = (int*)(bip->cBlock+1);
		firstEmptyBlock = *p2;
		p2 = (int*)(bip->cBlock+5);
		root = *p2;
		p2 = (int*)(bip->cBlock+9);
		numOfBlocks = *p2;
		p2 = (int*)(bip->cBlock+13);
		numOfLeaves = *p2;
		p2 = (int*)(bip->cBlock+17);
		firstLeafBlock = *p2;
		return;
	}
	else
	{//出错 
		cout<<"Read index head error";
	}
}
