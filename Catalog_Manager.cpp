#include "Catalog_Manager.h"

Catalog_Manager::Catalog_Manager(){
	DB_Used = "";
	Table_Count = 0;
	Table_Pos = NULL;
}
void Catalog_Manager::create_database(string DB_Name) {
	string path=".//"+DB_Name;
	if(_access(path.c_str(),0)==-1) {
		_mkdir(DB_Name.c_str());
	}
	else {
		throw 2;
	}
	path = "./"+DB_Name+"/"+DB_Name+".catlog";
	FILE *fp;
	fp = fopen(path.c_str(), "wb+");
	WORD table_count = 0;
	DWORD table_pos = 0;
	fwrite(&table_pos, sizeof(table_pos), 1, fp);
	fwrite(&table_count, sizeof(table_count), 1, fp);
	fclose(fp);
}
void Catalog_Manager::create_table(TABLEINFO &table, ATTRIBUTEINFO attr[32]) {
	if_database_used();
	if(if_table_exists(table.tableName)){
		throw 4;
	}
	string path = "./"+DB_Used+"/"+table.tableName+"_table.data";

	FILE *fp;
	int i;
	char attr_signal;
	DWORD *Table_Pos_New;
	const char *tableName = table.tableName.c_str();

//	fp = fopen(path.c_str(), "wb+");
//	fclose(fp);
	path = "./"+DB_Used+"/"+DB_Used+".catlog";
	fp = fopen(path.c_str(), "rb+");
	fseek(fp, Table_Pos[Table_Count], SEEK_SET);
	
	fwrite(tableName, __table_name_size, 1, fp);
	fwrite(&table.attribute_count, __attr_count_size, 1, fp);
	fwrite(&table.record_size, __record_length_size, 1, fp);
	fwrite(&table.record_count, __record_count_size, 1, fp);
	for(i=0; i<table.attribute_count; i++){
		fwrite(attr[i].attributeName.c_str(), __attr_name_size, 1, fp);
		fwrite(&attr[i].type, __attr_type_size, 1, fp);
		fwrite(&attr[i].type_count, __attr_char_size, 1, fp);
		attr_signal=0;
		attr_signal+=attr[i].has_index;
		attr_signal=attr_signal<<1;
		attr_signal+=attr[i].unique;
		attr_signal=attr_signal<<1;
		attr_signal+=attr[i].is_primary;
		fwrite(&attr_signal, __attr_signal_size, 1, fp);
		fwrite(attr[i].indexName.c_str(), __attr_index_name, 1, fp);
	}
	Table_Count++;
	Table_Pos_New = new DWORD[Table_Count+1];
	memcpy(Table_Pos_New, Table_Pos, (Table_Count)*sizeof(*Table_Pos));
	Table_Pos_New[Table_Count] = Table_Pos[Table_Count-1]
								+ __table_name_size 
								+ __attr_count_size 
								+ __record_length_size 
								+ __record_count_size 
								+ table.attribute_count*
								  ( __attr_name_size 
								  + __attr_type_size
								  + __attr_char_size
								  + __attr_signal_size
								  + __attr_index_name);
	fwrite(Table_Pos_New, sizeof(*Table_Pos_New), Table_Count+1, fp);
	fwrite(&Table_Count, sizeof(Table_Count), 1, fp);
	delete Table_Pos;
	Table_Pos = Table_Pos_New;
	fclose(fp);	
}

void Catalog_Manager::use_database(string DB_Name){
	close_database();
	string path=".//"+DB_Name;
	if(_access(path.c_str(),0)==-1) {
		throw 1;
	}
	FILE *fp;
	path = "./"+DB_Name+"/"+DB_Name+".catlog";

	DB_Used = DB_Name;

	fp = fopen(path.c_str(), "rb+"); 
	fseek(fp, -sizeof(Table_Count), SEEK_END);
	fread(&Table_Count, sizeof(Table_Count), 1, fp);
	fseek(fp, -(sizeof(Table_Count)+(Table_Count+1)*sizeof(*Table_Pos)), SEEK_END);
	Table_Pos = new DWORD[Table_Count+1];
	fread(Table_Pos, sizeof(*Table_Pos), Table_Count+1, fp);
	fclose(fp);
}

