#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include "node.h"
#include "keytype.h"
#include "indexhead.h"

using namespace std;

struct SearchArray
{
	int num;			
	int* indexNum;		
	
	SearchArray(int n)
	{
		indexNum = new int[n];
		num = 0;
	}
};

struct BPlusTree
{
	int fanout;
	string dbName;
	string fileName;
	Node* root;
	IndexHead head;
	Buffer_Manager* buffer;
	
	
	BPlusTree(int fanout, string dbName, string fileName, Buffer_Manager* buffer):head(buffer)
	{
		this->fanout = fanout;
		this->dbName = dbName;
		this->fileName = fileName;
		root = NULL;
		this->buffer = buffer;
	}
	
	void IncreaseEmptyBlock(string dbName ,string fileName, int blockNum);
	int GetEmptyBlock();
	void SetRoot(Node* N); 
	Node* FindLeaf(KeyType K);					//���в���key
	int FindParent(int N, KeyType K);		//���в��ҽڵ�ĸ��ڵ� 
	SearchArray* Search(int op, KeyType K);
	
	void Insert(KeyType K, int P);				//���в��������� 
	void InsertInParent(Node* N, KeyType VV, Node* NN);	//��Ҷ�ڵ������key
	
	
	void Delete(KeyType K, int P);				//ɾ������ 
	bool IfCanMerge(int num1, int num2, KeyType K);			
	void MergeLeaf(Node* S, Node* N);
	void MergeNoLeaf(Node* S, KeyType PK, Node* N);
	void DeleteEntry(Node* N, KeyType K);
};

#endif
