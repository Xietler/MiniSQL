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
	BYTE type; // ��������ʲô���͵ģ�int? float? char?
	BYTE type_count; //���char���ͣ��м���?
	bool unique; //������ֵ�Ƿ�Ψһ
	string attributeName; // ��������
	bool has_index; //���������Ƿ�������
	bool is_primary; //�Ƿ�������
	string indexName; //������
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
	//��ʼ��private�е�ֵ
	Catalog_Manager();
	//�������ݿ⣬����Ѿ����ڣ��򱨴����򴴽��ļ����Լ�.catlog�ļ���ͬʱ��ʼ�����ļ�
	void create_database(string DB_Name);
	//���������û��ʹ�����ݿ⣬���߱��ļ��Ѿ������򱨴����򴴽����ļ����Ҹ���.catlog�ļ�
	void create_table(TABLEINFO &table, ATTRIBUTEINFO attr[32]);
	//����index�����û��ʹ�����ݿ⣬���߱��ļ������ڣ�����index�ļ��Ѵ����򱨴����򴴽�����ʼ��index�ļ�������.catlog�ļ�
	void create_index(string Table_Name, string attrName, string indexName);
	//ɾ�����ݿ⣬���Ҫɾ����database�������򱨴��������ʹ�õ�database��Ҫɾ������ͬ����close_database��ɾ�������ļ���
	void drop_database(string DB_Name);
	//ɾ�������û��ʹ�����ݿ⣬����ļ��������򱨴�����.catlog�ļ��и��ļ�����Ϊ���ַ���������ɾ���ñ��ļ�
	void drop_table(string Table_Name);
	//ɾ��index�����û��ʹ�����ݿ⣬���߱��ļ������ڣ�����index�ļ��������򱨴��������.catlog�ļ�������ɾ����.catlog�ļ�
	void drop_index(string Index_Name);
	//ʹ�����ݿ⣬������ݿⲻ�����򱨴�����ر����ݿ⣬��ȡ�µ�.catlog�ļ������¶���private��Ϣ
	void use_database(string DB_Name);
	//�ر����ݿ⣬��DB_Used��Ϊ�գ���ָ����ڴ���գ���Table_Count��Ϊ0
	void close_database();
	//�ж��Ƿ���ʹ�����ݿ⣬���DB_UsedΪ���򱨴�
	void if_database_used();
	//�жϱ��Ƿ���ڣ����ұ��ļ���������ڷ���1�����򷵻�0
	bool if_table_exists(string Table_Name);
	//�ж������Ƿ���ڣ��������Ѿ��ٶ����ݿ�ͱ��ļ���û���⣩����ȡ�������ڵ�λ�ã����ظ�λ��
	int if_attr_exists(string Table_Name, string attrName);
	//���ݴ�������Ե����ֵõ�������Ϣ�����û��ʹ�����ݿ⣬���߱������򱨴����򽫸��ļ�������������Ϣ���Ƶ�attr�����У��ı�record_size��record_count
	void get_table_info(string Table_Name, ATTRIBUTEINFO attr[32], int attr_count, WORD *record_size, DWORD *record_count);
	//�õ�����������Ե�ȫ����Ϣ
	void get_table_attr(string Table_Name, ATTRIBUTEINFO attr[32], char *attr_count);
	//����index�����ж�index�Ƿ����
	bool if_index_exists(string Index_Name, string *Table_Name, string *Attr_Name);
};

#endif
