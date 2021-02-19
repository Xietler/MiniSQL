#include "recordhead.h"

void RecordHead::ReadHead(string dbName, string fileName)
{
	
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, 0);
	
	bip->byteNum=4096;
	int bp=0;
	
	char* p1 = (char*)(bip->cBlock);
	if( *p1=='H' )
	{//��RecorderHead�� 
		int *p2 = (int*)(bip->cBlock+1);
		firstEmptyNum = *p2;
		p2 = (int*)(bip->cBlock+5);
		NumOfRecords = *p2;
		p2 = (int*)(bip->cBlock+9);
		totalNum = *p2;
		p2 = (int*)(bip->cBlock+13);
		recordLength = *p2;
		p2 = (int*)(bip->cBlock+17);
		maxNumOfEachBlock = *p2;
		p2 = (int*)(bip->cBlock+21);
		NumOfBlocks = *p2;
		return;
	}
	else
	{//���� 
		cout<<"Read record head error";
	}
}

void RecordHead::WriteHead(string dbName, string fileName)
{
	BLOCKINFO* bip;
	bip = buffer->get_file_block(dbName, fileName, 0);
	
	bip->byteNum=4096;
	int bp=0;
	
	char* p1 = (char*)(bip->cBlock);
	*p1 = 'H';
	bp += sizeof(char);
	bip->dirtyBit = 1;
	
	//д���һ���ռ�¼��
	int* p2 = (int*)(bip->cBlock+bp);
	*p2 = firstEmptyNum;
	bp += sizeof(int);
	
	//д��ǿռ�¼��
	p2 = (int*)(bip->cBlock+bp);
	*p2 = NumOfRecords;
	bp += sizeof(int);
	
	//д��ʹ�ù��ļ�¼������+�ǿգ� 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = totalNum;
	bp += sizeof(int);
	
	//д���¼����
	p2 = (int*)(bip->cBlock+bp);
	*p2 = recordLength;
	bp += sizeof(int);
	
	//д��һ����������¼�� 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = maxNumOfEachBlock;
	bp += sizeof(int);
	
	//д����� 
	p2 = (int*)(bip->cBlock+bp);
	*p2 = NumOfBlocks;
	bp += sizeof(int);

}

/*
int main()
{
	RecorderHead head;
	
	head.firstEmptyNum = 0;
	head.NumOfRecords = 0;
	head.totalNum = 0;
	head.recordLength = 100;
	head.maxNumOfEachBlock = 40;
	head.NumOfBlocks = 1;
	
	head.WriteHead("lzy","test");
	head.ReadHead("lzy","test"); 
	cout<<head.firstEmptyNum<<head.NumOfRecords<<head.totalNum<<head.recordLength<<head.maxNumOfEachBlock<<head.NumOfBlocks;
}
*/
