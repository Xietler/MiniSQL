#ifndef NODE_H
#define NODE_H

#include <iostream>
#include "Buffer_Manager.h"
#include "keytype.h"
#include "indexhead.h"

using namespace std;

struct Node{
	int blockNum;		//块编号 
	bool leafNode;		//是否是叶节点 
	int fanout;			//扇出 
	int numOfKeys;		//已有key的数量 
	KeyType* key;		//key数组 
	int* pointer;		//非叶节点pointer数组 
	int* index;			//叶节点pointer数组 
	int next;			//叶节点最后一位pointer
	Buffer_Manager* buffer;
	
	Node(int fanout, bool leafNode, Buffer_Manager* buffer);
	~Node();
	
	int FindPointer(KeyType V);				//非叶节点中查找V的位置 
	int FindIndex(KeyType V);					//叶节点中查找V的位置 
	int FindPosition(KeyType V);					//找到V应该插入位置 
	
	void InsertInLeaf(KeyType V, int P);		//叶节点插入新key 
	void InsertInNoLeaf(KeyType V, int N);	//非叶节点插入新key 
	
	void Delete(KeyType K);		//删除叶节点中的索引 
	
	
	void WriteNodeToBlock(string dbName, string fileName, IndexHead* head, int blockNum);
	void ReadNodeFromBlock(string dbName, string fileName, IndexHead* head, int blockNum);
	void ClearBlock(string dbName, string fileName, IndexHead* head, int blockNum);
};

#endif