void Catalog_Manager::close_database() {
	if(DB_Used != ""){
		DB_Used = "";
		Table_Count = 0;
		delete[] Table_Pos;
		Table_Pos = NULL;
	}
}

void Catalog_Manager::drop_database(string DB_Name) {
	string path=".//"+DB_Name;
	if(DB_Used == DB_Name){
		close_database();
	}
	else if(_access(path.c_str(),0)==-1) {
		throw 1;
	}
	path="rd /s/q "+DB_Name;
	system(path.c_str());
}

void Catalog_Manager::drop_table(string Table_Name){
	if_database_used();
	FILE *fp;
	char attrName[__attr_name_size];
	string path;
	if(if_table_exists(Table_Name)){
		path = ".//"+DB_Used+"//"+Table_Name+"_table.data";
		remove(path.c_str());
		int i;
		char tableName[__table_name_size];
		path = "./"+DB_Used+"/"+DB_Used+".catlog";
		fp = fopen(path.c_str(), "rb+");
		int pointer;
		for(i=0; i<Table_Count; i++){
			fseek(fp, Table_Pos[i], SEEK_SET);
			fread(tableName, __table_name_size, 1, fp);
			string s(tableName);
			if(s==Table_Name){
				fseek(fp, Table_Pos[i], SEEK_SET);
				fwrite("", 1 , 1, fp);
				pointer = Table_Pos[i]+__table_name_size+__attr_count_size+__record_length_size+__record_count_size;
				while(pointer < Table_Pos[i+1]){
					fseek(fp, pointer, SEEK_SET);
					fread(attrName, __attr_name_size, 1, fp);
					string aName(attrName);
					path = ".//"+DB_Used+"//"+Table_Name+"_"+aName+"_index.data";
					remove(path.c_str());
					pointer+=__attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size+__attr_index_name;
				}
			}
		}
		fclose(fp);
	}
	else{
		throw 3;
	}
}

void Catalog_Manager::if_database_used(){
	if( DB_Used == "")
		throw 5;
}

bool Catalog_Manager::if_table_exists(string Table_Name){ 
	string path = "./"+DB_Used+"/"+Table_Name+"_table.data";
	FILE *fp;
	if(fp=fopen(path.c_str(), "rb+")) {
		fclose(fp);
		return 1;
	}
	else 
		return 0;
}

void Catalog_Manager::create_index(string Table_Name, string attrName, string indexName){
	if_database_used();
	string Index_Name = Table_Name + "_" + attrName +"_index";
	if(!if_table_exists(Table_Name))
		throw 3;
	else if(if_table_exists(Index_Name))
		throw 7;
	else if(if_index_exists(indexName,&attrName,&indexName))
		throw 7;
	else{
		int pointer = if_attr_exists(Table_Name, attrName);
		FILE *fp;
		BYTE signal;
		string path = "./"+DB_Used+"/"+DB_Used+".catlog";
		fp = fopen(path.c_str(), "rb+");
		pointer += __attr_name_size+__attr_type_size+__attr_char_size;
		fseek(fp, pointer, SEEK_SET);
		fread(&signal, sizeof(signal), 1, fp);
		if (!(signal & 0x2)){
			fclose(fp);
			throw 10;
		}
		signal = signal|0x4;
		fseek(fp, pointer, SEEK_SET);
		fwrite(&signal, sizeof(signal), 1, fp);
		fwrite(indexName.c_str(), __attr_index_name, 1, fp);
		
		path = "./"+DB_Used+"/"+Table_Name+"_"+attrName+"_index.data";
		fclose(fp);
//		fp = fopen(path.c_str(),"wb+");
//		fclose(fp);
	}
}

