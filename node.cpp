#include "node.h"

Node::Node(int fanout, bool leafNode, Buffer_Manager* buffer)
{
	this->fanout = fanout;
	this->leafNode = leafNode;
	numOfKeys = 0;
	key = new KeyType[fanout-1];
	index = new int[fanout-1];
	next = -1;
	pointer = new int[fanout];
	this->buffer = buffer;
}

Node::~Node()
{
	delete[]key;
	delete[]index;
	delete[]pointer;
}

int Node::FindPointer(KeyType V)
{
	int i = 0;
	
	while (i<numOfKeys && V>key[i])
		i++;
		
	if( i==numOfKeys )
		return pointer[i];
	else if ( V==key[i] )
		return pointer[i+1];
	else
		return pointer[i];
	
}


int Node::FindIndex(KeyType V)
{
	int i = 0;
	
	while (i<numOfKeys && V>key[i])
		i++;

	if (i<numOfKeys  && V == key[i])
		return index[i];
	else
		return -1;
	
}


int Node::FindPosition(KeyType V)
{
	if( 0==numOfKeys )
		return 0;
	int i;
	for(i=0; i<numOfKeys; i++)
	{
		if( key[i]>=V )
			return i;
	}
	return i;
}


void Node::InsertInLeaf(KeyType V, int P)
{
	if( numOfKeys==fanout-1 )
	{
		cout << "Node is full! Insert Error!";
		return;
	}
	
	int pos = FindPosition(V);
	for(int i=numOfKeys; i>pos; i--)
	{
		key[i] = key[i-1];
		index[i] = index[i-1];
	}
	key[pos] = V;
	index[pos] = P;
	numOfKeys++;
}


void Node::InsertInNoLeaf(KeyType V, int N)
{
	if( numOfKeys==fanout-1 )
	{
		cout << "Node is full! Insert Error!";
		return;
	}
	
	int pos = FindPosition(V);
	for(int i=numOfKeys; i>pos; i--)
	{
		key[i] = key[i-1];
		pointer[i+1] = pointer[i];
	}
	key[pos] = V;
	pointer[pos+1] = N;
	numOfKeys++;
}

void Node::Delete(KeyType K)
{
	int i;
	for(i=0; i<numOfKeys; i++)
	{
		if( key[i]== K )
			break;
	}
	if( leafNode )
	{
		for( ; i<numOfKeys-1; i++)
		{
			key[i] = key[i+1];
			index[i] = index[i+1];
		}		
	}
	else
	{
		for( ; i<numOfKeys-1; i++)
		{
			key[i] = key[i+1];
			pointer[i+1] = pointer[i+2];
		}
	}
	numOfKeys--;
}


void Node::WriteNodeToBlock(string dbName, string fileName, IndexHead* head, int blockNum)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName ,fileName, blockNum);
	
	bip->byteNum = 4096;
	int bp=0;
	
	char* p1 = (char*)(bip->cBlock);

	//是否是叶节点，叶节点写入T非叶节点写入F
	if( leafNode )
		*p1 = 'T';
	else
		*p1 = 'F';
	bip->dirtyBit = 1;
	bp += sizeof(char);
	
	//写入节点中key的数量 
	int* p2 = (int*)(bip->cBlock+bp);
	*p2 = numOfKeys;
	bp += sizeof(int);

	
	for(int i=0; i<numOfKeys; i++)
	{//依次写入pointer位置和key值 
		int* p3 = (int*)(bip->cBlock+bp);
		if( leafNode )
			*p3 = index[i];
		else
			*p3 = pointer[i];
		bp += sizeof(int);
		
		switch( key[i].type )
		{//根据key的类型写入key 
			case 'i':
			{//key是int 
				int* p4 = (int*)(bip->cBlock+bp);
				*p4 = key[i].intKey;
				bp += sizeof(int);
				break;	
			}
			case 'f':
			{//key是float 
				float* p4 = (float*)(bip->cBlock+bp);
				*p4 = key[i].floatKey;
				bp += sizeof(float);
				break;	
			}
			case 'c':
			{//key是字符（串） 
				char* p4 = (char*)(bip->cBlock+bp);
				const char* cs=key[i].stringKey.c_str();
				strcpy(p4,cs);
				bp += (key[i].length+1)*sizeof(char);
				break;	
			}	
		}
	}
	if( leafNode )
	{//叶节点写入下一叶节点的块号 
		int* p5 = (int*)(bip->cBlock+bp);
		*p5 = next;
		bp += sizeof(int);
	}
	else
	{//非叶节点写入最后一个子节点的块号 
		int* p5 = (int*)(bip->cBlock+bp);
		*p5 = pointer[numOfKeys];
		bp += sizeof(int);
	}

}

