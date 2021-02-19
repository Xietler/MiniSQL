#ifndef _API_H_
#define _API_H_

#include "Catalog_Manager.h"
#include "recordmanager.h"
#include "Interpreter.hpp"
#include "indexmanager.h"
#include "show.h"
#include <algorithm>

#define __int_float_type 1
#define __float_type 2
#define __string_type 3
#define __null_type 4
#define __error_type 5

#define __equal 6
#define __above 5
#define __below 4
#define __above_equal 3
#define __below_equal 2
#define __not_equal 1

#define __create_table 01
#define __create_database 00
#define __create_index 02
#define __drop_table 11
#define __drop_database 10
#define __drop_index 12
#define __insert 40
#define __delete_no_where 31
#define __delete_with_where 30
#define __select_no_where 21
#define __select_with_where 20
#define __use 50
#define __execfile 90
#define __quit 80
#define __error 99
#define __help 70

typedef struct SQLlanguage{
    string opId; //操作编号
    string attrSelect[32];
    int attrSelectNum;
    string opObj; //操作对象：database or table or file
    bool isAnd[64];
    string attrWhere[64]; //筛选的属性名
    int op[64];//筛选的操作符
    string val[64];//筛选的值 或 插入的值
    int attrWhereNum;
    string attrType[64];//"+":int,"-":float,"n":char(n);
    bool uniqueFlag[64];//true:unique;
    string primaryKey;
} SQLlang;

typedef struct attrRecord{
	char type;		//属性类型 
	char charNum;	//属性（最大）长度 
	string name;	//属性名 
	bool has_index;	//是否有索引 
	char andOr; 	//只在where的attr中有用，判断是and还是or
	int op; 		//只在where的attr中有用,判断操作符
	string val; 	//只在where的attr中有用
	int point;		//该属性一条记录中的偏移量 
}ATTRRECORD;


class API {
private:
	Catalog_Manager catalog;
	Buffer_Manager buffer;
	RecordManager record;
	IndexManager index;
	string strInstr;
	SQLlang instr;

public:
	API();
	void run(); 
	int judgeType(string val);
	string toString(string val);
	int toInt(string val);
	float toFloat(string val);
	
	void createDatabase();
	void createTable();
	void createIndex();
	
	void dropTable();
	void dropDatabase();
	void dropIndex();
	
	void insert();
	void deleteNoWhere();
	void deleteWithWhere();
	void selectNoWhere();
	void selectWithWhere();

	void judgeAttrExit(string Attr_Name[32], int attrCount, ATTRIBUTEINFO *attr, int totalCount);

	void useDatabase();
	void execFile();
	void mergeIndex(int *p1, int *p2, int *out, int count1, int count2, int &outCount, char mode);

	bool judgeInt(int a, int b, int op);
	bool judgeFloat(float a, float b, int op);
	bool judgeString(string a, string b, int op);
	
	SQLlang SQL_Modify(string SQL);
};

#endif

