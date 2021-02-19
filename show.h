#ifndef _show_
#define _show_

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

typedef struct showContent{
	int attrNum;
	string attrName[32];
	int recordLength;
	string **val;
} showOut;

void output(showOut out);
void print_help();
int calculateMax(string s);
void print_error(int errorCode);

#endif /* APIshow_hpp */

/*
int main() {
showOut out;
char c[20];
out.attrNum = 4;

out.attrName[0] = "id";
out.attrName[1] = "name";
out.attrName[2] = "birth";
out.attrName[3] = "height";

out.recordLength = 3;

out.val = new string *[out.attrNum];
for(int i=0;i<out.attrNum;i++) out.val[i] = new string[out.attrNum];
out.val[0][0] = "3131333";
out.val[0][1] = "2134566";
out.val[0][2] = "111111";

out.val[1][0] = "175";
out.val[1][1] = "173.6";
out.val[1][2] = "174.3";
//sprintf(c, "%g",out.valFloat[0][1]);

out.val[2][0] = "ying";
out.val[2][1] = "shi";
out.val[2][2] = "chen";

out.val[3][0] = "0602";
out.val[3][1] = "0603";
out.val[3][2] = "0203";

output(out);

for(int i=0;i<out.attrNum;i++){
delete[] out.val[i];
}
delete[] out.val;

return 0;
}
*/

