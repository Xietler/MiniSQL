#ifndef INDEXHEAD_H
#define INDEXHEAD_H

#include <iostream>
#include "Buffer_Manager.h"

using namespace std;

struct IndexHead
{
	int firstEmptyBlock;	//��һ���տ�� 
	int root;				//���ڵ��� 
	int numOfBlocks;		//�����Ŀ 
	int firstLeafBlock;		//��һ��Ҷ�ڵ��� 
	int numOfLeaves;		//Ҷ�ڵ���� 
	Buffer_Manager* buffer;
	
	IndexHead(Buffer_Manager* buffer)
	{
		this->buffer = buffer;
	}
	void WriteHeadToBlock(string dbName, string fileName);
	void ReadHeadFromBlock(string dbName, string fileName);
};

#endif
