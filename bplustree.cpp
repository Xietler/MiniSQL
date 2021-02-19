#include "bplustree.h"

#include <math.h>

void BPlusTree::IncreaseEmptyBlock(string dbName ,string fileName, int blockNum)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName ,fileName, blockNum);
	
	bip->byteNum = 4096;
	int bp=0;
	
	char* p1 = (char*)(bip->cBlock);
	*p1 = 'N';
	bip->dirtyBit = 1;
	bp += sizeof(char);
	
}


int BPlusTree::GetEmptyBlock()
{
	int blockNum = head.firstEmptyBlock;

	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, blockNum);
	bip->byteNum=4096;
	int bp=0;
	
	if( head.firstEmptyBlock==head.numOfBlocks )
	{
		IncreaseEmptyBlock(dbName ,fileName, head.firstEmptyBlock+1);
		head.firstEmptyBlock++;
		head.numOfBlocks++;
	}
	else
	{
		int *p = (int*)(bip->cBlock+1);
		head.firstEmptyBlock = *p;
	}
	
	return blockNum;
}


void BPlusTree::SetRoot(Node* N)
{
	root = N;
}


Node* BPlusTree::FindLeaf(KeyType K)
{
	Node* N = root;
	while ( false==N->leafNode )
	{
		int blockNum = N->FindPointer(K);
		N->key[0].type = K.type;
		N->key[0].length = K.length;
		N->ReadNodeFromBlock(dbName, fileName, &head, blockNum);
	}
	return N;
}


int BPlusTree::FindParent(int N, KeyType K)
{
	Node P(fanout, false, buffer);
	Node C(fanout, false, buffer);
	P.key[0].type = K.type;
	P.key[0].length = K.length;
	P.ReadNodeFromBlock(dbName, fileName, &head, head.root);
	int blockNum = P.FindPointer(K);
	C.key[0].type = K.type;
	C.key[0].length = K.length;
	C.ReadNodeFromBlock(dbName, fileName, &head, blockNum);
	while ( C.blockNum!=N )
	{
		P = C;
		blockNum = P.FindPointer(K);
		C.ReadNodeFromBlock(dbName, fileName, &head, blockNum);
	}
	return P.blockNum;
}


SearchArray* BPlusTree::Search(int op, KeyType K)
{
	SearchArray* sa = new SearchArray(head.numOfLeaves * fanout);
	
	if( 0==head.root)
		sa->num = -1;
	else
	{		
		Node* N;
		N = new Node(fanout, false, buffer);
		N->key[0].type = K.type;
		N->key[0].length = K.length;
		N->ReadNodeFromBlock(dbName, fileName, &head, head.root);
		SetRoot(N);
		N = FindLeaf(K);
		
		switch (op)
		{
			case 6://"="
			{
				sa->indexNum[0] = N->FindIndex(K);
				if( sa->indexNum[0]!=-1 )
					sa->num = 1;
				break;
			}
			case 5://">"
			{
				int start = N->FindPosition(K);
				if( N->key[start]==K )
				{
					if( start<N->numOfKeys )
						start++;
					else if( N->next!=-1 )
					{
						N->ReadNodeFromBlock(dbName, fileName, &head, N->next);
						start = 0;
					}
					else
					{
						return sa;
					}
				}
				int num=0;
				while(true)
				{
					while( start<N->numOfKeys )
					{
						sa->indexNum[num] = N->index[start];
						num++;
						start++;					
					}
					start = 0;
					if( N->next!=-1)
						N->ReadNodeFromBlock(dbName, fileName, &head, N->next);
					else
						break;
				}
				sa->num = num;
				break;
			}
			case 3://">="
			{
				int start = N->FindPosition(K);
				int num=0;
				while(true)
				{
					while( start<N->numOfKeys )
					{
						sa->indexNum[num] = N->index[start];
						num++;
						start++;					
					}
					start = 0;
					if( N->next!=-1)
						N->ReadNodeFromBlock(dbName, fileName, &head, N->next);
					else
						break;
				}
				sa->num = num;
				break;
			}
			case 4://"<"
			{
				N->ReadNodeFromBlock(dbName, fileName, &head, head.firstLeafBlock);
				int num = 0;
				int pos = 0;
				while( N->key[pos]<K )
				{
					sa->indexNum[num] = N->index[pos];
					num++;
					if( pos+1<N->numOfKeys)
						pos++;
					else if( N->next!=-1 )
					{
						pos = 0;
						N->ReadNodeFromBlock(dbName, fileName, &head, N->next);
					}
					else
					{
						break;
					}
				}
				sa->num = num;
				break;
			}
			case 2://"<="
			{
				N->ReadNodeFromBlock(dbName, fileName, &head, head.firstLeafBlock);
				int num = 0;
				int pos = 0;
				while( N->key[pos]<=K )
				{
					sa->indexNum[num] = N->index[pos];
					num++;
					if( pos+1<N->numOfKeys)
						pos++;
					else if( N->next!=-1 )
					{
						pos = 0;
						N->ReadNodeFromBlock(dbName, fileName, &head, N->next);
					}
					else
					{
						break;
					}
				}
				sa->num = num;
				break;
			}
		}
		delete N;
	}
	return sa;
}


