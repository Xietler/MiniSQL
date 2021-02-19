#include "API.h"
#include <stdio.h>

int main() {
	API api;
	api.run();
	
	return 0;
}
/*
create database lzy;
use lzy;
create table t13(name char(5), id int unique, gpa float, primary key(name) );
create index i1 on t13(id);
insert into t13 values('aaa',10,1.1);
insert into t13 values('bbb',11,null);
insert into t13 values('ccc',12,2.5);
insert into t13 values('ddd',10,5);

select * from t13;
select * from t13 where id=10;

drop database db;

execfile ./test/test1.txt;
*/
