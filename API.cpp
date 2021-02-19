#include "API.h"

API::API():buffer(),record(&buffer),index(&buffer){
}

void API::run() {
	int opId;
	bool ifOver = false;
	while(1) {
		/*使用interpreter读取指令
		解析string数组*/
		strInstr = Interpreter("");
		instr = SQL_Modify(strInstr);
		opId = atoi(instr.opId.c_str());
		try{
			switch(opId){
				case __create_table:
					createTable();
					break;
				case __create_database:
					createDatabase();
					break;
				case __create_index:
					createIndex();
					break;
				case __drop_table:
					dropTable();
					break;
				case __drop_database:
					dropDatabase();
					break;
				case __drop_index:
					dropIndex();
					break;
				case __insert:
					insert();
					break;
				case __delete_no_where:
					deleteNoWhere();
					break;
				case __delete_with_where:
					deleteWithWhere();
					break;
				case __select_no_where:
					selectNoWhere();
					break;
				case __select_with_where:
					selectWithWhere();
					break;
				case __use:
					useDatabase();
					break;
				case __execfile:
					execFile();
					break;
				case __quit:
					buffer.close_database(catalog.DB_Used);
					ifOver = true;
					break;
				case __error:
					break;
				case __help:
					print_help();
					break;
			}
		}
		catch(int errorId){
			print_error(errorId);
		}
		if(ifOver == true){
			break;
		}
	}
}

void API::useDatabase() {
	buffer.close_database(catalog.DB_Used);
	catalog.use_database(instr.opObj);

	cout << "database has changed" << endl;
}

void API::judgeAttrExit(string Attr_Name[32], int attrCount, ATTRIBUTEINFO *attr, int totalCount) {
	int i, j;
	bool flag;
	for (i = 0; i < attrCount; i++){
		flag = 0;
		for (j = 0; j < totalCount; j++){
			if (Attr_Name[i] == attr[j].attributeName){
				flag = 1;
				break;
			}

		}
		if (flag == 0)
			throw 16;
	}
}