void BPlusTree::InsertInParent(Node* N, KeyType VV, Node* NN)
{
	if( head.root==N->blockNum )
	{
		root = new Node(fanout, false, buffer);
		root->key[0] = VV;
		root->pointer[0] = N->blockNum;
		root->pointer[1] = NN->blockNum;
		root->numOfKeys = 1;
		
		head.root = GetEmptyBlock();
		root->WriteNodeToBlock(dbName, fileName, &head, head.root);			
		head.WriteHeadToBlock(dbName, fileName);
		
		delete root;
		return;
	}
	
	int blockNum = FindParent(N->blockNum, VV);
	Node *P;
	P = new Node(fanout, false, buffer);
	P->key[0].type = VV.type;
	P->key[0].length = VV.length;
	P->ReadNodeFromBlock(dbName, fileName, &head, blockNum);
	if( P->numOfKeys<fanout-1)
	{
		P->InsertInNoLeaf(VV, NN->blockNum);
		P->WriteNodeToBlock(dbName, fileName, &head, P->blockNum);
	}
	else
	{
		Node* T = new Node(fanout+1, false, buffer);
		//把P中索引和需插入的索引按序存储到T中 
		int i;
		for(i = 0; i<fanout-1; i++)
		{
			T->key[i] = P->key[i];
			T->pointer[i] = P->pointer[i];
		}
		T->pointer[i] = P->pointer[i];
		T->numOfKeys = fanout-1;
		T->InsertInNoLeaf(VV, NN->blockNum);
		//T的前一半复制到P中，后一半复制到PP中 
		int num = ceil( (float)fanout/2 );
		i=0;
		for( ; i<num-1; i++ )
		{
			P->pointer[i] = T->pointer[i];
			P->key[i] = T->key[i];
		}
		P->pointer[i] = T->pointer[i];
		P->numOfKeys = num-1;
		
		KeyType minKey = T->key[i];
		
		Node* PP = new Node(fanout, false, buffer);
		for( i++; i<fanout; i++ )
		{
			PP->pointer[i-num] = T->pointer[i];
			PP->key[i-num] = T->key[i];
		}
		PP->pointer[i-num] = T->pointer[i];
		PP->numOfKeys = fanout-num;
		
		PP->blockNum = GetEmptyBlock();
		P->WriteNodeToBlock(dbName, fileName, &head, P->blockNum);
		PP->WriteNodeToBlock(dbName, fileName, &head, PP->blockNum);			
		head.WriteHeadToBlock(dbName, fileName);
		
		InsertInParent(P, minKey, PP);
		
		delete PP;
		delete T;
	}
	delete P;
}

