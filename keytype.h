#ifndef KEYTYPE_H
#define KEYTYPE_H

#include <iostream>
#include <string>
using namespace std;
using std::string;

struct KeyType
{
	char type; 			//key���ͣ�'i'����int,'f'����flaot,'c'����string 
	int  intKey;		//int���͵�key��ֵ 
	float floatKey;		//float���͵�key��ֵ 
	string stringKey;	//string���͵�key��ֵ 
	int length;			//key����
	
	friend bool operator==(const KeyType& k1, const KeyType& k2);
	friend bool operator>(const KeyType& k1, const KeyType& k2);
	friend bool operator!=(const KeyType& k1, const KeyType& k2);
	friend bool operator<(const KeyType& k1, const KeyType& k2);
	friend bool operator>=(const KeyType& k1, const KeyType& k2);
	friend bool operator<=(const KeyType& k1, const KeyType& k2);
};

#endif