void Node::ReadNodeFromBlock(string dbName, string fileName,IndexHead* head, int blockNum)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, blockNum);
	
	bip->byteNum=4096;
	int bp=0;
	
	this->blockNum = blockNum;
	//读入是否是叶节点，T为叶节点,F为非叶节点 
	char* p1 = (char*)(bip->cBlock);
	if( *p1=='T' )
		leafNode = true;
	else if( *p1=='F' )
		leafNode = false;
	else if( *p1=='N' )
	{
		
	}
	bp += sizeof(char);
	
	//读入节点中key的数量 
	int* p2 = (int*)(bip->cBlock+bp);
	numOfKeys = *p2;
	bp += sizeof(int);
	
	for(int i=0; i<numOfKeys; i++)
	{//依次读入pointer指和key值 
		int* p3 = (int*)(bip->cBlock+bp);
		if( leafNode )
			index[i] = *p3;
		else
			pointer[i] = *p3;
		bp += sizeof(int);
		
		key[i].type = key[0].type;
		key[i].length = key[0].length;
		switch( key[i].type )
		{//根据key的类型读入key 
			case 'i':
			{//key是int 
				int* p4 = (int*)(bip->cBlock+bp);
				key[i].intKey = *p4;
				bp += sizeof(int);
				break;	
			}
			case 'f':
			{//key是float 
				float* p4 = (float*)(bip->cBlock+bp);
				key[i].floatKey = *p4;
				bp += sizeof(float);
				break;	
			}
			case 'c':
			{//key是字符（串） 
				char* p4 = (char*)(bip->cBlock+bp);
				char s[256];
				int j;
				for(j=0; (*p4)!='\0'; p4++,j++)
				{
					s[j]=*p4;
				}
				s[j]=*p4;
				key[i].stringKey = s;
				bp += (key[i].length+1)*sizeof(char);
				break;	
			}	
		}
	}
	if( leafNode )
	{//叶节点读入下一叶节点的块号 
		int* p5 = (int*)(bip->cBlock+bp);
		next = *p5;
		bp += sizeof(int);
	}
	else
	{//非叶节点写入最后一个子节点的块号 
		int* p5 = (int*)(bip->cBlock+bp);
		pointer[numOfKeys] = *p5;
		bp += sizeof(int);
	}
	/*
	cout<<"blocknum:"<<blockNum<<" leafnode:"<<leafNode<<" numofkeys:"<<numOfKeys<<" fanout:"<<fanout<<"\n";
	int i=0;
	for(; i<numOfKeys; i++)
	{
		if(leafNode)
			cout<<" "<<index[i]<<" "<<key[i].stringKey;
		else
			cout<<" "<<pointer[i]<<" "<<key[i].stringKey;
	}
	if(leafNode)
		cout<<" "<<next;
	else
		cout<<" "<<pointer[i];
	cout<<"\n";
	*/
}

void Node::ClearBlock(string dbName, string fileName, IndexHead* head, int blockNum)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName ,fileName, blockNum);
	
	bip->byteNum = 4096;
	int bp=0;
	
	char* p1 = (char*)(bip->cBlock);
	*p1 = 'N';
	bip->dirtyBit = 1;
	bp += sizeof(char);
	
	int* p2 = (int*)(bip->cBlock+bp);
	*p2 = head->firstEmptyBlock;
	head->firstEmptyBlock = blockNum;
	bp += sizeof(int);
	
}


/*
int main()
{
	
//	Node N(3,true);	
//	N.blockNum=1;
//	N.numOfKeys=2;
//	N.key[0].type='c';
//	N.key[1].type='c';
//	N.key[0].stringKey="123123";
//	N.key[1].stringKey="abcdefghij";
//	N.key[0].length=10;
//	N.key[1].length=10;
//	if( N.leafNode )
//	{
//		N.index[0]=5;
//		N.index[1]=8;
//		N.next=-1;
//	}
//	else
//	{
//		N.pointer[0]=1;
//		N.pointer[1]=2;
//		N.pointer[2]=3;
//	}
	
	//N.WriteNodeToBlock("lzy", "test_address", &H, 1);
}
*/
