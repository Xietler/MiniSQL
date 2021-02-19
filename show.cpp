#include "show.h"

void output(showOut out)
{
	int maxLength[36];
	//确定每列宽度
	for (int i = 0; i<out.attrNum; i++){
		maxLength[i] = out.attrName[i].length();
		for (int j = 0; j<out.recordLength; j++){
			if (out.val[i][j].length()>maxLength[i]) maxLength[i] = out.val[i][j].length();
		}
		maxLength[i] += 1;
	}

	//绘制列头
	for (int i = 0; i<out.attrNum; i++){
		cout << "+";
		for (int j = 0; j<maxLength[i] + 1; j++)
			cout << "-";
	}
	cout << "+" << endl;

	for (int i = 0; i<out.attrNum; i++){
		cout << "| ";
		cout << left << setw(maxLength[i]) << out.attrName[i];
	}
	cout << "|" << endl;

	for (int i = 0; i<out.attrNum; i++){
		cout << "+";
		for (int j = 0; j<maxLength[i] + 1; j++)
			cout << "-";
	}
	cout << "+" << endl;

	for (int i = 0; i<out.recordLength; i++){
		for (int j = 0; j<out.attrNum; j++){
			cout << "| ";
			cout << left << setw(maxLength[j]) << out.val[j][i];
		}
		cout << "|" << endl;
	}

	for (int i = 0; i<out.attrNum; i++){
		cout << "+";
		for (int j = 0; j<maxLength[i] + 1; j++)
			cout << "-";
	}
	cout << "+" << endl;

}

void print_help()
{
	cout << "----------------------------------------------------------------" << endl;
	cout << "------------------------------Help------------------------------" << endl;
	cout << "----------------------------------------------------------------" << endl;
	cout << "The miniSQL supports following datastructure: int,floag,char(n) " << endl;
	cout << "This miniSQL supports following commands:" << endl;
	cout << "create database:       create database databasename;" << endl;
	cout << "create table:          create table tablename(" << endl;
	cout << "                           attr1   attr1type, " << endl;
	cout << "                           attr2   attr2type unique(optional), " << endl;
	cout << "                           ..." << endl;
	cout << "                           attrn   attrntype, " << endl;
	cout << "                           primary key (attr)" << endl;
	cout << "                       );" << endl;
	cout << "      caution:last line in the brackets won't have a comma" << endl;
	cout << "create index:          create indexname on tablename(attrname);" << endl;
	cout << "drop database:         drop database databasename;" << endl;
	cout << "drop table:            drop table tablename;" << endl;
	cout << "drop index:            drop index indexname;" << endl;
	cout << "insert into:           insert into tablename values(val1,val2,..,valn);" << endl;
	cout << "delete:                delete attrname from tablename where op1 and/or op2... opn;" << endl;
	cout << "select:                select attrname from tablename where op1 and/or op2... opn;" << endl;
	cout << "      caution:where in delete and select are optional" << endl;
	cout << "use:                   use databasename;" << endl;
	cout << "execfile:              execfile filename;" << endl;
	cout << "help:                  help;" << endl;
	cout << "quit:                  quit;" << endl;
}

void print_error(int errorCode){
	switch (errorCode){
	case 1:
		cout << "Error: Database not found" << endl;
		break;
	case 2:
		cout << "Error: Database already exists" << endl;
		break;
	case 3:
		cout << "Error: Table not found" << endl;
		break;
	case 4:
		cout << "Error: Table already exists" << endl;
		break;
	case 5:
		cout << "Error: No database is assigned" << endl;
		break;
	case 6:
		cout << "Error: Attribute not exists" << endl;
		break;
	case 7:
		cout << "Error: Index already exists" << endl;
		break;
	case 8:
		cout << "Error: Index not found" << endl;
		break;
	case 9:
		cout << "Error: Attribute not found" << endl;
		break;
	case 10:
		cout << "Error: Only Unique attributes can create index" << endl;
		break;
	case 11:
		cout << "Error: The number of insert attributes is incorrect" << endl;
		break;
	case 12:
		cout << "Error: The type of insert value is incorrect" << endl;
		break;
	case 13:
		cout << "Error: Can not insert NULL to a primary key" << endl;
		break;
	case 14:
		cout << "Error: Number of chars is exceeded" << endl;
		break;
	case 15:
		cout << "Error: Unique attributes can only be inserted once" << endl;
		break;
	case 16:
		cout << "Error: No attributes found in this table" << endl;
		break;
	}
}