void BPlusTree::Insert(KeyType K, int P)
{
	
	if ( 0==head.root)
	{
		Node root(fanout, true, buffer);
		root.InsertInLeaf(K, P);
		head.root = GetEmptyBlock();
		head.firstLeafBlock = head.root; 
		head.numOfLeaves = 1;
		root.WriteNodeToBlock(dbName, fileName, &head, head.root);			
		head.WriteHeadToBlock(dbName, fileName);
	}
	else
	{
		Node* N;
		N = new Node(fanout, false, buffer);
		N->key[0].type = K.type;
		N->key[0].length = K.length;
		N->ReadNodeFromBlock(dbName, fileName, &head, head.root);
		SetRoot(N);

		Node* L = root;
		int blockNum;
		while ( false==L->leafNode )
		{
			blockNum = L->FindPointer(K);
			L->key[0].type = K.type;
			L->key[0].length = K.length; 
			L->ReadNodeFromBlock(dbName, fileName, &head, blockNum);
		}
		if ( L->numOfKeys<fanout-1 )
		{
			L->InsertInLeaf(K, P);
			L->WriteNodeToBlock(dbName, fileName, &head, L->blockNum);
		}
		else
		{	//把L中索引和需插入的索引按序存储到T中 
			Node* T = new Node(fanout+1, true, buffer);
			for(int i = 0; i<fanout-1; i++)
			{
				T->key[i] = L->key[i];
				T->index[i] = L->index[i];
			}
			T->numOfKeys = fanout-1;
			T->InsertInLeaf(K, P);
			
			Node* LL = new Node(fanout, true, buffer);
			//T的前一半复制到L中，后一半复制到LL中 
			int num = ceil( (float)fanout/2 );
			int i=0;
			for( ; i<num; i++ )
			{
				L->key[i] = T->key[i];
				L->index[i] = T->index[i];
			}
			L->numOfKeys = num;
			
			for( ; i<fanout; i++ )
			{
				LL->key[i-num] = T->key[i];
				LL->index[i-num] = T->index[i];
			}
			LL->numOfKeys = fanout-num;
			
			head.numOfLeaves++; 
			
			LL->blockNum = GetEmptyBlock();
			LL->next = L->next;
			L->next = LL->blockNum;
			L->WriteNodeToBlock(dbName, fileName, &head, L->blockNum);
			LL->WriteNodeToBlock(dbName, fileName, &head, LL->blockNum);			
			head.WriteHeadToBlock(dbName, fileName);
			
			KeyType minKey = LL->key[0];
			InsertInParent(L, minKey, LL);
			
			delete LL;
			delete T;
		}
		delete L;
	}
}


void BPlusTree::Delete(KeyType K, int P)
{
	Node* N;
	N = new Node(fanout, false, buffer);
	N->key[0].type = K.type;
	N->key[0].length = K.length;
	N->ReadNodeFromBlock(dbName, fileName, &head, head.root);
	SetRoot(N);
	
	Node* L = FindLeaf(K);
	int pos=L->FindIndex(K);
	if( pos!= P )
	{
		cout << P <<" doesn't exist.";
		return;
	}
	DeleteEntry(L, K);
}


bool BPlusTree::IfCanMerge(int num1, int num2, KeyType K)
{
	Node S(fanout, false, buffer);
	Node N(fanout, false, buffer);
	S.key[0].type = K.type;
	S.key[0].length = K.length;
	S.ReadNodeFromBlock(dbName, fileName, &head, num1);
	N.key[0].type = K.type;
	N.key[0].length = K.length;
	N.ReadNodeFromBlock(dbName, fileName, &head, num2);
	if( S.leafNode )
	{
		if( S.numOfKeys+N.numOfKeys<=fanout-1 )
			return true;
	}
	else
	{
		if( S.numOfKeys+N.numOfKeys<=fanout-2 )
			return true;
	}
	return false;
}


void BPlusTree::MergeLeaf(Node* S, Node* N)
{
	for(int i=0; i<N->numOfKeys; i++)
	{
		S->InsertInLeaf(N->key[i], N->index[i]);
	}
}


void BPlusTree::MergeNoLeaf(Node* S, KeyType PK, Node* N)
{
	S->InsertInNoLeaf(PK, N->pointer[0]);
	for(int i=0; i<N->numOfKeys; i++)
	{
		S->InsertInNoLeaf(N->key[i], N->pointer[i+1]);
	}
}


