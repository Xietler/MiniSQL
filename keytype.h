#ifndef KEYTYPE_H
#define KEYTYPE_H

#include <iostream>
#include <string>
using namespace std;
using std::string;

struct KeyType
{
	char type; 			//key类型，'i'代表int,'f'代表flaot,'c'代表string 
	int  intKey;		//int类型的key的值 
	float floatKey;		//float类型的key的值 
	string stringKey;	//string类型的key的值 
	int length;			//key长度
	
	friend bool operator==(const KeyType& k1, const KeyType& k2);
	friend bool operator>(const KeyType& k1, const KeyType& k2);
	friend bool operator!=(const KeyType& k1, const KeyType& k2);
	friend bool operator<(const KeyType& k1, const KeyType& k2);
	friend bool operator>=(const KeyType& k1, const KeyType& k2);
	friend bool operator<=(const KeyType& k1, const KeyType& k2);
};

#endif
