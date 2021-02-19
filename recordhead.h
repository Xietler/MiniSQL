#ifndef RECORDHEAD_H
#define	RECORDHEAD_H

#include <iostream>
#include <string>
#include "Buffer_Manager.h"

using namespace std;

struct RecordHead
{
	int firstEmptyNum;
	int NumOfRecords;
	int totalNum;
	int recordLength;
	int maxNumOfEachBlock;
	int NumOfBlocks;
	Buffer_Manager* buffer;
	
	RecordHead(Buffer_Manager* buffer) 
	{
		this->buffer = buffer;
	}
	
	void ReadHead(string dbName, string fileName);
	void WriteHead(string dbName, string fileName);
};

#endif
