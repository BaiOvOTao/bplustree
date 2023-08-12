#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplustree.h"

#define true 1
#define false 0
#define MAX_KEY 1000000

char *buffer;
int fsize;
char input_file[100];
char output_file[100];

const int TotalRecords = 1000000;
int validRecords;
int keys[1000000], key_num;

int new_key, new_pos, new_len;
char new_st[12];

void Read_Buffer(char *input_file) {
	FILE* fin = fopen(input_file, "rb");
	if (fin == NULL) {
		fputs("File Error\n", stderr);
		exit(1);
	}
	fseek(fin, 0, SEEK_END);
	fsize = ftell(fin);
	rewind(fin);
	buffer = (char*)malloc(sizeof(char) * fsize);
	if (buffer == NULL) {
		fputs("Memory Error\n", stderr);
		exit(2);
	}
	fread(buffer, 1, fsize, fin);
	fclose(fin);
}

void Read_Data_And_Insert() {
	int rid = 0;
	int cur = 0;
	while (1) {
		while (cur < fsize && !('0' <= buffer[cur] && buffer[cur] <= '9')) cur++; 
		if (cur == fsize) break;

		rid++;
		new_key = 0;
		new_pos = cur;
		while (buffer[cur] != ' ' && buffer[cur] != '\t') {
			new_key = new_key * 10 + (buffer[cur] - '0');
			cur++;
		}
		cur++;
		new_len = 0;
		while (buffer[cur] == ' ') cur++; 
		while (cur < fsize && buffer[cur] != '\n') {
			new_st[new_len++] = buffer[cur++];
		}
		new_st[new_len] = '\0';
		char* value = (char*)malloc(sizeof(char) * new_len);
		strcpy(value, new_st);
		keys[key_num++] = new_key;
		if (bplustree_insert(new_key, new_pos, value) == true) validRecords++;
	}
	free(buffer);
	buffer = NULL;
	//printf ("%d",validRecords);
}

int File_Modify(int pos, int key, char *value) {
	int old_key, old_len, len = strlen(value), i;
	char old_value[12];

	FILE* file = fopen("out.txt", "r+");
	fseek(file, pos, SEEK_SET);
	//printf("%d\n",pos);
	fscanf(file, "%d %s", &old_key, old_value);
	//9printf("%d %s\n",old_key,old_value);
	old_len = strlen(old_value);
	//printf("%d\n",old_len);
	if (len > old_len) return false;
	fseek(file, pos - ftell(file), SEEK_CUR);
	fprintf(file, "%d\t%s", key, value);
	for (i = len; i < old_len; i++) fprintf(file, " "); 
	fclose(file);
	return true;
}

void File_Delete(int pos) {
	int old_key, i;
	char old_value[12];

	FILE* file = fopen("out.txt", "r+");
	fseek(file, pos, SEEK_SET);
	fscanf(file, "%d %s", &old_key, old_value); 
   	int end_pos = ftell(file);
	//printf("%d %d\n", pos, end_pos);
	fseek(file, pos - end_pos, SEEK_CUR); 
	for (i = pos; i < end_pos; i++) fprintf(file, " "); 
	fclose(file);
}

int File_Insert(int new_key, char* new_st) {
	FILE* file = fopen("out.txt", "r+");
	fseek(file, 0, SEEK_END);
	int new_pos = ftell(file);
	fprintf(file, "%d %s\n", new_key, new_st);
	fclose(file);
	return new_pos;
}

void help(){
	printf("0 set the order,the begin order is 10\n");
	printf("1 build the bplustree\n");
	printf("2 find the key\n");
	printf("3 modify the key value\n");
	printf("4 delete key and key value\n");
	printf("5 insert new record\n");
	printf("6 print bplustree\n");
	printf("7 showhelp\n");
	printf("9 quit\n");
}

int main(){
	char input_file[50];
	char output_file[50];
	strcpy(input_file, "./out.txt");
	int built = false;
	Read_Buffer(input_file);
	bplustree_init();
	//bplustree_print();
	/*bplustree_delete(3);
	bplustree_print();
	bplustree_modify(54,"adf");
	bplustree_print();*/
	int a;
	help();
	while(1){
	scanf("%d",&a);
	switch(a){
		case 0: {
				printf("input Order: ");
				int order;
				scanf("%d", &order);
				bplustree_setmaxchild(order);
				break;
			}
		case 1:{
				// Build B+tree
				if (built == true) {
					printf("You have built the B+tree\n");
					break;
				}
				built = true;
				Read_Data_And_Insert();
				printf("Valid Records on B+tree is  %d\n", validRecords);
				printf("build success\n");
				break;
			}
		case 2:{
				printf("input the key: ");
				int key;
				scanf("%d", &key);
				bplustree_key(key);
				break;
			}
		case 3:{
				printf("input key value: ");
				scanf("%d %s", &new_key, new_st);
				char* value = (char*)malloc(sizeof(char) * strlen(new_st));
				strcpy(value, new_st);
				int pos = bplustree_find(new_key);
				//printf("%d\n",pos);
				if (pos != -1) { 
					if (File_Modify(pos, new_key, new_st)) { 
						bplustree_modify(new_key, value);
						printf("Modify success\n");
					}
				} else {
					printf("failed, do not have this key on B+tree.\n");
				}
				break;
			}
		case 4:{
				printf("input delete key: ");
				int key;
				scanf("%d", &key);
				int pos = bplustree_find(key);
				if (pos != -1) { 
					File_Delete(pos);
					bplustree_delete(key);
					printf("Delete success.\n");
				} else {
					printf("failed, do not have this key on B+tree.\n");
				}
				break;
			}
		case 5:{
				printf("input insert key value: ");
				scanf("%d %s", &new_key, new_st);
				char* value = (char*)malloc(sizeof(char) * new_len);
				strcpy(value, new_st);

				int pos = bplustree_find(new_key);
				if (pos == -1) {
					new_pos = File_Insert(new_key, new_st);
					keys[key_num++] = new_key;
					bplustree_insert(new_key, new_pos, value);
					validRecords++;
					printf("Insert success\n");
				} else {
					printf("failed, the key  exist\n");
				}
				break;
			}
		case 6:{
			bplustree_print();
			break;
		}
		case 7:{
			help();
			break;
		}
		case 9:{
			bplustree_destroy();
			printf("now quit\n");
			return 0;
		}
		default: break;
	}
	}
}
