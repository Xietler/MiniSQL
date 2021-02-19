#ifndef INDEXHEAD_H
#define INDEXHEAD_H

#include <iostream>
#include "Buffer_Manager.h"

using namespace std;

struct IndexHead
{
	int firstEmptyBlock;	//第一个空块号 
	int root;				//根节点块号 
	int numOfBlocks;		//块的数目 
	int firstLeafBlock;		//第一个叶节点块号 
	int numOfLeaves;		//叶节点个数 
	Buffer_Manager* buffer;
	
	IndexHead(Buffer_Manager* buffer)
	{
		this->buffer = buffer;
	}
	void WriteHeadToBlock(string dbName, string fileName);
	void ReadHeadFromBlock(string dbName, string fileName);
};

#endif
