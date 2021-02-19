//////////////////////////////////////////////////////////
///----------------------------------------------------///
///       Module: Interpreter_Module                   ///
///       Produced by: SZY                             ///
///       Description: Produced to deal with SQL parse ///
///       date: 2015/10/20                             ///
///----------------------------------------------------///
//////////////////////////////////////////////////////////

#include "Interpreter.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//读取用户输入
string read_input()
{
    string SQL;
    string temp;
    char str[100];
    bool finish=false;
    while(!finish)
    {
        cin>>str;
        temp=str;
        SQL=SQL+" "+temp;
        if(SQL[SQL.length()-1]==';')
        {
            SQL[SQL.length()-1]=' ';
            SQL+=";";
            finish=true;
        }
    }
    //将输入大写转化为小写
    for(int i=0;i<SQL.length();i++)
        if((SQL[i]>='A')&&(SQL[i]<='Z'))
            SQL[i]=SQL[i]-'A'+'a';
    //返回用户输入
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create database语句是否有效
string create_database(string SQL,int start)
{
    string temp;
    int index,end;
    //获取第三个单词
    while(SQL[start]==' ')
        start++;
    index=start;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"error: database name has not been specified!"<<endl;
        SQL="99";
    }
    else
    {
        while(SQL[start]==' ')
            start++;
        //若为非法信息，打印出错信息
        if(SQL[start]!=';'||start!=SQL.length()-1)
        {
            temp.assign(SQL,index,SQL.length()-index-2);
            cout<<"error12:"<<temp<<"---is not a valid database name!"<<endl;
            SQL="99";
        }
        //返回drop database语句的内部形式
        else
            SQL="00"+temp;
    }
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性
string get_attribute(string temp,string sql)
{
    int start=0,end,index;
    string T,C;
    temp+=" ";
    //获得属性名
    end=temp.find(' ',start);
    T.assign(temp,start,end-start);
    start=end+1;
    sql+=T+" ";
    //获得数据类型
    while(temp[start]==' ')
        start++;
    end=temp.find(' ',start);
    T.assign(temp,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(T.empty())
    {
        cout<<"error : error in create table statement!Name not found"<<endl;
        sql="99";
        return sql;
    }
    //若为int
    else if(T=="int")
        sql+="+";
    //若为float
    else if(T=="float")
        sql+="-";
    //其他
    else
    {
        index=T.find('(');
        C.assign(T,0,index);
        index++;
        //若有误，打印出错信息
        if(C.empty())
        {
            cout<<"error: "<<T<<"---is not a valid data type definition!"<<endl;
            sql="99";
            return sql;
        }
        //若为char
        else if(C=="char")
        {
            C.assign(T,index,T.length()-index-1);
            if(C.empty())
            {
                cout<<"error: the length of the data type char has not been specified!"<<endl;
                sql="99";
                return sql;
            }
            else
                sql+=C;
        }
        //若为非法信息，打印出错信息
        else
        {
            cout<<"error: "<<C<<"---is not a valid key word!"<<endl;
            sql="99";
            return sql;
        }
    }
    //是否有附加信息
    while(start<temp.length()&&temp[start]==' ')
        start++;
    if(start<temp.length())
    {
        //若为unique定义，保存信息
        end=temp.find(' ',start);
        T.assign(temp,start,end-start);
        if(T=="unique")
        {
            sql+=" 1,";
        }
        //若为非法信息，打印出错信息
        else
        {
            cout<<"error: "<<temp<<"---is not a valid key word!"<<endl;
            sql="99";
            return sql;
        }
    }
    //若无附加信息
    else
        sql+=" 0,";
    return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create table语句是否有效
string create_table(string SQL,int start)
{
    string temp,sql,T;
    int index,end,length;
    //获取表名
    while(SQL[start]==' ')
        start++;
    index=start;
    if((end=SQL.find('(',start))==-1)
    {
        cout<<"error: missing ( in the statement!"<<endl;
        SQL="99";
        return SQL;
    }
    temp.assign(SQL,start,end-start);
    start=end+1;
    //去除表名中多余的空格
    if(!temp.empty())
    {
        while(SQL[start]==' ')
            start++;
        length=temp.length()-1;
        while(temp[length]==' ')
            length--;
        temp.assign(temp,0,length+1);
    }
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"error: error in create table statement! table name not found"<<endl;
        SQL="99";
        return SQL;
    }
    //若为非法信息，打印出错信息
    else if(temp.find(' ')!=-1)
    {
        cout<<"error: "<<temp<<"---is not a valid table name! Please check if there are spaces in your table name"<<endl;
        SQL="99";
        return SQL;
    }
    else
    {
        //sql=tablename+","
        sql=temp+",";
        //获取每个属性
        while((end=SQL.find(',',start))!=-1)
        {
            temp.assign(SQL,start,end-start);
            start=end+1;
            //若有空属性，打印出错信息
            if(temp.empty())
            {
                cout<<"error: error in create table statement! Please check if there none colume in your table"<<endl;
                SQL="99";
                return SQL;
            }
            //保存属性
            else
            {
                sql=get_attribute(temp,sql);
                if(sql=="99")
                    return sql;
            }
            while(SQL[start]==' ')
                start++;
        }
        //获得最后属性
        temp.assign(SQL,start,SQL.length()-start-1);
        length=temp.length()-1;
        while(temp[length]!=')'&&length>=0)
            length--;
        //若无，打印出错信息
        if(length<1)
        {
            cout<<"error: error in create table statement! Expected: ) "<<endl;
            SQL="99";
            return SQL;
        }
        //存储属性
        else
        {
            temp.assign(temp,0,length);
            end=SQL.find(' ',start);
            T.assign(SQL,start,end-start);
            start=end+1;
            //若为主键定义
            if(T=="primary")
            {
                //判断是否有关键字key
                temp+=")";
                while(SQL[start]==' ')
                    start++;
                end=SQL.find('(',start);
                T.assign(SQL,start,end-start);
                start=end+1;
                length=T.length()-1;
                while(T[length]==' ')
                    length--;
                T.assign(T,0,length+1);
                //若为空，打印出错信息
                if(T.empty())
                {
                    cout<<"syntax error: syntax error in create table statement!"<<endl;
                    cout<<"\t missing key word key!"<<endl;
                    SQL="99";
                    return SQL;
                }
                //若有，继续验证
                else if(T=="key")
                {
                    //获取主键属性名
                    while(SQL[start]==' ')
                        start++;
                    end=SQL.find(')',start);
                    T.assign(SQL,start,end-start);
                    length=T.length()-1;
                    while(T[length]==' ')
                        length--;
                    T.assign(T,0,length+1);
                    //若无，打印出错信息
                    if(T.empty())
                    {
                        cout<<"error : missing primary key attribute name!"<<endl;
                        SQL="99";
                        return SQL;
                    }
                    //若为非法信息，打印出错信息
                    else if(T.find(' ')!=-1)
                    {
                        cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
                        SQL="99";
                        return SQL;
                    }
                    //保存主键
                    else
                    {
                        sql+=T+" #,";
                        SQL="01"+sql;
                    }
                }
                //若为非法信息，打印出错信息
                else
                {
                    cout<<"error : "<<T<<"---is not a valid key word!"<<endl;
                    SQL="99";
                    return SQL;
                }
            }
            //若为一般属性
            else
            {
                sql=get_attribute(temp,sql);
                if(sql=="99")
                {
                    SQL="99";
                    return SQL;
                }
                else
                    SQL="01"+sql;
            }
        }
    }
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index on语句是否有效
string create_index_on(string SQL,int start,string sql)
{
    string temp;
    int end,length;
    //获取表名
    while(SQL[start]==' ')
        start++;
    end=SQL.find('(',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"syntax error: syntax error for create index statement!"<<endl;
        cout<<"\t missing ( !"<<endl;
        SQL="99";
        return SQL;
    }
    else
    {
        //检验是否为有效文件名
        length=temp.length()-1;
        while(temp[length]==' ')
            length--;
        temp.assign(temp,0,length+1);
        //有效
        if(temp.find(' ')==-1)
        {
            sql+=" "+temp;
            //获取属性名
            while(SQL[start]==' ')
                start++;
            end=SQL.find(')',start);
            temp.assign(SQL,start,end-start);
            start=end+1;
            //若无，打印出错信息
            if(temp.empty())
            {
                cout<<"syntax error: syntax error for create index statement!"<<endl;
                cout<<"\t missing ) !"<<endl;
                SQL="99";
                return SQL;
            }
            else
            {
                //检验是否为有效属性名
                length=temp.length()-1;
                while(temp[length]==' ')
                    length--;
                temp.assign(temp,0,length+1);
                //有效
                if(temp.find(' ')==-1)
                {
                    sql+=" "+temp;
                    while(SQL[start]==' ')
                        start++;
                    if(SQL[start]!=';'||start!=SQL.length()-1)
                    {
                        cout<<"syntax error: syntax error for quit!"<<endl;
                        SQL="99";
                        return SQL;
                    }
                    //返回create index语句的内部形式
                    else
                        SQL="02"+sql;
                }
                //无效,打印出错信息
                else
                {
                    cout<<"error:"<<" "<<temp<<"---is not a valid attribute name!"<<endl;
                    SQL="99";
                    return SQL;
                }
            }
        }
        //无效,打印出错信息
        else
        {
            cout<<"error:"<<" "<<temp<<"---is not a valid table name!"<<endl;
            SQL="99";
            return SQL;
        }
    }
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index语句是否有效
string create_index(string SQL,int start)
{
    string temp,sql;
    int end;
    //获取第三个单词
    while(SQL[start]==' ')
        start++;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"syntax error: syntax error for create index statement!"<<endl;
        SQL="99";
    }
    else
    {
        sql=temp;
        //获取第四个单词
        while(SQL[start]==' ')
            start++;
        end=SQL.find(' ',start);
        temp.assign(SQL,start,end-start);
        start=end+1;
        //若无，打印出错信息
        if(temp.empty())
        {
            cout<<"syntax error: syntax error for create index statement!"<<endl;
            SQL="99";
        }
        //若为on,继续验证
        else if(temp=="on")
            SQL=create_index_on(SQL,start,sql);
        //若为非法信息，打印非法信息
        else
        {
            cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
            SQL="99";
        }
    }
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
string create_clause(string SQL,int start)
{
    string temp;
    int end;
    //获取第二个单词
    while(SQL[start]==' ')
        start++;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"syntax error: syntax error for create statement!"<<endl;
        SQL="99";
    }
    //若为database,继续验证
    else if(temp=="database")
        SQL=create_database(SQL,start);
    //若为table,继续验证
    else if(temp=="table")
        SQL=create_table(SQL,start);
    //若为index,继续验证
    else if(temp=="index")
        SQL=create_index(SQL,start);
    //若为错误信息，打印出错信息
    else
    {
        cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
        SQL="99";
    }
    //返回create语句的内部形式
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
string drop_database(string SQL,int start)
{
    string temp;
    int index,end;
    //获取第三个单词
    while(SQL[start]==' ')
        start++;
    index=start;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"error: database name has not been specified!"<<endl;
        SQL="99";
    }
    else
    {
        while(SQL[start]==' ')
            start++;
        //若为非法信息，打印出错信息
        if(SQL[start]!=';'||start!=SQL.length()-1)
        {
            temp.assign(SQL,index,SQL.length()-index-2);
            cout<<"error:"<<temp<<"---is not a valid database name!"<<endl;
            SQL="99";
        }
        //返回drop database语句的内部形式
        else
            SQL="10"+temp;
    }
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
string drop_table(string SQL,int start)
{
    string temp;
    int index,end;
    //获取第三个单词
    while(SQL[start]==' ')
        start++;
    index=start;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"error: table name has not been specified!"<<endl;
        SQL="99";
    }
    else
    {
        while(SQL[start]==' ')
            start++;
        if(SQL[start]!=';'||start!=SQL.length()-1)
        {
            temp.assign(SQL,index,SQL.length()-index-2);
            cout<<"error:"<<temp<<"---is not a valid table name!"<<endl;
            SQL="99";
        }
        else
            SQL="11"+temp;
    }	
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
string drop_index(string SQL,int start)
{
    string temp;
    int index,end;
    //获取第三个单词
    while(SQL[start]==' ')
        start++;
    index=start;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无，打印出错信息
    if(temp.empty())
    {
        cout<<"error: index name has not been specified!"<<endl;
        SQL="99";
    }
    else
    {
        while(SQL[start]==' ')
            start++;
        if(SQL[start]!=';'||start!=SQL.length()-1)
        {
            temp.assign(SQL,index,SQL.length()-index-2);
            cout<<"error:"<<temp<<"---is not a valid index name!"<<endl;
            SQL="99";
        }
        else
            SQL="12"+temp;
    }	
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop语句是否有效
string drop_clause(string SQL,int start)
{
    string temp;
    int end;
    while(SQL[start]==' ')
        start++;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    if(temp.empty())
    {
        cout<<"syntax error: syntax error for drop statement!"<<endl;
        SQL="99";
    }
    else if(temp=="database")
        SQL=drop_database(SQL,start);
    else if(temp=="table")
        SQL=drop_table(SQL,start);
    else if(temp=="index")
        SQL=drop_index(SQL,start);
    else
    {
        cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
        SQL="99";
    }
    return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//验证select是否有效
string select_clause(string SQL,int start)
{
    string temp,sql,T,op1,op2,flag;
    int end,startcol,endcol,op,orsl,andsl,sllength;
    int length;
    while(SQL[start]==' ')
        start++;
    startcol=start;
    end=SQL.find("from",start);
    start=end;
    //判断from是否存在
    if(end!=-1){
        temp.assign(SQL,startcol,end-startcol);
        length=temp.length()-1;
        while((length>=0)&&(temp[length]==' ')) length--;
        temp.assign(temp,0,length+1);
        sql="20";
        if(temp=="*") sql+="*,";
        else{
          temp+=",";
          startcol=0;
          //分词
          while((end=temp.find(",",startcol))!=-1)
          {
			  endcol = end - 1;
			  while (temp[startcol] == ' ') startcol++;
			  while ((temp[endcol] == ' ')&&(endcol>=0)) endcol--;
			  endcol += 1;
			  if (startcol<endcol)  
                T.assign(temp,startcol,endcol-startcol);
			  else{
				  cout << "Syntax Error: check if there is a null colume" << endl;
				  SQL = "99";
				  return SQL;
			  }
			  cout << T << endl;
              startcol=end+1;
              if(T.find(" ")==-1)
              {
                  sql+=T;
                  sql+=",";
              }
              else
              {
                  cout<<"Syntax Error: check if there are spaces in col"<<endl;
                  SQL="99";
                  return SQL;
              }
          }
        }
        sql+=" ";
        start+=4;
        //获取表名
        while(SQL[start]==' ') start++;
        end=SQL.find(" ",start);
        temp.assign(SQL,start,end-start);
        start=end+1;
        if(temp.find(" ")==-1) sql+=temp;
        else{
            cout<<"Syntax Error: check if there are spaces in table name"<<endl;
            SQL="99";
            return SQL;
        }
        //检验是否有where
        while(SQL[start]==' ') start++;
        if(SQL[start]!=';')
        {
            end=SQL.find(" ",start);
            temp.assign(SQL,start,end-start);
            start=end+1;
            if(temp!="where"){
                cout<<"Systax Error: check if you want to use where"<<endl;
                SQL="99";
                return SQL;
            }
            else{
                sql+=" !";
                SQL.insert(SQL.length()-2, " and");
                //对每个and进行检测
                andsl = SQL.find("and",start);
                orsl = SQL.find("or",start);
                if((orsl!=-1)&&(andsl>orsl)) {
                    end = orsl; sllength = 2;}
                else {end = andsl; sllength = 3;}
                while(end!=-1)
                {
                    while(SQL[start]==' ') start++;
                    //cout<<start<<" "<<end<<endl;
                    temp.assign(SQL,start,end-start);
                    startcol=0;
                    start=end+3;
                    length=temp.length()-1;
					while ((length>0) && (temp[length] == ' ')) length--;
                    if(length==0){
                        cout<<"Systax Error: check your conditions"<<endl;
                        SQL="99";
                        return SQL;
                    } 
                    else{
                        end=length;
                        //cout<<end<<endl;
                        temp.assign(temp,0,length+1);
                        flag="0";
                        if((op=temp.find("<>",0))!=-1) flag="1";
                        else if((op=temp.find("<=",0))!=-1) flag="2";
                        else if((op=temp.find(">=",0))!=-1) flag="3";
                        else if((op=temp.find("<",0))!=-1) flag="4";
                        else if((op=temp.find(">",0))!=-1) flag="5";
                        else if((op=temp.find("=",0))!=-1) flag="6";
                        if(flag=="0") {cout<<"Systax Error: lack of operator"<<endl; SQL="99";}
                        else {
                            //cout<<temp<<endl;
                            //cout<<startcol<<" "<<op<<endl;
                            op1.assign(temp,startcol,op-startcol);
                            length=op1.length()-1;
                            while((op1[length]==' ')&&(length>=0)) length--;
                            length++;
                            if(length!=0){
                                op1.assign(op1,0,length);
                                if(op1.find(" ")!=-1){
                                    cout<<"Systax Error: blanks in colume"<<endl;
                                    SQL="99";
                                    return SQL;
                                }
                                sql+=op1; sql+=" "; sql+=flag; sql+=" ";
                                startcol=op;
                                if((flag=="1")||(flag=="2")||(flag=="3")) startcol+=2; else startcol+=1;
                                while(temp[startcol]==' ') startcol++;
                                op2.assign(temp,startcol,end-startcol+1);
                                if(((op2[0]==39)&&(op2[op2.length()-1]!=39))||((op2[0]!=39)&&(op2[op2.length()-1]==39)))
                                {
                                    cout<<"Systax Error: systax error for selection02"<<endl;
                                    SQL="99";
                                    return SQL;
                                }
                                if((op2[0]==39)&&(op2[op2.length()-1]==39)) sql+=op2;
                                else if(op2.find(" ")==-1) sql+=op2;
                                else{
                                    cout<<"Systax Error: systan error for selection03"<<endl;
                                    SQL="99";
                                    return SQL;
                                }
                                if(sllength==2) sql+='?'; else sql+=',';
                            }
                            else{
                                cout<<"Systax Error: systax error for selection"<<endl;
                                SQL="99";
                                return SQL;
                            }
                            }
                        }
                    andsl = SQL.find("and",start);
                    orsl = SQL.find("or",start);
                    if((orsl!=-1)&&(andsl>orsl)) {
                        end = orsl; sllength = 2;}
                    else {end = andsl; sllength = 3;}
                }
                
            }
        }
    }
    else
    {
        cout<<"Syntax Error: from not found"<<endl;
        SQL="99";
    }
    SQL=sql;
    return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证delete是否有效
string delete_clause(string SQL,int start)
{
    string sql,temp,op1,op2,flag;
    int end,length,startcol,op,andsl,orsl,sllength;
    //检查是否有from
    while(SQL[start]==' ') start++;
    end=SQL.find(" ",start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    if(temp!="from"){
        cout<<"Systax Error: Expected 'from'"<<endl;
        SQL="99";
        return SQL;
    }
    //检查表名
    while(SQL[start]==' ') start++;
    end=SQL.find(" ",start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    if(temp.find(" ")!=-1){
        cout<<"Systax Error: check if there are spaces in colume"<<endl;
        SQL="99";
        return SQL;
    }
    sql="30"+temp;
    while(SQL[start]==' ') start++;
    if(SQL[start]==';') {SQL=sql;return SQL;}
    else {
        end=SQL.find(" ",start);
        temp.assign(SQL,start,end-start);
        start=end+1;
        if(temp!="where"){
            cout<<"Systax Error: check if you want to use where"<<endl;
            SQL="99";
            return SQL;
        }
        else{
            sql+=" !";
            SQL.insert(SQL.length()-2, " and");
            //对每个and进行检测
            andsl = SQL.find("and",start);
            orsl = SQL.find("or",start);
            if((orsl!=-1)&&(andsl>orsl)) {
                end = orsl; sllength = 2;}
            else {end = andsl; sllength = 3;}
            while(end!=-1)
            {
                while(SQL[start]==' ') start++;
                //cout<<start<<" "<<end<<endl;
                temp.assign(SQL,start,end-start);
                startcol=0;
                start=end+sllength;
                length=temp.length()-1;
				while ((length>0)&&(temp[length] == ' ')) length--;
                if(length==0){
                    cout<<"Systax Error: check your conditions"<<endl;
                    SQL="99";
                    return SQL;
                }
                else{
                    end=length;
                    //cout<<end<<endl;
                    temp.assign(temp,0,length+1);
                    flag="0";
                    if((op=temp.find("<>",0))!=-1) flag="1";
                    else if((op=temp.find("<=",0))!=-1) flag="2";
                    else if((op=temp.find(">=",0))!=-1) flag="3";
                    else if((op=temp.find("<",0))!=-1) flag="4";
                    else if((op=temp.find(">",0))!=-1) flag="5";
                    else if((op=temp.find("=",0))!=-1) flag="6";
                    if(flag=="0") {cout<<"Systax Error: lack of operator"<<endl; SQL="99";}
                    else {
                        //cout<<temp<<endl;
                        //cout<<startcol<<" "<<op<<endl;
                        op1.assign(temp,startcol,op-startcol);
                        length=op1.length()-1;
                        while((op1[length]==' ')&&(length>=0)) length--;
                        length++;
                        if(length!=0){
                            op1.assign(op1,0,length);
                            if(op1.find(" ")!=-1){
                                cout<<"Systax Error: blanks in colume"<<endl;
                                SQL="99";
                                return SQL;
                            }
                            sql+=op1; sql+=" "; sql+=flag; sql+=" ";
                            startcol=op;
                            if((flag=="1")||(flag=="2")||(flag=="3")) startcol+=2; else startcol+=1;
                            while(temp[startcol]==' ') startcol++;
                            op2.assign(temp,startcol,end-startcol+1);
                            if(((op2[0]==39)&&(op2[op2.length()-1]!=39))||((op2[0]!=39)&&(op2[op2.length()-1]==39)))
                            {
                                cout<<"Systax Error: systax error for selection02"<<endl;
                                SQL="99";
                                return SQL;
                            }
                            if((op2[0]==39)&&(op2[op2.length()-1]==39)) sql+=op2;
                            else if(op2.find(" ")==-1) sql+=op2;
                            else{
                                cout<<"Systax Error: systan error for selection03"<<endl;
                                SQL="99";
                                return SQL;
                            }
                            if(sllength==2) sql+='?'; else sql+=',';
                        }
                        else{
                            cout<<"Systax Error: systax error for selection"<<endl;
                            SQL="99";
                            return SQL;
                        }
                    }
                }
            andsl = SQL.find("and",start);
            orsl = SQL.find("or",start);
            if((orsl!=-1)&&(andsl>orsl)) {
                end = orsl; sllength = 2;}
            else {end = andsl; sllength = 3;}
            }
        }
        }
    SQL=sql;
    return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert是否有效
string insert_clause(string SQL, int start)
{
    string temp,sql;
    int end,length;
    //检查是否有into
    while(SQL[start]==' ') start++;
    end=SQL.find(" ",start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    if(temp!="into"){
        cout<<"Systax Error: Expected 'into'"<<endl;
        SQL="99";
        return SQL;
    }
    //检查表名
    while(SQL[start]==' ') start++;
    end=SQL.find(" ",start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    if(temp.find(" ")!=-1){
        cout<<"Systax Error: check if there are spaces in table name"<<endl;
        SQL="99";
        return SQL;
    }
    sql="40"+temp+" !";
    //检查是否有values
    while(SQL[start]==' ') start++;
    if(SQL.substr(start,6)!="values"){
        cout<<"Systax Error: Expected 'values'"<<endl;
        SQL="99";
        return SQL;
    }
    start+=6;
    //检查是否有左括号
    while(SQL[start]==' ') start++;
    if(SQL[start]!='('){
        cout<<"Systax Error: Expected '('"<<endl;
        SQL="99";
        return SQL;
    }
    start++;
    //检查是否有右括号
    end = SQL.length()-2;
    while((SQL[end]==' ')&&(end>start)) end--;
    if(SQL[end]!=')'){
        cout<<"Systax Error: Expected ')'"<<endl;
        SQL="99";
        return SQL;
    }
    SQL.insert(end, ",");
    //加入每个值
    while((end=SQL.find(",",start))!=-1){
        while(SQL[start]==' ') start++;
        temp.assign(SQL,start,end-start);
        start=end+1;
        length=temp.length()-1;
        while((temp[length]==' ')&&(length>=0)) length--;
        if(length<0){
            cout<<"Systax Error: Do you mean None?"<<endl;
            SQL="99";
            return SQL;
        }
        temp.assign(temp,0,length+1);
        if(((temp[0]==39)&&(temp[temp.length()-1]!=39))||((temp[0]!=39)&&(temp[temp.length()-1]==39)))
        {
            cout<<"Systax Error: systax error for insert01"<<endl;
            SQL="99";
            return SQL;
        }
        if((temp[0]==39)&&(temp[temp.length()-1]==39)) sql+=temp;
        else if(temp.find(" ")==-1) sql+=temp;
        else{
            cout<<"Systax Error: systan error for insert02"<<endl;
            SQL="99";
            return SQL;
        }
        sql+=",";
    }
    SQL=sql;
    return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//检验use是否有效
string use_clause(string SQL, int start)
{
    string temp,sql;
    int end;
    while(SQL[start]==' ') start++;
    end=SQL.find(" ",start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    if(temp.find(" ")!=-1){
        cout<<"Systax Error: check if there are spaces in database name"<<endl;
        SQL="99";
        return SQL;
    }
    while(SQL[start]==' ') start++;
    if(SQL[start]!=';'){
        cout<<"Systax Error: Expected ';'"<<endl;
        SQL="99";
        return SQL;
    }
    SQL="50"+temp;
    return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//执行quit语句
string quit_clause(string SQL, int start)
{
    while(SQL[start]==' ') start++;
    if(SQL[start]!=';'){
        cout<<"Systax Error: Expected ';'"<<endl;
        SQL="99";
        return SQL;
    }
    SQL="80";
    return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//检验execfile是否有效
string execfile_clause(string SQL, int start)
{
    int end,length;
    string temp;
    while(SQL[start]==' ') start++;
    end=SQL.find(" ",start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    while(SQL[start]==' ') start++;
    if(SQL[start]!=';'){
        cout<<"Systax Error: Blanks in file name"<<endl;
        SQL="99";
        return SQL;
    }
    SQL="90"+temp;
    return SQL;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter(string statement)
{
    string SQL;
    string temp;
    string sql;
    int start=0,end;
    if(statement.empty())
        //获取用户输入
        SQL=read_input();
	else{
		SQL = statement;
		SQL[SQL.length() - 1] = ' ';
		SQL += ";";
		for (int i = 0; i<SQL.length(); i++)
			if ((SQL[i] >= 'A') && (SQL[i] <= 'Z'))
				SQL[i] = SQL[i] - 'A' + 'a';
	}
    //获取输入的第一个单词
    while(SQL[start]==' ')
        start++;
    end=SQL.find(' ',start);
    temp.assign(SQL,start,end-start);
    start=end+1;
    //若无输入，打印出错信息
    if(temp.empty())
    {
        cout<<"syntax error: empty statement!"<<endl;
        SQL="99";
    }
    //若为create语句
    else if(temp=="create")
        SQL=create_clause(SQL,start);
    //若为drop语句
    else if(temp=="drop")
        SQL=drop_clause(SQL,start);
    //若为select语句
    else if(temp=="select")
        SQL=select_clause(SQL,start);
    //若为delete语句
    else if(temp=="delete")
        SQL=delete_clause(SQL,start);
    //若为insert语句
    else if(temp=="insert")
        SQL=insert_clause(SQL,start);
    //若为use语句
    else if(temp=="use")
        SQL=use_clause(SQL,start);
    //若为execfile语句
    else if(temp=="execfile")
        SQL=execfile_clause(SQL,start);
    //若为quit语句
    else if(temp=="quit")
        SQL=quit_clause(SQL,start);
    //获取帮助
    else if(temp=="help")
        SQL="70";
    //若为非法语句
    else
    {
        cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
        SQL="99";
    }
    //返回输入语句的内部形式
    return SQL;
}

