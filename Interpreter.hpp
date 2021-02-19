//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: Interpreter                          ///
///       Produced by: SZY                             ///
///       Description: Read user's input and change it ///
///                    to another format that can be   ///
///                    recognized by Module API        ///
///       date: 2015/10/20                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////
#if !defined(_INTERPRETER_H_)
#define _INTERPRETER_H_

#include <iostream>
#include <math.h>
#include <string.h>
#include <string>
using namespace std;

//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
string Interpreter(string statement);
//��ȡ�û�����
string read_input();
//��֤create����Ƿ���Ч
string create_clause(string SQL,int start);
//��֤create database����Ƿ���Ч
string create_database(string SQL,int start);
//��֤create table����Ƿ���Ч
string create_table(string SQL,int start);
//�������
string get_attribute(string temp,string sql);
//��֤create index����Ƿ���Ч
string create_index(string SQL,int start);
//��֤create index on����Ƿ���Ч
string create_index_on(string SQL,int start,string sql);
//��֤drop����Ƿ���Ч
string drop_clause(string SQL,int start);
//��֤drop database����Ƿ���Ч
string drop_database(string SQL,int start);
//��֤drop table����Ƿ���Ч
string drop_table(string SQl,int start);
//��֤drop index����Ƿ���Ч
string drop_index(string SQL,int start);
//��֤select ����Ƿ���Ч
string select_clause(string SQL,int start);
//��֤insert ����Ƿ���Ч
string insert_clause(string SQL,int start);
//��֤delete����Ƿ���Ч
string delete_clause(string SQL,int start);
//�����ʽת��Ϊ�ڲ���ʽ
string get_expression(string temp,string sql);
//��֤use����Ƿ���Ч
string use_clause(string SQL,int start);
//��֤execfile����Ƿ���Ч
string execfile_clause(string SQL,int start);
//��֤quit����Ƿ���Ч
string quit_clause(string SQL,int start);

#endif
