#include "indexhead.h"

void IndexHead::WriteHeadToBlock(string dbName, string fileName)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName ,fileName, 0);
	
	bip->byteNum = 4096;
	int bp=0;
	
	//д��'H'��ʾIndexHead�� 
	char* p1 = (char*)(bip->cBlock);
	*p1 = 'H';
	bp += sizeof(char);
	bip->dirtyBit = 1;
	
	//д���һ���տ��
	int* p2 = (int*)(bip->cBlock+bp);
	*p2 = firstEmptyBlock;
	bp += sizeof(int);
	
	//д����ڵ��� 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = root;
	bp += sizeof(int);
	
	//д�����
	p2 = (int*)(bip->cBlock+bp);
	*p2 = numOfBlocks;
	bp += sizeof(int);
	
	//д��Ҷ�ڵ���� 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = numOfLeaves;
	bp += sizeof(int);
	
	//д���һ��Ҷ�ڵ���  
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
	{//��IndexHead�� 
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
	{//���� 
		cout<<"Read index head error";
	}
}
