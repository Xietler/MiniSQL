#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <iostream>
#include "bplustree.h"
#include "keytype.h"

using namespace std;


struct KeyInfo
{
	string attribute;			//属性名 
	KeyType value;				//key值 
	int indexNum;				//记录的ID
};


struct IndexManager
{
	int fanout;
	Buffer_Manager* buffer;
	IndexManager(Buffer_Manager* buffer)
	{
		fanout = 3;
		this->buffer = buffer;
	}
	void Create(string dbName, string tableName, string attribute);
	SearchArray* Search(string dbName, string tableName, int op, KeyInfo* ki); 
	void Insert(string dbName, string tableName, KeyInfo* ki);
	void Delete(string dbName, string tableName, KeyInfo* ki);
	
};

#endif