bool Catalog_Manager::if_index_exists(string Index_Name, string *Table_Name, string *Attr_Name) {
	if_database_used();
	string path;
	int i;
	char tableName[__table_name_size];
	char attrName[__attr_name_size];
	char indexName[__attr_index_name];
	path = "./"+DB_Used+"/"+DB_Used+".catlog";
	FILE *fp = fopen(path.c_str(), "rb+");
	int pointer;
	for(i=0; i<Table_Count; i++){
		pointer = Table_Pos[i]+__table_name_size+__attr_count_size+__record_length_size+__record_count_size
				+__attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size;
		while(pointer < Table_Pos[i+1]){
			fseek(fp, pointer, SEEK_SET);
			fread(indexName, __attr_index_name, 1, fp);
			string iName(indexName);
			if(iName == Index_Name){
				pointer = pointer-__attr_signal_size-__attr_char_size-__attr_type_size-__attr_name_size;
				fseek(fp, pointer, SEEK_SET);
				fread(attrName, __attr_name_size, 1, fp);
				string aName(attrName);
				*Attr_Name = aName;
				pointer = Table_Pos[i];
				fseek(fp, pointer, SEEK_SET);
				fread(tableName, __table_name_size, 1, fp);
				string sTableName(tableName);
				*Table_Name = sTableName;
				fclose(fp);
				return true;
			}
			pointer+=__attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size+__attr_index_name;
		}
	}
	fclose(fp);
	return false;
}

void Catalog_Manager::drop_index(string Index_Name) {
	if_database_used();
	string path;
	int i;
	char attrName[__attr_name_size];
	char indexName[__attr_index_name];
	char tableName[__table_name_size];
	path = "./"+DB_Used+"/"+DB_Used+".catlog";
	FILE *fp = fopen(path.c_str(), "rb+");
	int pointer;
	string empty = "";
	for(i=0; i<Table_Count; i++){
		fseek(fp, Table_Pos[i], SEEK_SET);
		fread(tableName, __table_name_size, 1, fp);
		pointer = Table_Pos[i]+__table_name_size+__attr_count_size+__record_length_size+__record_count_size
				+__attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size;
		while(pointer < Table_Pos[i+1]){
			fseek(fp, pointer, SEEK_SET);
			fread(indexName, __attr_index_name, 1, fp);
			string iName(indexName);
			if(iName == Index_Name){
				pointer = pointer-__attr_signal_size-__attr_char_size-__attr_type_size-__attr_name_size;
				fseek(fp, pointer, SEEK_SET);
				fread(attrName, __attr_name_size, 1, fp);
				string aName(attrName);
				string Table_Name(tableName);
				path = ".//"+DB_Used+"//"+Table_Name+"_"+aName+"_index.data";
				remove(path.c_str());
				BYTE signal;
				pointer = pointer+__attr_name_size+__attr_type_size+__attr_char_size;
				fseek(fp, pointer, SEEK_SET);
				fread(&signal, sizeof(signal), 1, fp);
				signal = signal&0xfb;
				fseek(fp, pointer, SEEK_SET);
				fwrite(&signal, sizeof(signal), 1, fp);
				fwrite(empty.c_str(), __attr_index_name, 1, fp);
				fclose(fp);
				return;
			}
			pointer+=__attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size+__attr_index_name;
		}
	}
	fclose(fp);
	throw 8;
}


int Catalog_Manager::if_attr_exists(string Table_Name, string Attr_Name){ //返回该属性头部在catalog文件中的位置
	FILE *fp;
	int i, pointer;
	char tableName[__table_name_size];
	char attrName[__attr_name_size];
	string path = "./"+DB_Used+"/"+DB_Used+".catlog";
	fp = fopen(path.c_str(), "rb+"); 
	for(i=0; i<Table_Count; i++){
		fseek(fp, Table_Pos[i], SEEK_SET);
		fread(tableName, __table_name_size, 1, fp);
		string s(tableName);
		if(s==Table_Name){
			pointer = Table_Pos[i]+__table_name_size+__attr_count_size+__record_length_size+__record_count_size;
			while(pointer<Table_Pos[i+1]){
				fseek(fp, pointer, SEEK_SET);
				fread(attrName, __attr_name_size, 1, fp);
				string s_attrName(attrName);
				if(s_attrName == Attr_Name){
					fclose(fp);
					return pointer;
				}
				pointer += __attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size+__attr_index_name;
			}
			fclose(fp);
			throw 6;
		}
	}
	fclose(fp);
	return 0;
}

