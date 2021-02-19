#include"indexmanager.h" 

void IndexManager::Create(string dbName, string tableName, string attribute)
{
	string fileName = tableName+"_"+attribute+"_index";
	IndexHead head(buffer);
	head.firstEmptyBlock = 1;
	head.firstLeafBlock = 0;
	head.numOfBlocks = 1;
	head.numOfLeaves = 0;
	head.root = 0;
	
	head.WriteHeadToBlock(dbName,fileName);
}

SearchArray* IndexManager::Search(string dbName, string tableName, int op, KeyInfo* ki)
{
	string fileName = tableName+"_"+ki->attribute+"_index";
	BPlusTree tree(fanout, dbName, fileName, buffer);
	
	tree.head.ReadHeadFromBlock(dbName, fileName);
	
	return tree.Search(op, ki->value);
} 

void IndexManager::Insert(string dbName, string tableName, KeyInfo* ki)
{
	string fileName = tableName+"_"+ki->attribute+"_index";
	
	BPlusTree tree(fanout, dbName, fileName, buffer);
	tree.head.ReadHeadFromBlock(dbName, fileName);
	
	tree.Insert(ki->value, ki->indexNum);
}

void IndexManager::Delete(string dbName, string tableName, KeyInfo* ki)
{
	string fileName = tableName+"_"+ki->attribute+"_index";
	
	BPlusTree tree(fanout, dbName, fileName, buffer);
	tree.head.ReadHeadFromBlock(dbName, fileName);
	
	tree.Delete(ki->value, ki->indexNum);
}

/*
int main()
{
	Buffer_Manager buffer;
	IndexManager im(&buffer);
	SearchArray* sa;
	
	string dbName="lzy";
	string tableName="test";
	struct KeyInfo i[10];
	i[0].attribute = "address";
	i[0].value.type = 'c';
	i[0].value.length = 10;
	i[0].value.stringKey = "abc";
	i[0].indexNum = 8;
	
	i[1].attribute = "address";
	i[1].value.type = 'c';
	i[1].value.length = 10;
	i[1].value.stringKey = "abcde";
	i[1].indexNum = 6;
	
	i[2].attribute = "address";
	i[2].value.type = 'c';
	i[2].value.length = 10;
	i[2].value.stringKey = "abcd";
	i[2].indexNum = 7;
	
	i[3].attribute = "address";
	i[3].value.type = 'c';
	i[3].value.length = 10;
	i[3].value.stringKey = "ab";
	i[3].indexNum = 9;
	
	i[4].attribute = "address";
	i[4].value.type = 'c';
	i[4].value.length = 10;
	i[4].value.stringKey = "a";
	i[4].indexNum = 10;
	
//	im.Create(dbName, tableName, "address");
//	im.Insert(dbName, tableName, &i[0]);
//	im.Insert(dbName, tableName, &i[1]);
//	im.Insert(dbName, tableName, &i[2]);
//	im.Insert(dbName, tableName, &i[3]);
//	im.Insert(dbName, tableName, &i[4]);
//
//	im.Delete(dbName, tableName, &i[4]);
//	im.Delete(dbName, tableName, &i[3]);
//	im.Delete(dbName, tableName, &i[2]);
//	im.Delete(dbName, tableName, &i[1]);
//	im.Delete(dbName, tableName, &i[0]);
	
//	6= 5> 4< 3>= 2<=
	sa = im.Search(dbName, tableName, 2, &i[2]);
	
	if( (sa->num)>0 )
	{
		for(int i=0; i<sa->num; i++)
		{
			cout<<sa->indexNum[i]<<" ";
		}
		cout<<"\n";
	}
	else
	{
		cout<<"404 Not Found!";
	}
	
	buffer.close_database(dbName);
}
*/
