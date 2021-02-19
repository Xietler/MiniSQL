#ifndef _CATALOG_H_
#define _CATALOG_H_

#include <string>
#include <io.h>
#include <direct.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

#define BYTE unsigned char
#define WORD unsigned short int
#define DWORD unsigned long int
#define FLOAT_SIZE	sizeof(float)

#define __table_name_size 21
#define __attr_count_size 1
#define __record_length_size 2
#define __record_count_size 4
#define __attr_name_size 21
#define __attr_type_size 1
#define __attr_char_size 1
#define __attr_signal_size 1
#define __attr_index_name 21

#define __INT 'i'
#define __CHAR 'c'
#define __FLOAT 'f' 

typedef struct tableInfo TABLEINFO;
typedef struct attributeInfo ATTRIBUTEINFO;

struct attributeInfo {
	BYTE type; // 该属性是什么类型的，int? float? char?
	BYTE type_count; //针对char类型，有几个?
	bool unique; //该属性值是否唯一
	string attributeName; // 属性名字
	bool has_index; //该属性上是否有索引
	bool is_primary; //是否是主键
	string indexName; //索引名
};

struct tableInfo {
	int tableNum;
	string tableName;
	BYTE attribute_count;
	DWORD record_count;
	WORD record_size;
};

class Catalog_Manager {
private:
	WORD Table_Count;
	DWORD *Table_Pos;
public:
	string DB_Used;
	//初始化private中的值
	Catalog_Manager();
	//创建数据库，如果已经存在，则报错，否则创建文件夹以及.catlog文件，同时初始化该文件
	void create_database(string DB_Name);
	//创建表，如果没有使用数据库，或者表文件已经存在则报错，否则创建表文件并且更新.catlog文件
	void create_table(TABLEINFO &table, ATTRIBUTEINFO attr[32]);
	//创建index，如果没有使用数据库，或者表文件不存在，或者index文件已存在则报错，否则创建并初始化index文件，更新.catlog文件
	void create_index(string Table_Name, string attrName, string indexName);
	//删除数据库，如果要删除的database不存在则报错，如果正在使用的database与要删除的相同，则close_database，删除整个文件夹
	void drop_database(string DB_Name);
	//删除表，如果没有使用数据库，或表文件不存在则报错，否则将.catlog文件中该文件名改为空字符串，并且删除该表文件
	void drop_table(string Table_Name);
	//删除index，如果没有使用数据库，或者表文件不存在，或者index文件不存在则报错，否则更新.catlog文件，并且删除该.catlog文件
	void drop_index(string Index_Name);
	//使用数据库，如果数据库不存在则报错，否则关闭数据库，读取新的.catlog文件，更新对象private信息
	void use_database(string DB_Name);
	//关闭数据库，将DB_Used设为空，将指针的内存清空，将Table_Count设为0
	void close_database();
	//判断是否有使用数据库，如果DB_Used为空则报错
	void if_database_used();
	//判断表是否存在，查找表文件，如果存在返回1，否则返回0
	bool if_table_exists(string Table_Name);
	//判断属性是否存在，（这里已经假定数据库和表文件都没问题），读取属性所在的位置，返回该位置
	int if_attr_exists(string Table_Name, string attrName);
	//根据传入的属性的名字得到到表信息，如果没有使用数据库，或者表不存在则报错，否则将该文件的所有属性信息复制到attr数组中，改变record_size和record_count
	void get_table_info(string Table_Name, ATTRIBUTEINFO attr[32], int attr_count, WORD *record_size, DWORD *record_count);
	//得到表的所有属性的全部信息
	void get_table_attr(string Table_Name, ATTRIBUTEINFO attr[32], char *attr_count);
	//根据index名称判断index是否存在
	bool if_index_exists(string Index_Name, string *Table_Name, string *Attr_Name);
};

#endif