void Catalog_Manager::get_table_info(string Table_Name, ATTRIBUTEINFO attr[32], int attr_count, WORD *record_size, DWORD *record_count){
	if_database_used();
	if(!if_table_exists(Table_Name))
		throw 3;
	int i,j;
	char tableName[__table_name_size];
	string path = "./"+DB_Used+"/"+DB_Used+".catlog";
	FILE *fp = fopen(path.c_str(), "rb+"); 
	int pointer;
	char attrName[__attr_name_size];
	char indexName[__attr_index_name];
	BYTE signal;

	for(i=0; i<Table_Count; i++){
		fseek(fp, Table_Pos[i], SEEK_SET);
		fread(tableName, __table_name_size, 1, fp);
		string s(tableName);
		if(s==Table_Name){
			pointer = Table_Pos[i]+__table_name_size+__attr_count_size;
			fseek(fp, pointer, SEEK_SET);
			fread(record_size, sizeof(*record_size), 1, fp);
			fread(record_count, sizeof(*record_count), 1, fp);
			for(j=0; j<attr_count; j++){
				pointer = Table_Pos[i]+__table_name_size+__attr_count_size+__record_length_size+__record_count_size;
				while(pointer<Table_Pos[i+1]){
					fseek(fp, pointer, SEEK_SET);
					fread(attrName, __attr_name_size, 1, fp);
					string s_attrName(attrName);
					if(s_attrName == attr[j].attributeName){
						fread(&attr[j].type, __attr_type_size, 1, fp);
						fread(&attr[j].type_count, __attr_char_size, 1, fp);
						fread(&signal, __attr_signal_size, 1, fp);
						fread(indexName, __attr_index_name, 1, fp);
						string iName(indexName);
						attr[j].indexName = iName;
						attr[j].has_index=(signal&0x4)?1:0;
						attr[j].unique=(signal&0x2)?1:0;
						attr[j].is_primary=(signal&0x1)?1:0;
						break;
					}
					pointer += __attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size+__attr_index_name;
				}
				if(pointer>=Table_Pos[i+1]){
					fclose(fp);
					throw 9;
				}
			}
			fclose(fp);
			return;
		}
	}
}

void Catalog_Manager::get_table_attr(string Table_Name, ATTRIBUTEINFO attr[32], char *attr_count) {
	if_database_used();
	if(!if_table_exists(Table_Name))
		throw 3;
	int i,j;
	char tableName[__table_name_size];
	string path = "./"+DB_Used+"/"+DB_Used+".catlog";
	FILE *fp = fopen(path.c_str(), "rb+"); 
	int pointer;
	char attrName[__attr_name_size];
	BYTE signal;

	for(i=0; i<Table_Count; i++){
		fseek(fp, Table_Pos[i], SEEK_SET);
		fread(tableName, __table_name_size, 1, fp);
		string s(tableName);
		if(s==Table_Name){
			pointer = Table_Pos[i]+__table_name_size;
			fseek(fp, pointer, SEEK_SET);
			fread(attr_count, sizeof(*attr_count), 1, fp);
			pointer = Table_Pos[i]+__table_name_size+__attr_count_size+__record_length_size+__record_count_size;
			for(j=0; j<*attr_count; j++){
				fseek(fp, pointer, SEEK_SET);
				fread(attrName, __attr_name_size, 1, fp);
				string s_attrName(attrName);
				attr[j].attributeName = s_attrName;
				fread(&attr[j].type, __attr_type_size, 1, fp);
				fread(&attr[j].type_count, __attr_char_size, 1, fp);
				fread(&signal, __attr_signal_size, 1, fp);
				attr[j].has_index=(signal&0x4)?1:0;
				attr[j].unique=(signal&0x2)?1:0;
				attr[j].is_primary=(signal&0x1)?1:0;
				pointer += __attr_name_size+__attr_type_size+__attr_char_size+__attr_signal_size+__attr_index_name;
			}
			fclose(fp);
			return;
		}
	}
}