void API::deleteWithWhere(){
	ATTRIBUTEINFO attrList[32]; //表中全部的属性
	char totalCount; //全部属性的个数
	string tableName = instr.opObj;
	int attrWhereId, attrId, attrSelectId;

	catalog.get_table_attr(tableName, attrList, &totalCount);
	//如果选择不为*，则从判断属性在catalog中是否存在
	if (instr.attrSelect[0] != "*"){
		judgeAttrExit(instr.attrSelect, instr.attrSelectNum, attrList, totalCount);
	}
	else{
		instr.attrSelectNum = totalCount;
		for (int i = 0; i<totalCount; i++){
			instr.attrSelect[i] = attrList[i].attributeName;
		}
	}
	//判断where后面的属性在catalog中是否存在
	judgeAttrExit(instr.attrWhere, instr.attrWhereNum, attrList, totalCount);
	//判断where后面的属性的值是否合法 
	int charLength;
	for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
		switch (judgeType(instr.val[attrWhereId])){
		case __string_type:
			charLength = instr.val[attrWhereId].length();
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (charLength - 2>attrList[attrId].type_count){
						throw 15;
					}
					if (attrList[attrId].type != 'c'){
						throw 12;
					}
					break;
				}
			}
			break;
		case __null_type:
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (attrList[attrId].is_primary){
						throw 13;
					}
					break;
				}
			}
			break;
		case __int_float_type:
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (attrList[attrId].type == 'c'){
						throw 12;
					}
					break;
				}
			}
			break;
		case __float_type:
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (attrList[attrId].type != 'f'){
						throw 12;
					}
					break;
				}
			}
			break;
		case __error_type:
			throw 12;
			break;
		}
	}

	ATTRRECORD *attrWhereInfo = new ATTRRECORD[instr.attrWhereNum];
	ATTRRECORD *attrSelectInfo = new ATTRRECORD[instr.attrSelectNum];
	int pointer = 1;
	bool has_or = false;

	for (attrId = 0; attrId < totalCount; attrId++){
		for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
			if (attrList[attrId].attributeName == instr.attrWhere[attrWhereId]){
				if (attrWhereId == 0)
					attrWhereInfo[attrWhereId].andOr = 'a';
				else{
					if (instr.isAnd[attrWhereId - 1])
						attrWhereInfo[attrWhereId].andOr = 'a';
					else{
						attrWhereInfo[attrWhereId].andOr = 'o';
						has_or = true;
					}
				}
				attrWhereInfo[attrWhereId].has_index = attrList[attrId].has_index;
				attrWhereInfo[attrWhereId].name = attrList[attrId].attributeName;
				attrWhereInfo[attrWhereId].op = instr.op[attrWhereId];
				attrWhereInfo[attrWhereId].point = pointer;
				attrWhereInfo[attrWhereId].type = attrList[attrId].type;
				attrWhereInfo[attrWhereId].charNum = attrList[attrId].type_count;
				attrWhereInfo[attrWhereId].val = instr.val[attrWhereId];
			}
		}
		for (attrSelectId = 0; attrSelectId < instr.attrSelectNum; attrSelectId++){
			if (attrList[attrId].attributeName == instr.attrSelect[attrSelectId]){
				attrSelectInfo[attrSelectId].has_index = attrList[attrId].has_index;
				attrSelectInfo[attrSelectId].name = attrList[attrId].attributeName;
				attrSelectInfo[attrSelectId].point = pointer;
				attrSelectInfo[attrSelectId].type = attrList[attrId].type;
				attrSelectInfo[attrSelectId].charNum = attrList[attrId].type_count;
			}
		}
		switch (attrList[attrId].type)
		{
		case 'i':
			pointer += sizeof(int) + 1;
			break;
		case 'f':
			pointer += sizeof(int) + 1;
			break;
		case 'c':
			pointer += attrList[attrId].type_count + 2;
			break;
		default:
			break;
		}
	}

	int numOfRecords = record.GetNumOfRecords(catalog.DB_Used, tableName);	//表中非空记录数 
	int *recordLeft = new int[numOfRecords];
	int *recordAndRight;
	int *recordTemp;
	int recordLeftCount = numOfRecords;
	int recordAndRightCount;
	int recordTempCount;

	for (int i = 0; i < numOfRecords; i++)
		recordLeft[i] = i;
	if (!has_or){
		for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
			if (attrWhereInfo[attrWhereId].has_index){
				struct KeyInfo keyInfo;
				SearchArray *searchRes;
				keyInfo.attribute = attrWhereInfo[attrWhereId].name;
				keyInfo.value.type = attrWhereInfo[attrWhereId].type;
				switch (attrWhereInfo[attrWhereId].type){
				case 'i':
					keyInfo.value.intKey = toInt(attrWhereInfo[attrWhereId].val);
					keyInfo.value.length = sizeof(int);
					break;
				case 'f':
					keyInfo.value.floatKey = toFloat(attrWhereInfo[attrWhereId].val);
					keyInfo.value.length = sizeof(float);
					break;
				case 'c':
					keyInfo.value.stringKey = toString(attrWhereInfo[attrWhereId].val);
					keyInfo.value.length = attrWhereInfo[attrWhereId].charNum;
					break;
				}
				if (attrWhereInfo[attrWhereId].op != __not_equal){
					searchRes = index.Search(catalog.DB_Used, tableName, attrWhereInfo[attrWhereId].op, &keyInfo);
					recordAndRight = searchRes->indexNum;
					recordAndRightCount = searchRes->num;
					delete searchRes;
				}
				else{
					searchRes = index.Search(catalog.DB_Used, tableName, __above, &keyInfo);
					recordAndRight = searchRes->indexNum;
					recordAndRightCount = searchRes->num;
					searchRes = index.Search(catalog.DB_Used, tableName, __below, &keyInfo);
					recordTempCount = max(searchRes->num, recordAndRightCount);
					recordTemp = new int[recordTempCount];
					mergeIndex(recordAndRight, searchRes->indexNum, recordTemp, recordAndRightCount, searchRes->num, recordTempCount, 'o');
					delete[]recordAndRight;
					delete[]searchRes->indexNum;
					recordAndRight = recordTemp;
					recordAndRightCount = recordTempCount;
					delete searchRes;
				}

				recordTempCount = max(recordLeftCount, recordAndRightCount);
				recordTemp = new int[recordTempCount];
				mergeIndex(recordLeft, recordAndRight, recordTemp, recordLeftCount, recordAndRightCount, recordTempCount, 'a');
				delete[] recordLeft;
				//delete[] recordAndRight;
				recordLeft = recordTemp;
				recordLeftCount = recordTempCount;
			}
		}
	}

	int recordnum = 0;		//用来记录输出的记录数 ，out.val的纵栏 
	char *recordGetChar;	//表中每条记录 (char)
	char recordHeadChar;	//每条记录或每个属性前的head判断是否为空； 
	showOut out;	//输出

	out.attrNum = instr.attrSelectNum;
	out.val = new string *[out.attrNum];
	for (int i = 0; i<out.attrNum; i++)
		out.val[i] = new string[numOfRecords];
	for (int selectId = 0; selectId < instr.attrSelectNum; selectId++){
		out.attrName[selectId] = attrSelectInfo[selectId].name;
	}

	float floatTemp;
	int intTemp;
	char charTemp[256];
	int recordLeftId;
	string attrRes;
	bool if_show;
	for (recordLeftId = 0; recordLeftId < recordLeftCount; recordLeftId++){
		if_show = 1;
		recordGetChar = record.Select(catalog.DB_Used, tableName, recordLeft[recordLeftId]);
		recordHeadChar = *(char*)recordGetChar;
		if (recordHeadChar == 'N'){
			continue;
		}
		//检测值是否符合
		for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
			if (attrWhereInfo[attrWhereId].andOr == 'a'){
				if (if_show){
					recordHeadChar = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point);
					if (recordHeadChar == 'N'){
						attrRes = "null";
						if (attrRes != attrWhereInfo[attrWhereId].val)
							if_show = 0;
						continue;
					}
					else{
						switch (attrWhereInfo[attrWhereId].type)
						{
						case 'i':
							intTemp = *(int*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
							if (!judgeInt(intTemp, toInt(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
								if_show = 0;
							break;
						case 'f':
							floatTemp = *(float*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
							if (!judgeFloat(floatTemp, toFloat(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
								if_show = 0;
							break;
						case 'c':
							for (int i = 0; i < attrWhereInfo[attrWhereId].charNum; i++){
								charTemp[i] = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point + i + 1);
							}
							charTemp[attrWhereInfo[attrWhereId].charNum] = '\0';
							attrRes = charTemp;
							if (!judgeString(attrRes, toString(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
								if_show = 0;
							break;
						}
					}
				}
				else{
					continue;
				}
			}
			else if (attrWhereInfo[attrWhereId].andOr == 'o'){
				if (if_show)
					break;
				else {
					if_show = 1;
					if (if_show){
						recordHeadChar = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point);
						if (recordHeadChar == 'N'){
							attrRes = "null";
							if (attrRes != attrWhereInfo[attrWhereId].val)
								if_show = 0;
							continue;
						}
						else{
							switch (attrWhereInfo[attrWhereId].type)
							{
							case 'i':
								intTemp = *(int*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
								if (!judgeInt(intTemp, toInt(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
									if_show = 0;
								break;
							case 'f':
								floatTemp = *(float*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
								if (!judgeFloat(floatTemp, toFloat(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
									if_show = 0;
								break;
							case 'c':
								for (int i = 0; i < attrWhereInfo[attrWhereId].charNum; i++){
									charTemp[i] = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point + i + 1);
								}
								charTemp[attrWhereInfo[attrWhereId].charNum] = '\0';
								attrRes = charTemp;
								if (!judgeString(attrRes, toString(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
									if_show = 0;
								break;
							}
						}
					}
				}
			}
		}
		//如果值符合，则展示
		if (if_show){
			record.Delete(catalog.DB_Used, tableName, recordLeft[recordLeftId]);
			recordnum++;
		}
	}
	cout << recordnum << "rows have been deleted" << endl;
	delete[] attrWhereInfo;
	delete[] attrSelectInfo;

}

void API::deleteNoWhere(){
	string tableName = instr.opObj;
	
	int recordId;	//用来遍历表中所有record 
	char *recordGetChar;	//表中每条记录 (char)
	char recordHeadChar;	//每条记录或每个属性前的head判断是否为空； 
	
	for (recordId = 0; recordId < record.GetTotalNum(catalog.DB_Used, tableName); recordId++){
		recordGetChar = record.Select(catalog.DB_Used, tableName, recordId);
		recordHeadChar = *(char*)recordGetChar;
		if (recordHeadChar == 'N'){
			continue;
		}
		record.Delete(catalog.DB_Used, tableName, recordId);
	}
	cout << "the table has been deleted"<<endl;
}

void API::selectWithWhere() {
	ATTRIBUTEINFO attrList[32]; //表中全部的属性
	char totalCount; //全部属性的个数
	string tableName = instr.opObj;
	int attrWhereId, attrId, attrSelectId;	

	catalog.get_table_attr(tableName, attrList, &totalCount);
	//如果选择不为*，则从判断属性在catalog中是否存在
	if (instr.attrSelect[0] != "*"){
		judgeAttrExit(instr.attrSelect, instr.attrSelectNum, attrList, totalCount);
	}
	else{
		instr.attrSelectNum = totalCount;
		for(int i=0; i<totalCount; i++){
			instr.attrSelect[i] = attrList[i].attributeName;
		}
	}
	//判断where后面的属性在catalog中是否存在
	judgeAttrExit(instr.attrWhere, instr.attrWhereNum, attrList, totalCount);
	//判断where后面的属性的值是否合法 
	int charLength;
	for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
		switch (judgeType(instr.val[attrWhereId])){
		case __string_type:
			charLength = instr.val[attrWhereId].length();
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (charLength - 2>attrList[attrId].type_count){
						throw 15;
					}
					if (attrList[attrId].type != 'c'){
						throw 12;
					}
					break;
				}
			}
			break;
		case __null_type:
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (attrList[attrId].is_primary){
						throw 13;
					}
					break;
				}
			}
			break;
		case __int_float_type:
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (attrList[attrId].type == 'c'){
						throw 12;
					}
					break;
				}
			}
			break;
		case __float_type:
			for (attrId = 0; attrId < totalCount; attrId++){
				if (instr.attrWhere[attrWhereId] == attrList[attrId].attributeName){
					if (attrList[attrId].type != 'f'){
						throw 12;
					}
					break;
				}
			}
			break;
		case __error_type:
			throw 12;
			break;
		}
	}

	ATTRRECORD *attrWhereInfo = new ATTRRECORD[instr.attrWhereNum];
	ATTRRECORD *attrSelectInfo = new ATTRRECORD[instr.attrSelectNum];
	int pointer=1;
	bool has_or = false;

	for (attrId = 0; attrId < totalCount; attrId++){
		for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
			if (attrList[attrId].attributeName == instr.attrWhere[attrWhereId]){
				if (attrWhereId == 0)
					attrWhereInfo[attrWhereId].andOr = 'a';
				else{
					if (instr.isAnd[attrWhereId-1])
						attrWhereInfo[attrWhereId].andOr = 'a';
					else{
						attrWhereInfo[attrWhereId].andOr = 'o';
						has_or = true;
					}
				}
				attrWhereInfo[attrWhereId].has_index = attrList[attrId].has_index;
				attrWhereInfo[attrWhereId].name = attrList[attrId].attributeName;
				attrWhereInfo[attrWhereId].op = instr.op[attrWhereId];
				attrWhereInfo[attrWhereId].point = pointer;
				attrWhereInfo[attrWhereId].type = attrList[attrId].type;
				attrWhereInfo[attrWhereId].charNum = attrList[attrId].type_count;
				attrWhereInfo[attrWhereId].val = instr.val[attrWhereId];
			}
		}
		for (attrSelectId = 0; attrSelectId < instr.attrSelectNum; attrSelectId++){
			if (attrList[attrId].attributeName == instr.attrSelect[attrSelectId]){
				attrSelectInfo[attrSelectId].has_index = attrList[attrId].has_index;
				attrSelectInfo[attrSelectId].name = attrList[attrId].attributeName;
				attrSelectInfo[attrSelectId].point = pointer;
				attrSelectInfo[attrSelectId].type = attrList[attrId].type;
				attrSelectInfo[attrSelectId].charNum = attrList[attrId].type_count;
			}
		}
		switch (attrList[attrId].type)
		{
		case 'i':
			pointer += sizeof(int) + 1;
			break;
		case 'f':
			pointer += sizeof(int) + 1;
			break;
		case 'c':
			pointer += attrList[attrId].type_count + 2;
			break;
		default:
			break;
		}
	}

	int numOfRecords = record.GetNumOfRecords(catalog.DB_Used, tableName);	//表中非空记录数 
	int *recordLeft = new int[numOfRecords];
	int *recordAndRight;
	int *recordTemp;
	int recordLeftCount = numOfRecords;
	int recordAndRightCount;
	int recordTempCount;
	
	for (int i = 0; i < numOfRecords; i++)
		recordLeft[i] = i;
	if (!has_or){
		for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
			if (attrWhereInfo[attrWhereId].has_index){
				struct KeyInfo keyInfo;
				SearchArray *searchRes;
				keyInfo.attribute = attrWhereInfo[attrWhereId].name;
				keyInfo.value.type = attrWhereInfo[attrWhereId].type;
				switch (attrWhereInfo[attrWhereId].type){
				case 'i':
					keyInfo.value.intKey = toInt(attrWhereInfo[attrWhereId].val);
					keyInfo.value.length = sizeof(int);
					break;
				case 'f':
					keyInfo.value.floatKey = toFloat(attrWhereInfo[attrWhereId].val);
					keyInfo.value.length = sizeof(float);
					break;
				case 'c':
					keyInfo.value.stringKey = toString(attrWhereInfo[attrWhereId].val);
					keyInfo.value.length = attrWhereInfo[attrWhereId].charNum;
					break;
				}
				if (attrWhereInfo[attrWhereId].op != __not_equal){
					searchRes = index.Search(catalog.DB_Used, tableName, attrWhereInfo[attrWhereId].op, &keyInfo);
					recordAndRight = searchRes->indexNum;
					recordAndRightCount = searchRes->num;
					delete searchRes;
				}
				else{
					searchRes = index.Search(catalog.DB_Used, tableName, __above, &keyInfo);
					recordAndRight = searchRes->indexNum;
					recordAndRightCount = searchRes->num;
					searchRes = index.Search(catalog.DB_Used, tableName, __below, &keyInfo);
					recordTempCount = max(searchRes->num, recordAndRightCount);
					recordTemp = new int[recordTempCount];
					mergeIndex(recordAndRight, searchRes->indexNum, recordTemp, recordAndRightCount, searchRes->num, recordTempCount, 'o');
					delete[]recordAndRight;
					delete[]searchRes->indexNum;
					recordAndRight = recordTemp;
					recordAndRightCount = recordTempCount;
					delete searchRes;
				}
				
				recordTempCount = max(recordLeftCount, recordAndRightCount);
				recordTemp = new int[recordTempCount];
				mergeIndex(recordLeft, recordAndRight, recordTemp, recordLeftCount, recordAndRightCount, recordTempCount, 'a');
				delete[] recordLeft;
				//delete[] recordAndRight;
				recordLeft = recordTemp;
				recordLeftCount = recordTempCount;
			}
		}
	}

	int recordnum = 0;		//用来记录输出的记录数 ，out.val的纵栏 
	char *recordGetChar;	//表中每条记录 (char)
	char recordHeadChar;	//每条记录或每个属性前的head判断是否为空； 
	showOut out;	//输出
	
	out.attrNum = instr.attrSelectNum;
	out.val = new string *[out.attrNum];
	for (int i = 0; i<out.attrNum; i++)
		out.val[i] = new string[numOfRecords];
	for (int selectId = 0; selectId < instr.attrSelectNum; selectId++){
		out.attrName[selectId] = attrSelectInfo[selectId].name;
	}

	float floatTemp;
	int intTemp;
	char charTemp[256];
	int recordLeftId;
	string attrRes;
	bool if_show;
	for (recordLeftId = 0; recordLeftId < recordLeftCount; recordLeftId++){
		if_show = 1;
		recordGetChar = record.Select(catalog.DB_Used, tableName, recordLeft[recordLeftId]);
		recordHeadChar = *(char*)recordGetChar;
		if (recordHeadChar == 'N'){
			continue;
		}
		//检测值是否符合
		for (attrWhereId = 0; attrWhereId < instr.attrWhereNum; attrWhereId++){
			if (attrWhereInfo[attrWhereId].andOr == 'a'){
				if (if_show){
					recordHeadChar = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point);
					if (recordHeadChar == 'N'){
						attrRes = "null";
						if (attrRes != attrWhereInfo[attrWhereId].val)
							if_show = 0;
						continue;
					}
					else{
						switch (attrWhereInfo[attrWhereId].type)
						{
						case 'i':
							intTemp = *(int*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
							if (!judgeInt(intTemp, toInt(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
								if_show = 0;
							break;
						case 'f':
							floatTemp = *(float*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
							if (!judgeFloat(floatTemp, toFloat(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
								if_show = 0;
							break;
						case 'c':
							for (int i = 0; i < attrWhereInfo[attrWhereId].charNum; i++){
								charTemp[i] = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point + i + 1);
							}
							charTemp[attrWhereInfo[attrWhereId].charNum] = '\0';
							attrRes = charTemp;
							if (!judgeString(attrRes, toString(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
								if_show = 0;
							break;
						}
					}
				}
				else{
					continue;
				}
			}
			else if (attrWhereInfo[attrWhereId].andOr == 'o'){
				if (if_show)
					break;
				else {
					if_show = 1;
					if (if_show){
						recordHeadChar = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point);
						if (recordHeadChar == 'N'){
							attrRes = "null";
							if (attrRes != attrWhereInfo[attrWhereId].val)
								if_show = 0;
							continue;
						}
						else{
							switch (attrWhereInfo[attrWhereId].type)
							{
							case 'i':
								intTemp = *(int*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
								if (!judgeInt(intTemp, toInt(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
									if_show = 0;
								break;
							case 'f':
								floatTemp = *(float*)(recordGetChar + attrWhereInfo[attrWhereId].point + 1);
								if (!judgeFloat(floatTemp, toFloat(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
									if_show = 0;
								break;
							case 'c':
								for (int i = 0; i < attrWhereInfo[attrWhereId].charNum; i++){
									charTemp[i] = *(char*)(recordGetChar + attrWhereInfo[attrWhereId].point + i + 1);
								}
								charTemp[attrWhereInfo[attrWhereId].charNum] = '\0';
								attrRes = charTemp;
								if (!judgeString(attrRes, toString(attrWhereInfo[attrWhereId].val), attrWhereInfo[attrWhereId].op))
									if_show = 0;
								break;
							}
						}
					}
				}
			}
		}
		//如果值符合，则展示
		if (if_show){
			for (int attrnum = 0; attrnum<instr.attrSelectNum; attrnum++){
				recordHeadChar = *(char*)(recordGetChar + attrSelectInfo[attrnum].point);
				if (recordHeadChar == 'N'){
					out.val[attrnum][recordnum] = "null";
				}
				else if (attrSelectInfo[attrnum].type == 'i'){
					char s[256];
					sprintf(s, "%d", *(int*)(recordGetChar + attrSelectInfo[attrnum].point + 1));
					out.val[attrnum][recordnum] = s;
				}
				else if (attrSelectInfo[attrnum].type == 'f'){
					char s[256];
					sprintf(s, "%g", *(float*)(recordGetChar + attrSelectInfo[attrnum].point + 1));
					out.val[attrnum][recordnum] = s;
				}
				else if (attrSelectInfo[attrnum].type == 'c'){
					char s[256];
					int i;
					for (i = 0; i < attrList[attrId].type_count; i++){
						s[i] = *(char*)(recordGetChar + attrSelectInfo[attrnum].point + i + 1);
					}
					s[i] = 0;
					out.val[attrnum][recordnum] = s;
					out.val[attrnum][recordnum] = "\'" + out.val[attrnum][recordnum] + "\'";
				}
			}
			recordnum++;
		}
	}

	out.recordLength = recordnum;
	output(out);

	for (int i = 0; i < out.attrNum; i++)
		delete[] out.val[i];
	delete[] out.val;
	delete[] attrWhereInfo;
	delete[] attrSelectInfo;
	
}

void API::selectNoWhere() {
	ATTRIBUTEINFO attrList[32]; //表中全部的属性
	char totalCount; //全部属性的个数
	string tableName = instr.opObj;

	catalog.get_table_attr(tableName, attrList, &totalCount);
	//如果选择不为*，则从判断属性在catalog中是否存在
	if (instr.attrSelect[0] != "*"){
		judgeAttrExit(instr.attrSelect, instr.attrSelectNum, attrList, totalCount);
	}
	else{
		instr.attrSelectNum = totalCount;
		for (int i = 0; i<totalCount; i++){
			instr.attrSelect[i] = attrList[i].attributeName;
		}
	}

	showOut out;	//输出 
	int numOfRecords = record.GetNumOfRecords(catalog.DB_Used, tableName);	//表中非空记录数 

	out.attrNum = instr.attrSelectNum;
	out.val = new string *[out.attrNum];
	for (int i = 0; i<out.attrNum; i++)
		out.val[i] = new string[numOfRecords];

	ATTRRECORD attrSelectInfo[32];
	int attrId;			//用来遍历表中所有属性 
	int attrnum;	//用来记录输出的属性数，out.val的横栏
	int pointer;	//每条记录中偏移量，用来确定下一属性的位置

	attrnum = 0;
	pointer = 1;
	for (attrId = 0; attrId < totalCount; attrId++){
		for (int selectId = 0; selectId<instr.attrSelectNum; selectId++){
			if (attrList[attrId].attributeName == instr.attrSelect[selectId]){
				out.attrName[attrnum] = instr.attrSelect[selectId];
				attrSelectInfo[attrnum].name = attrList[attrId].attributeName;
				attrSelectInfo[attrnum].type = attrList[attrId].type;
				attrSelectInfo[attrnum].charNum = attrList[attrId].type_count;
				attrSelectInfo[attrnum].has_index = attrList[attrId].has_index;
				attrSelectInfo[attrnum].point = pointer;
				attrnum++;
			}
		}
		pointer += 1;
		if (attrList[attrId].type == 'i'){
			pointer += sizeof(int);
		}
		else if (attrList[attrId].type == 'f'){
			pointer += sizeof(float);
		}
		else if (attrList[attrId].type == 'c'){
			pointer += attrList[attrId].type_count + 1;
		}
	}

	int recordId;	//用来遍历表中所有record 
	int recordnum;		//用来记录输出的记录数 ，out.val的纵栏 
	char *recordGetChar;	//表中每条记录 (char)
	char recordHeadChar;	//每条记录或每个属性前的head判断是否为空； 

	recordnum = 0;
	for (recordId = 0; recordId < record.GetTotalNum(catalog.DB_Used, tableName); recordId++){
		recordGetChar = record.Select(catalog.DB_Used, tableName, recordId);
		recordHeadChar = *(char*)recordGetChar;
		if (recordHeadChar == 'N'){
			continue;
		}
		for (int attrnum = 0; attrnum<instr.attrSelectNum; attrnum++){
			recordHeadChar = *(char*)(recordGetChar + attrSelectInfo[attrnum].point);
			if (recordHeadChar == 'N'){
				out.val[attrnum][recordnum] = "null";
			}
			else if (attrSelectInfo[attrnum].type == 'i'){
				char s[256];
				sprintf(s, "%d", *(int*)(recordGetChar + attrSelectInfo[attrnum].point + 1));
				out.val[attrnum][recordnum] = s;
			}
			else if (attrSelectInfo[attrnum].type == 'f'){
				char s[256];
				sprintf(s, "%g", *(float*)(recordGetChar + attrSelectInfo[attrnum].point + 1));
				out.val[attrnum][recordnum] = s;
			}
			else if (attrSelectInfo[attrnum].type == 'c'){
				char s[256];
				int i;
				for (i = 0; i < attrList[attrId].type_count; i++){
					s[i] = *(char*)(recordGetChar + attrSelectInfo[attrnum].point + i + 1);
				}
				s[i] = 0;
				out.val[attrnum][recordnum] = s;
				out.val[attrnum][recordnum] = "\'" + out.val[attrnum][recordnum] + "\'";
			}
		}
		recordnum++;
	}

	out.recordLength = recordnum;
	output(out);
	for (int i = 0; i<out.attrNum; i++)
		delete[]out.val[i];
	delete[]out.val;
}

void API::insert() {
	
	string tableName = instr.opObj;
	int valNum = instr.attrWhereNum;
	char attrNum;
	ATTRIBUTEINFO attr[32];
	int i;
	int recordSize = 1;

	/*cout << "table: " << instr.opObj << endl;
	for (int i = 0; i < valNum; i++) {
		cout << i << " " << instr.val[i] << endl;
	}*/
	//从catalog中获取属性的元数据
	catalog.get_table_attr(tableName, attr, &attrNum);
	//比较个数是否一致
	if (valNum != (int)attrNum){
		throw 11;
	}
	//判断输入值是否有类型错误
	for (i = 0; i < attrNum; i++){
		switch (judgeType(instr.val[i]))
		{
		case __null_type:
			if (attr[i].is_primary)
				throw 13;
			break;
		case __int_float_type:
			if (attr[i].type == 'c')
				throw 12;
			break;
		case __float_type:
			if (attr[i].type != 'f')
				throw 12;
			break;
		case __string_type:
			if (attr[i].type != 'c')
				throw 12;
			if (instr.val[i].length()-2 > (int)attr[i].type_count)
				throw 14;
			break;
		case __error_type:
			throw 12;
			break;
		}
	}
	bool ifUnique[32];	//unique并且没有index的属性 则为true 
	int uniqueCount = 0;
	for (i = 0; i < 32; i++){
		ifUnique[i] = false;
	}
	//判断unique是否有重复
	for (i = 0; i < attrNum; i++) {
		if (attr[i].type == 'f') {
			if (attr[i].unique && attr[i].has_index) {
				struct KeyInfo key;
				SearchArray *searchRes;
				//string fileName = tableName + "_" + attr[i].attributeName;
				key.attribute = attr[i].attributeName;
				key.value.type = 'f';
				key.value.floatKey = toFloat(instr.val[i]);
				key.value.length = 4;
				//searchRes = index.Search(catalog.DB_Used, fileName, __equal, &key);
				searchRes = index.Search(catalog.DB_Used, tableName, __equal, &key);
				if (searchRes->num > 0)
					throw 15;
			}
			else if (attr[i].unique){
				ifUnique[i] = true;
				uniqueCount++;
			}
			recordSize += sizeof(float) + 1;
		}
		else if (attr[i].type == 'i') {
			if (attr[i].unique && attr[i].has_index) {
				struct KeyInfo key;
				SearchArray *searchRes;
				//string fileName = tableName + "_" + attr[i].attributeName;
				key.attribute = attr[i].attributeName;
				key.value.type = 'i';
				key.value.intKey = toInt(instr.val[i]);
				key.value.length = 4;
				//searchRes = index.Search(catalog.DB_Used, fileName, __equal, &key);
				searchRes = index.Search(catalog.DB_Used, tableName, __equal, &key);
				if (searchRes->num > 0)
					throw 15;
			}
			else if (attr[i].unique){
				ifUnique[i] = true;
				uniqueCount++;
			}
			recordSize += sizeof(int) + 1;
		}
		else if (attr[i].type == 'c') {
			if (attr[i].unique && attr[i].has_index) {
				struct KeyInfo key;
				SearchArray *searchRes;
				//string fileName = tableName + "_" + attr[i].attributeName;
				string stringKey = toString(instr.val[i]);
				key.attribute = attr[i].attributeName;
				key.value.type = 'c';
				key.value.length = attr[i].type_count;
				key.value.stringKey = stringKey;
				//searchRes = index.Search(catalog.DB_Used, fileName, __equal, &key);
				searchRes = index.Search(catalog.DB_Used, tableName, __equal, &key);
				if (searchRes->num > 0)
					throw 15;
			}
			else if (attr[i].unique){
				ifUnique[i] = true;
				uniqueCount++;
			}
			recordSize += attr[i].type_count*sizeof(char) + 2;
		}
	}
	int recordId;
	int attrId;
	char *recordGetChar;
	char recordHeadChar;
	int pointer;
	int attrInt;
	float attrFloat;
	char attrChar[256];
	//如果有unique且没有index的，则需一条一条查询重复
	if (uniqueCount != 0){
		for (recordId = 0; recordId < record.GetTotalNum(catalog.DB_Used, tableName); recordId++){
			recordGetChar = record.Select(catalog.DB_Used, tableName, recordId);
			recordHeadChar = *(char*)recordGetChar;
			if (recordHeadChar == 'N'){
				continue;
			}
			pointer = 1;
			for (attrId = 0; attrId < attrNum; attrId++){
				recordHeadChar = *(char*)(recordGetChar + pointer);
				pointer += 1;
				if (attr[attrId].type == 'i'){
					attrInt = *(int*)(recordGetChar + pointer);
					pointer += sizeof(int);
					if ( attr[attrId].unique && attrInt == toInt(instr.val[attrId]))
						throw 15;
				}
				else if (attr[attrId].type == 'f'){
					attrFloat = *(float*)(recordGetChar + pointer);
					pointer += sizeof(float);
					if ( attr[attrId].unique && attrFloat == toFloat(instr.val[attrId]))
						throw 15;
				}
				else if (attr[attrId].type == 'c'){
					for (i = 0; i < attr[attrId].type_count; i++){
						attrChar[i] = *(char*)(recordGetChar + pointer + i);
					}
					attrChar[i] = 0;
					pointer += attr[attrId].type_count + 1;
					if (attr[attrId].unique && !strcmp(attrChar, toString(instr.val[attrId]).c_str()))
						throw 15;
				}
			}
		}
	}

	char* recordContent = new char[recordSize];
	*(char*)recordContent = 'Y';
	pointer = 1;
	//生成插入内容
	for (attrId = 0; attrId < attrNum; attrId++) {
		if (attr[attrId].type == 'f') {
			if (judgeType(instr.val[attrId]) == __null_type){
				*(char*)(recordContent+pointer) = 'N';
				pointer += 1 + sizeof(float);
			}
			else {
				*(char*)(recordContent+pointer) = 'Y';
				pointer += 1;
				*(float*)(recordContent + pointer) = toFloat(instr.val[attrId]);
				pointer += sizeof(float);
			}
		}
		else if (attr[attrId].type == 'i') {
			if (judgeType(instr.val[attrId]) == __null_type){
				*(char*)(recordContent + pointer) = 'N';
				pointer += 1 + sizeof(int);
			}
			else {
				*(char*)(recordContent + pointer) = 'Y';
				pointer += 1;
				*(int*)(recordContent + pointer) = toInt(instr.val[attrId]);
				pointer += sizeof(int);
			}
		}
		else if (attr[attrId].type == 'c') {
			if (judgeType(instr.val[attrId]) == __null_type){
				*(char*)(recordContent + pointer) = 'N';
				pointer += 2 + attr[attrId].type_count;
			}
			else{
				*(char*)(recordContent + pointer) = 'Y';
				pointer += 1;
				strcpy(attrChar,toString(instr.val[attrId]).c_str());
				for (i = 0; i < attr[attrId].type_count; i++){
					*(char*)(recordContent + pointer + i) = attrChar[i];
				}
				*(char*)(recordContent + pointer + i) = 0;
				pointer += 1 + attr[attrId].type_count;
			}
		}
	}

	int recordID;
	recordID = record.Insert(catalog.DB_Used, tableName, recordContent);
	//删除指针中的内容
	delete[] recordContent;
	
	//判断每个属性是否有index，如果有，且该值不为null，则插入index
	for (attrId = 0; attrId < attrNum; attrId++) {
		if (attr[attrId].has_index && judgeType(instr.val[attrId])!=__null_type){
			struct KeyInfo indexInfo;
			indexInfo.attribute = attr[attrId].attributeName;
			indexInfo.value.type =  attr[attrId].type;
			switch (indexInfo.value.type)
			{
				case 'i':
					indexInfo.value.intKey = toInt(instr.val[attrId]);
					indexInfo.value.length = 4;
					break;
				case 'f':
					indexInfo.value.floatKey = toFloat(instr.val[attrId]);
					indexInfo.value.length = 4;
					break;
				case 'c':
					indexInfo.value.stringKey = toString(instr.val[attrId]).c_str();
					indexInfo.value.length = attr[attrId].type_count;
					break;
			}
			indexInfo.indexNum = recordID;
			
			index.Insert(catalog.DB_Used, tableName, &indexInfo);
		}
	}
	cout << "insert 1 row succeddfully"<<endl;
}

bool API::judgeInt(int a, int b, int op){
	switch (op){
	case __equal:
		if (a == b)
			return true;
		else
			return false;
		break;
	case __above:
		if (a > b)
			return true;
		else
			return false;
		break;
	case __below:
		if (a < b)
			return true;
		else
			return false;
	case __above_equal:
		if (a >= b)
			return true;
		else
			return false;
	case __below_equal:
		if (a <= b)
			return true;
		else
			return false;
	case __not_equal:
		if (a != b)
			return true;
		else
			return false;
	}
}
bool API::judgeFloat(float a, float b, int op){
	switch (op){
	case __equal:
		if (a == b)
			return true;
		else
			return false;
		break;
	case __above:
		if (a > b)
			return true;
		else
			return false;
		break;
	case __below:
		if (a < b)
			return true;
		else
			return false;
	case __above_equal:
		if (a >= b)
			return true;
		else
			return false;
	case __below_equal:
		if (a <= b)
			return true;
		else
			return false;
	case __not_equal:
		if (a != b)
			return true;
		else
			return false;
	}
}
bool API::judgeString(string a, string b, int op){
	switch (op){
	case __equal:
		if (a == b)
			return true;
		else
			return false;
		break;
	case __above:
		if (a > b)
			return true;
		else
			return false;
		break;
	case __below:
		if (a < b)
			return true;
		else
			return false;
	case __above_equal:
		if (a >= b)
			return true;
		else
			return false;
	case __below_equal:
		if (a <= b)
			return true;
		else
			return false;
	case __not_equal:
		if (a != b)
			return true;
		else
			return false;
	}
}

void API::dropIndex() {
	string tableName;
	string attrName;
	string indexName = instr.opObj;
	if(catalog.if_index_exists(indexName, &tableName, &attrName)){
		string fileName = tableName+"_"+attrName;
		buffer.delete_file(catalog.DB_Used, fileName);
	}
	catalog.drop_index(indexName);

	cout << "index has been droped" << endl;
}

void API::dropDatabase() {
	if(instr.opObj == catalog.DB_Used){
		buffer.close_database(catalog.DB_Used);
	}
	catalog.drop_database(instr.opObj);

	cout << "database has been droped" << endl;
}

void API::dropTable() {
	cout << "tableName: " << instr.opObj << endl;
	char attrCount;
	int i;
	string tableName = instr.opObj;
	string fileName;
	ATTRIBUTEINFO attr[32];
	catalog.get_table_attr(tableName, attr, &attrCount);
	for(i=0; i<attrCount; i++) {
		if(attr[i].has_index){
			fileName = tableName+"_"+attr[i].attributeName+"_table";
			buffer.delete_file(catalog.DB_Used,fileName);
		}
	}
	buffer.delete_file(catalog.DB_Used,tableName);
	catalog.drop_table(tableName);

	cout << "table has been drop" << endl;
}

void API::createIndex() {
	string tableName = instr.attrSelect[0];
	string attrName = instr.attrSelect[1];
	string indexName = instr.opObj;
	catalog.create_index(tableName, attrName, indexName);
	index.Create(catalog.DB_Used,tableName,attrName);
	
	
	int attrId;
	int pointer;
	char attrNum;
	ATTRIBUTEINFO attr[32];
	//从catalog中获取属性的元数据
	catalog.get_table_attr(tableName, attr, &attrNum);
	//找到该属性在每条记录中的偏移量 
	pointer = 1;
	for (attrId = 0; attrId < attrNum; attrId++){
		if( attr[attrId].attributeName == attrName )
			break;
		pointer += 1;
		if (attr[attrId].type == 'i'){
			pointer += sizeof(int);
		}
		else if (attr[attrId].type == 'f'){
			pointer += sizeof(float);
		}
		else if (attr[attrId].type == 'c'){
			pointer += attr[attrId].type_count + 1;
		}
	}
	
	int recordId;
	char *recordGetChar;
	char recordHeadChar;
	int attrInt;
	float attrFloat;
	char attrChar[256];
	struct KeyInfo indexInfo;
	indexInfo.attribute = attrName;
	for (recordId = 0; recordId < record.GetTotalNum(catalog.DB_Used, tableName); recordId++){
		recordGetChar = record.Select(catalog.DB_Used, tableName, recordId);
		recordHeadChar = *(char*)recordGetChar;
		if (recordHeadChar == 'N'){
			continue;
		}
		recordHeadChar = *(char*)(recordGetChar + pointer);
		if( recordHeadChar == 'N')
			continue;
		indexInfo.indexNum = recordId;
		if (attr[attrId].type == 'i'){
			indexInfo.value.type = 'i';
			indexInfo.value.intKey = *(int*)(recordGetChar + pointer + 1);
			indexInfo.value.length = 4;
		}
		else if (attr[attrId].type == 'f'){
			indexInfo.value.type = 'f';
			indexInfo.value.floatKey = *(float*)(recordGetChar + pointer + 1);
			indexInfo.value.length = 4;
		}
		else if (attr[attrId].type == 'c'){
			indexInfo.value.type = 'c';
			int i;
			for (i = 0; i < attr[attrId].type_count; i++){
				attrChar[i] = *(char*)(recordGetChar + pointer + 1 + i);
			}
			attrChar[i] = 0;
			indexInfo.value.stringKey = attrChar;
			indexInfo.value.length = i;
		}
		
		index.Insert(catalog.DB_Used,tableName, &indexInfo);
	}

	cout << "create index successfully" << endl;
}

void API::createTable() {
	TABLEINFO table;
	ATTRIBUTEINFO attrinfo[32];
	int attrCount;
	//cout << "tableName: " << instr.opObj << endl;
	table.tableName = instr.opObj;
	//cout << "attrCount: " << instr.attrWhereNum  << endl;
	table.attribute_count = instr.attrWhereNum;
	table.record_count = 0;
	table.record_size = 1;
	for(attrCount = 0; attrCount < table.attribute_count; attrCount++){
		/*
		cout << attrCount << " " << instr.attrWhere[attrCount]
			 << " " << instr.attrType[attrCount]
			 << " " << instr.primaryKey
			 << " " << instr.uniqueFlag[attrCount] << endl;
		*/
		attrinfo[attrCount].attributeName = instr.attrWhere[attrCount];
		attrinfo[attrCount].has_index = false;
		if(instr.attrType[attrCount] == "+"){
			attrinfo[attrCount].type = 'i';
			table.record_size += sizeof(int)+1;
		}
		else if(instr.attrType[attrCount] == "-"){
			attrinfo[attrCount].type = 'f';
			table.record_size += sizeof(float)+1;
		}
		else{
			attrinfo[attrCount].type = 'c';
			attrinfo[attrCount].type_count = (char)toInt(instr.attrType[attrCount]);
			table.record_size += attrinfo[attrCount].type_count*sizeof(char)+2;
		}
		if(attrinfo[attrCount].attributeName == instr.primaryKey){
			attrinfo[attrCount].is_primary=true;
			attrinfo[attrCount].unique = true;
			attrinfo[attrCount].has_index = true;
			index.Create(catalog.DB_Used,table.tableName,attrinfo[attrCount].attributeName);
		}
		else{
			attrinfo[attrCount].is_primary=false;
			attrinfo[attrCount].unique = instr.uniqueFlag[attrCount];
		}
	}
	catalog.create_table(table, attrinfo);
	record.Create(catalog.DB_Used,table.tableName,table.record_size);

	cout << "create table successfully" << endl;
}

void API::createDatabase() {
	catalog.create_database(instr.opObj);

	cout << "create database successfully" << endl;
}

int API::judgeType(string val) {
	const char* pchar = val.c_str();
	if (pchar[0] == '\'')
		return __string_type;
	else if (val == "null")
		return __null_type;
	else{
		int i;
		int countPoint = 0;
		for (i = 0; pchar[i] != 0; i++){
			if (!(pchar[i] <= '9' && pchar[i] >= '0' || pchar[i]=='.')){
				return __error_type;
			}
		}
		for (i = 0; pchar[i] != 0; i++) {
			if (pchar[i] == '.'){
				countPoint++;
			}
		}
		if (countPoint > 1){
			return __error_type;
		}
		if (countPoint == 1){
			return __float_type;
		}
		else{
			return __int_float_type;
		}
	}
}

void API::execFile(){
	string filepath = instr.opObj;
	FILE *fp;
	char StrLine[1024];
	string instrPiece = "";
	int lineNum = 0;
	int opId;

	if ((fp = fopen(filepath.c_str(), "r")) == NULL) //判断文件是否存在及可读
	{
		printf("Open file error!");
		return;
	}
	while (fgets(StrLine, 1024, fp)!=NULL)
	{
		//fgets(StrLine, 1024, fp);  //读取一行
		//if (feof(fp)) break;
		int i = 0;
		while (StrLine[i] != '\n'&&StrLine[i] != '\0'){
			//instrPiece += "";
			while (StrLine[i] != ';'&&StrLine[i] != '\n'&&StrLine[i] != '\0'){
				instrPiece += StrLine[i];
				i++;
			}

			if (StrLine[i] == ';'){
				instrPiece += ';';
				cout << instrPiece << endl;
				i++;
				//将interPiece送入Interpreter
				//执行语句，如果发生错误，输出错误的行号。
				strInstr = Interpreter(instrPiece);
				instrPiece = "";
				if (strInstr == "99"){
					break;
				}
				instr = SQL_Modify(strInstr);
				opId = atoi(instr.opId.c_str());
				try{
					switch (opId){
					case __create_table:
						createTable();
						break;
					case __create_database:
						createDatabase();
						break;
					case __create_index:
						createIndex();
						break;
					case __drop_table:
						dropTable();
						break;
					case __drop_database:
						dropDatabase();
						break;
					case __drop_index:
						dropIndex();
						break;
					case __insert:
						insert();
						break;
					case __delete_no_where:
						deleteNoWhere();
						break;
					case __delete_with_where:
						deleteWithWhere();
						break;
					case __select_no_where:
						selectNoWhere();
						break;
					case __select_with_where:
						selectWithWhere();
						break;
					case __use:
						useDatabase();
						break;
					case __execfile:
						execFile();
						break;
					case __quit:
						break;
					case __error:
						break;
					case __help:
						print_help();
						break;
					}
				}
				catch (int errorId){
					cout << "Line num: " << lineNum << " ";
					print_error(errorId);
					break;
				}
			}
		}
		lineNum++;
	}

	return;
}

void API::mergeIndex(int *p1, int *p2, int *out, int count1, int count2, int &outCount, char mode){
	if (count1 == 0){
		if (mode == 'o'){
			for (int i = 0; i < count2; i++)
				out[i] = p2[i];
			outCount = count2;
		}
		else if (mode == 'a'){
			outCount = 0;
		}
		return;
	}
	if (count2 == 0){
		if (mode == 'o'){
			for (int i = 0; i < count1; i++)
				out[i] = p1[i];
			outCount = count1;
		}
		else if (mode == 'a'){
			outCount = 0;
		}
		return;
	}
	sort(p1, p1 + count1);
	sort(p2, p2 + count2);
	int c1 = 0, c2 = 0, c3 = 0;
	outCount = 0;
	if (mode == 'o'){
		while (c1 != count1 || c2 != count2)
		{
			if (p1[c1] == p2[c2] && c1 != count1 && c2 != count2){
				out[c3] = p1[c1];
				c1++;
				c2++;
				c3++;
			}
			else if ((p1[c1] < p2[c2] && c1 != count1) || c2 == count2){
				out[c3] = p1[c1];
				c1++;
				c3++;
			}
			else{
				out[c3] = p2[c2];
				c2++;
				c3++;
			}
		}
		outCount = c3;
		return;
	}
	if (mode == 'a'){
		while (c1 != count1 || c2 != count2)
		{
			if (p1[c1] == p2[c2] && c1 != count1 && c2 != count2){
				out[c3] = p1[c1];
				c1++;
				c2++;
				c3++;
			}
			else if ((p1[c1] < p2[c2] && c1 != count1) || c2 == count2){
				c1++;
			}
			else{
				c2++;
			}
		}
		outCount = c3;
		return;
	}
}

string API::toString(string val){
	int length = val.length();
	const char* p1 = val.c_str();
	char* p2 = new char[length+1];
	strcpy(p2,p1);
	p2[length-1]=0;
	string ret(p2+1);
	delete[] p2;
	return ret;
}

float API::toFloat(string val){
	return atof(val.c_str());
}

int API::toInt(string val){
	return atoi(val.c_str());	
}

SQLlang API::SQL_Modify(string SQL)
{
    SQLlang sl;
    int start=0,end=0,length=0;
    int attrnum=0, endattr=0, startattr=0;
    int selenum=0, intand=0, intor=0;
	sl.attrWhereNum = 0;
	sl.attrSelectNum = 0;
    string midString,temp;
    sl.opId.assign(SQL,0,2);
    if((sl.opId=="00")||(sl.opId=="11")||(sl.opId=="10")||(sl.opId=="12")||(sl.opId=="50")||(sl.opId=="90")){
        sl.attrSelectNum=1;
        length = SQL.length();
        sl.opObj.assign(SQL,2,length-2);
    }
    //create table
    else if(sl.opId=="01"){
        SQL.insert(SQL.length()-1," ");
        start = 2;
        end = SQL.find(',',start);
        sl.opObj.assign(SQL,start,end-start);
        start=end+1;
        while((end=SQL.find(',',start))!=-1){
            //获取属性名
            startattr=start;
            endattr=SQL.find(' ',startattr);
            sl.attrWhere[sl.attrWhereNum].assign(SQL,startattr,endattr-startattr);
            //获取属性类型
            startattr=endattr+1;
            endattr=SQL.find(' ',startattr);
            midString.assign(SQL,startattr,endattr-startattr);
            if(midString!="#"){
                sl.attrType[sl.attrWhereNum] = midString;
                //是否为unique
                startattr=endattr+1;
                if(SQL[startattr]=='0') sl.uniqueFlag[sl.attrWhereNum] = false; else sl.uniqueFlag[sl.attrWhereNum] = true;
                sl.attrWhereNum+=1;
            }
            else{
                sl.primaryKey = sl.attrWhere[sl.attrWhereNum];
            }
            start=end+1;
        }
    }
    //create index
    else if(sl.opId=="02"){
        start = 2;
        end = SQL.find(' ',start);
        //index name
        sl.opObj.assign(SQL,start,end-start);
        start = end + 1;
        end = SQL.find(' ',start);
        //table name
        sl.attrSelect[0].assign(SQL,start,end-start);
        start = end + 1;
        //attribute name
        sl.attrSelect[1].assign(SQL,start,SQL.length()-start);
    }
    //insert
    else if(sl.opId=="40"){
        start = 2;
        end = SQL.find(' ',start);
        //table name
        sl.opObj.assign(SQL,start,end-start);
        start = end+2;
        while((end=SQL.find(',',start))!=-1){
            sl.val[sl.attrWhereNum].assign(SQL,start,end-start);
            sl.attrWhereNum+=1;
            start=end+1;
        }
    }
    //delete
    else if(sl.opId=="30"){
        start = 2;
        if((end=SQL.find('!',start))!=-1){
            //delete from
            sl.opObj.assign(SQL,start,end-1-start);
            start=end+1;
            intand = SQL.find(',',start);
            intor = SQL.find('?',start);
            if((intor!=-1)&&(intor<intand)) end = intor; else end = intand;
            while(end!=-1){
                if((intor!=-1)&&(intor<intand)) sl.isAnd[sl.attrWhereNum] = false; else sl.isAnd[sl.attrWhereNum] = true;
                startattr = start;
                endattr = SQL.find(' ',start);
                //attribute name
                sl.attrWhere[sl.attrWhereNum].assign(SQL,startattr,endattr-startattr);
                startattr=endattr+1;
                //op
                sl.op[sl.attrWhereNum] = SQL[startattr]-'0';
                startattr+=2;
                //comparing value
                sl.val[sl.attrWhereNum].assign(SQL,startattr,end-startattr);
                start=end+1;
                sl.attrWhereNum+=1;
                //find and or or
                intand = SQL.find(',',start);
                intor = SQL.find('?',start);
                if((intor!=-1)&&(intor<intand)) end = intor; else end = intand;
            }
        }
        else{
            //delete
            sl.opId = "31";
            sl.opObj.assign(SQL,start,SQL.length()-start);
        }
    }
    else if(sl.opId=="20"){
        start = 2;
        sl.attrSelectNum = 0;
        if((end=SQL.find('!',start))!=-1){
            //select from where
            end = SQL.find(' ',start);
            temp.assign(SQL,start,end-start);
            
            while((endattr=temp.find(',',startattr))!=-1){
                sl.attrSelect[sl.attrSelectNum].assign(temp,startattr,endattr-startattr);
                sl.attrSelectNum+=1;
                startattr=endattr+1;
            }
            start = end+1;
            end = SQL.find('!',start);
            sl.opObj.assign(SQL,start,end-start-1);
            start = end+1;
            
            intand = SQL.find(',',start);
            intor = SQL.find('?',start);
            if((intor!=-1)&&(intor<intand)) end = intor; else end = intand;
            while(end!=-1){
                if((intor!=-1)&&(intor<intand)) sl.isAnd[sl.attrWhereNum] = false; else sl.isAnd[sl.attrWhereNum] = true;
                startattr = start;
                endattr = SQL.find(' ',start);
                //attribute name
                sl.attrWhere[sl.attrWhereNum].assign(SQL,startattr,endattr-startattr);
                startattr=endattr+1;
                //op
                sl.op[sl.attrWhereNum] = SQL[startattr]-'0';
                startattr+=2;
                //comparing value
                sl.val[sl.attrWhereNum].assign(SQL,startattr,end-startattr);
                start=end+1;
                sl.attrWhereNum+=1;
                //find and or or
                intand = SQL.find(',',start);
                intor = SQL.find('?',start);
                if((intor!=-1)&&(intor<intand)) end = intor; else end = intand;
            }
            
        }
        else{
            //selece from
            sl.opId = "21";
            end = SQL.find(' ',start);
            temp.assign(SQL,start,end-start);
            
            while((endattr=temp.find(',',startattr))!=-1){
                sl.attrSelect[sl.attrSelectNum].assign(temp,startattr,endattr-startattr);
                sl.attrSelectNum+=1;
                startattr=endattr+1;
            }
            start = end+1;
            sl.opObj.assign(SQL,start,SQL.length()-start);
        }
    }
    
    /*
    cout<<attrnum<<endl;
    cout<<sl.opId<<endl;
    cout<<sl.opObj<<endl;
    for(int i=0;i<sl.attrSelectNum;i++)
        cout<<sl.attrSelect[i]<<endl;
    //cout<<sl.attrSelect[0]<<endl;
    //cout<<sl.attrSelect[1]<<endl;
    for(int i=0;i<attrnum;i++)
    {
        cout<<sl.attrWhere[i]<<" "<<sl.op[i]<<" "<<sl.isAnd[i]<<" "<<sl.attrType[i]<<" "<<sl.uniqueFlag[i]<<endl;
        cout<<sl.val[i]<<endl;
    }
    //cout<<sl.primaryKey<<endl;*/
    return sl;
}

