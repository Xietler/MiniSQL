#ifndef RECORDMANAGER_H
#define	RECORDMANAGER_H

#include <iostream>
#include <string>
#include "Buffer_Manager.h"
#include "recordhead.h"

using namespace std;

struct RecordManager
{
	Buffer_Manager* buffer;
	
	RecordManager(Buffer_Manager* buffer) 
	{
		this->buffer = buffer;
	}
	
	int GetTotalNum(string dbName, string tableName);
	int GetNumOfRecords(string dbName, string tableName);
	void Create(string dbName, string tableName, int recordLength);
	char* Select(string dbName, string tableName, int recordNum);
	void IncreasEmptyRecord(string dbName, string fileName, RecordHead* head);
	int GetFirstEmptyNum(string dbName, string tableName, RecordHead* head);
	int Insert(string dbName, string tableName, char* record);
	void Delete(string dbName, string tableName, int recordNum);

};

#endif 