void BPlusTree::DeleteEntry(Node* N, KeyType K)
{
	N->Delete(K);
	N->WriteNodeToBlock(dbName, fileName, &head, N->blockNum);

	if( head.root==N->blockNum && 0==N->numOfKeys )
	{
		head.root = N->pointer[0];
		N->ClearBlock(dbName, fileName, &head, N->blockNum);
		head.WriteHeadToBlock(dbName, fileName);
		delete N;
	}
	else if( N->numOfKeys < ceil( (float)fanout/2 ) - 1 )
	{
		int blockNum = FindParent(N->blockNum, K);
		Node* P;
		P = new Node(fanout, false, buffer);
		P->key[0].type = K.type;
		P->key[0].length = K.length;
		P->ReadNodeFromBlock(dbName, fileName, &head, blockNum);
		
		int i;
		for(i=0; i<P->numOfKeys; i++)
		{
			if( P->pointer[i]==N->blockNum )
				break;
		}
		
		int pointerOfSibling;
		KeyType PK;
		if( (i!=0) && IfCanMerge(P->pointer[i], P->pointer[i-1], P->key[i-1]) )
		{
			pointerOfSibling = i-1;
			PK = P->key[i-1];
		}
		else
		{
			pointerOfSibling = i+1;
			PK = P->key[i];
		}
		
		Node* S;
		S = new Node(fanout, false, buffer);
		S->key[0].type = K.type;
		S->key[0].length = K.length;
		S->ReadNodeFromBlock(dbName, fileName, &head, P->pointer[pointerOfSibling]);
		
		if( IfCanMerge(S->blockNum, N->blockNum, PK) )
		{
			if( pointerOfSibling > i )
			{
				Node *T = N;
				N = S;
				S = T;
			}
			if( N->leafNode )
			{
				MergeLeaf(S, N);
				S->next = N->next;
				head.numOfLeaves--;
			}
			else
			{
				MergeNoLeaf(S, PK, N);
			}
			DeleteEntry(P, PK);
			
			N->ClearBlock(dbName, fileName, &head, N->blockNum);
			S->WriteNodeToBlock(dbName, fileName, &head, S->blockNum);			
			head.WriteHeadToBlock(dbName, fileName);
			delete N;
			delete S;
		}
		else
		{
			if( pointerOfSibling<i )
			{
				if( false==N->leafNode )
				{
					for(int j=N->numOfKeys; j>0; j--)
					{
						N->key[j] = N->key[j-1];
						N->pointer[j+1] = N->pointer[j];
					}
					N->pointer[1] = N->pointer[0];
					N->pointer[0] = S->pointer[S->numOfKeys];
					N->key[0] = PK;
					N->numOfKeys++;
					
					P->key[i-1] = S->key[S->numOfKeys-1];
					S->Delete(S->key[S->numOfKeys-1]);
					
					P->WriteNodeToBlock(dbName, fileName, &head, P->blockNum);
					N->WriteNodeToBlock(dbName, fileName, &head, N->blockNum);
					S->WriteNodeToBlock(dbName, fileName, &head, S->blockNum);			
				}
				else
				{
					N->InsertInLeaf(S->key[S->numOfKeys-1], S->index[S->numOfKeys-1]);
					P->key[i-1] = S->key[S->numOfKeys-1];
					S->Delete(S->key[S->numOfKeys-1]);
					
					P->WriteNodeToBlock(dbName, fileName, &head, P->blockNum);
					N->WriteNodeToBlock(dbName, fileName, &head, N->blockNum);
					S->WriteNodeToBlock(dbName, fileName, &head, S->blockNum);	
				}
			}
			else
			{
				if( false==N->leafNode )
				{
					N->InsertInNoLeaf(PK, S->pointer[0]);
					P->key[i] = S->key[0];
					//S->Delete(S->key[0]);
					int j;
					for(j=0; j<S->numOfKeys-1; j++)
					{
						S->key[j] = S->key[j+1];
						S->pointer[j] = S->pointer[j+1];
					}
					S->pointer[j] = S->pointer[j+1];
					
					P->WriteNodeToBlock(dbName, fileName, &head, P->blockNum);
					N->WriteNodeToBlock(dbName, fileName, &head, N->blockNum);
					S->WriteNodeToBlock(dbName, fileName, &head, S->blockNum);	
				}
				else
				{
					N->InsertInLeaf(S->key[0], S->index[0]);
					P->key[i] = S->key[1];
					S->Delete(S->key[0]);
					
					P->WriteNodeToBlock(dbName, fileName, &head, P->blockNum);
					N->WriteNodeToBlock(dbName, fileName, &head, N->blockNum);
					S->WriteNodeToBlock(dbName, fileName, &head, S->blockNum);	
				}
			}
			delete P;
			delete S;
			delete N;
		}
	}
	else
	{
		delete N;
	}
}



