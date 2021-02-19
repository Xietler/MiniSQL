#ifndef NODE_H
#define NODE_H

#include <iostream>
#include "Buffer_Manager.h"
#include "keytype.h"
#include "indexhead.h"

using namespace std;

struct Node{
	int blockNum;		//���� 
	bool leafNode;		//�Ƿ���Ҷ�ڵ� 
	int fanout;			//�ȳ� 
	int numOfKeys;		//����key������ 
	KeyType* key;		//key���� 
	int* pointer;		//��Ҷ�ڵ�pointer���� 
	int* index;			//Ҷ�ڵ�pointer���� 
	int next;			//Ҷ�ڵ����һλpointer
	Buffer_Manager* buffer;
	
	Node(int fanout, bool leafNode, Buffer_Manager* buffer);
	~Node();
	
	int FindPointer(KeyType V);				//��Ҷ�ڵ��в���V��λ�� 
	int FindIndex(KeyType V);					//Ҷ�ڵ��в���V��λ�� 
	int FindPosition(KeyType V);					//�ҵ�VӦ�ò���λ�� 
	
	void InsertInLeaf(KeyType V, int P);		//Ҷ�ڵ������key 
	void InsertInNoLeaf(KeyType V, int N);	//��Ҷ�ڵ������key 
	
	void Delete(KeyType K);		//ɾ��Ҷ�ڵ��е����� 
	
	
	void WriteNodeToBlock(string dbName, string fileName, IndexHead* head, int blockNum);
	void ReadNodeFromBlock(string dbName, string fileName, IndexHead* head, int blockNum);
	void ClearBlock(string dbName, string fileName, IndexHead* head, int blockNum);
};

#endif
