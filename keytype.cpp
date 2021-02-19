#include "keytype.h"

bool operator==(const KeyType& k1, const KeyType& k2)
{
	switch(k1.type)
	{
		case 'i': return k1.intKey==k2.intKey;
		case 'f': return k1.floatKey==k2.floatKey;
		case 'c': return k1.stringKey==k2.stringKey;
	}
}

bool operator>(const KeyType& k1, const KeyType& k2)
{
	switch(k1.type)
	{
		case 'i': return k1.intKey>k2.intKey;
		case 'f': return k1.floatKey>k2.floatKey;
		case 'c': return k1.stringKey>k2.stringKey;
	}
}

bool operator!=(const KeyType& k1, const KeyType& k2)
{
	return !(k1==k2);
}

bool operator<(const KeyType& k1, const KeyType& k2)
{
	return k2>k1;
}

bool operator>=(const KeyType& k1, const KeyType& k2)
{
	return !(k2>k1);
}

bool operator<=(const KeyType& k1, const KeyType& k2)
{
	return !(k1>k2);
}
