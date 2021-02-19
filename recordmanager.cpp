#include "recordmanager.h"

int RecordManager::GetTotalNum(string dbName, string tableName)
{
	string fileName = tableName+"_table";
	RecordHead head(buffer);
	head.ReadHead(dbName, fileName);
	return head.totalNum;
}

int RecordManager::GetNumOfRecords(string dbName, string tableName)
{
	string fileName = tableName+"_table";
	RecordHead head(buffer);
	head.ReadHead(dbName, fileName);
	return head.NumOfRecords;
}

void RecordManager::Create(string dbName, string tableName, int recordLength)
{
	string fileName = tableName+"_table";
	RecordHead head(buffer);
	
	head.firstEmptyNum = 0;
	head.NumOfRecords = 0;
	head.totalNum = 0;
	head.recordLength = recordLength;
	head.maxNumOfEachBlock = 4096/recordLength;
	head.NumOfBlocks = 1;
	
	head.WriteHead(dbName,fileName);
}

char* RecordManager::Select(string dbName, string tableName, int recordNum)
{
	string fileName = tableName+"_table";
	RecordHead head(buffer);
	
	head.ReadHead(dbName, fileName);
	
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, recordNum/head.maxNumOfEachBlock+1 );
	
	bip->byteNum=4096;
	int bp = recordNum%head.maxNumOfEachBlock;
	bp *= head.recordLength;
	
	char* p1 = (char*)(bip->cBlock+bp);
	char* s;
	s = new char[head.recordLength+1];
	int j;
	for(j=0; j<head.recordLength; p1++,j++)
	{
		s[j]=*p1;
	}
	s[j]='\0';
	
	return s;
} 


void RecordManager::IncreasEmptyRecord(string dbName, string fileName, RecordHead* head)
{
	BLOCKINFO* bip;
	int blockNum = head->totalNum/head->maxNumOfEachBlock+1;
	
	int bp = head->totalNum%head->maxNumOfEachBlock;
	
	if( bp==head->maxNumOfEachBlock-1 )
	{
		bip = buffer->get_file_block(dbName, fileName, blockNum+1);
		bip->byteNum=4096;
		head->NumOfBlocks++;
		bp = 0;
	}
	else
	{
		bip = buffer->get_file_block(dbName, fileName, blockNum );
		bip->byteNum=4096;
		bp *= head->recordLength;
	}
	
	char* p = (char*)(bip->cBlock+bp);
	*p = 'N';
	bip->dirtyBit = 1;
	
	head->firstEmptyNum++;
	head->totalNum++;
	
}


int RecordManager::GetFirstEmptyNum(string dbName, string fileName, RecordHead* head)
{	
	int num = head->firstEmptyNum;
	
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, num/head->maxNumOfEachBlock+1 );
	
	if( head->firstEmptyNum==head->totalNum )
	{
		IncreasEmptyRecord(dbName ,fileName, head);
	}
	else
	{
		int bp = num%head->maxNumOfEachBlock;
		bp *= head->recordLength;
		int *p = (int*)(bip->cBlock+bp+1);
		head->firstEmptyNum = *p;
	}
	
	return num;
		
}


int RecordManager::Insert(string dbName, string tableName, char* record)
{
	string fileName = tableName+"_table";
	RecordHead head(buffer);
	
	head.ReadHead(dbName, fileName);
	
	int recordNum = GetFirstEmptyNum(dbName, fileName,  &head );
	
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, recordNum/head.maxNumOfEachBlock+1 );
	bip->byteNum=4096;
	int bp = recordNum%head.maxNumOfEachBlock;
	bp *= head.recordLength;
	
	char* p = (char*)(bip->cBlock+bp);
	for(int i=0; i<head.recordLength; i++)
	{
		p[i] = record[i];	
	}
	bp += (head.recordLength)*sizeof(char);

	bip->dirtyBit = 1;
	head.NumOfRecords++;
	
	head.WriteHead(dbName, fileName);
	
	return recordNum;
}


void RecordManager::Delete(string dbName, string tableName, int recordNum)
{
	string fileName = tableName+"_table";
	RecordHead head(buffer);
	
	head.ReadHead(dbName, fileName);
	
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, recordNum/head.maxNumOfEachBlock+1 );
	bip->byteNum=4096;
	int bp = recordNum%head.maxNumOfEachBlock;
	bp *= head.recordLength;
	
	char* p1 = (char*)(bip->cBlock+bp);
	*p1 = 'N';
	bip->dirtyBit = 1;
	bp+=1;
	int* p2 = (int*)(bip->cBlock+bp);
	*p2 = head.firstEmptyNum;
	head.firstEmptyNum = recordNum;
	head.NumOfRecords--;
	
	head.WriteHead(dbName, fileName);
}
/*
int main()
{
	Buffer_Manager buffer;
	RecordManager rm(&buffer);
	RecordHead head(&buffer);
	char* p;
	p = new char[100];
	for(int i=0; i<100; i++)
	{
		if( i==0 )
			p[i] = 'Y';
		else if(i<20)
			p[i] = '0';
		else if ( i<80 )
			p[i] = 0;
		else
			p[i] = '0';
	}
	//rm.Create("lzy","test",100);
	cout<<"insert into"<<rm.Insert("lzy","test",p)<<endl;
	//rm.Delete("lzy","test",40);
	cout << rm.Select("lzy","test",2)<<endl;
	head.ReadHead("lzy","test_table"); 
	cout<<"firstEmptyNum"<<head.firstEmptyNum<<endl
		<<"NumOfRecords"<<head.NumOfRecords<<endl
		<<"totalNum"<<head.totalNum<<endl
		<<"recordLength"<<head.recordLength<<endl
		<<"maxNumOfEachBlock"<<head.maxNumOfEachBlock<<endl
		<<"NumOfBlocks"<<head.NumOfBlocks;
	buffer.close_database("lzy");
}
*/
