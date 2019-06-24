#define _CRT_SECURE_NO_WARNINGS

#define _DEBUG_MODE_OFF
#define DEBUG0(x)
#define DEBUG1(x, y)
#define DEBUG2(x, y1, y2)

#ifndef _DEBUG_MODE_OFF
#define DEBUG0(x) printf("%s", "(DEBUG) "); printf(x); printf("\n");
#define DEBUG1(x, y) printf("%s", "(DEBUG) "); printf(x, y); printf("\n");
#define DEBUG2(x, y1, y2) printf("%s", "(DEBUG) "); printf(x, y1, y2); printf("\n");
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "debug.h"
#include "list.h"
#include "hash.h"
#include "bitmap.h"

struct dataStructure {
	char type;  // list : 'l', hashtable : 'h', bitmap : 'b', none : 'n'
	char name[100]; // 해당 자료구조의 이름
	void *ptr;  // 해당 자료구조의 주소를 저장
};

struct dataStructure table[30];
struct list foo_list[10];
struct hash foo_hash[10];

struct foo_list_elem {
	int data;
	struct list_elem elem;
};

struct foo_hash_elem {
	int data;
	struct hash_elem elem;
};

#include "usrDefFunc.h"
#include "tableMethod.h"
#include "handleInst.h"

/*
    
*/
int main() {
        struct list l[10];
        char tmpstr[100];
        char tmpchar;
        int i;
        struct list* f;

        /* initialize */
        tableInit();
        srand(time(NULL));

        /* get instructions */
        while(1) {
			//DEBUG("table[0].name : %s", table[0].name);
			scanf("%s", tmpstr);
			DEBUG1("Instruction %s starts", tmpstr);
            if(strcmp(tmpstr, "create") == 0) {
				scanf("%s", tmpstr);
                tmpchar = tmpstr[0];
				scanf("%s", tmpstr);
                insertDS(tmpstr, tmpchar);
            }
            else if(strcmp(tmpstr, "delete") == 0) {
				scanf("%s", tmpstr);
                deleteDS(tmpstr);
            }
            else if(strcmp(tmpstr, "dumpdata") == 0) {
				scanf("%s", tmpstr);
				dumpdata(tmpstr);
            }
            else if(strcmp(tmpstr, "quit") == 0) {
                break;
            }
            else {
                switch(tmpstr[0]) {
                case 'l':
                    handleListInst(tmpstr);
                    break;
                case 'h':
					handleHashInst(tmpstr);
                    break;
                case 'b':
					handleBitmapInst(tmpstr);
                    break;
                }
            }
        }
        return 0;
}
