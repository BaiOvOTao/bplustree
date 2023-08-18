#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplustree.h"
#include <time.h>
#include <stdbool.h>

#define true 1
#define false 0
#define MAX_KEY 1000000

char *buffer;
int fsize;
char input_file[100];
char output_file[100];
struct bplustreenode* root;

const int TotalRecords = 1000000;
int validRecords;
int keys[1000000], key_num;

int new_key, new_pos, new_len;
char new_st[12];

typedef struct Queue
{
	bplustreenode* node[MAX_KEY];
	int front;
	int rear;
}Queue;

Queue* initqueue(){
	struct Queue* q =(struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = 0;
	q->node[0]= NULL;
	return q;
}

bool QueueEmpty(Queue* q)
{
	if (q->rear == q->front)	//队空条件
		return true;
	else
		return false;
}

bool EnQueue(Queue* q, bplustreenode* x)
{
	if ((q->rear+1) % MAX_KEY == q->front)
		return false;		//队列满则报错

	q->node[q->rear] = x;		//将x插入队尾
	q->rear = (q->rear + 1) % MAX_KEY;    //队尾指针后移
	return true;
}

bool DeQueue(Queue* q, bplustreenode* x)
{
	if (q->rear == q->front)
		return false;	//队空则报错

	x = q->node[q->front];
	q->front = (q->front + 1) % MAX_KEY; //队头指针后移
	return true;
}

// 获取队头元素
bplustreenode* GetHead(Queue* q)
{
	if (q->rear == q->front)
		return false;	//队空则报错
	bplustreenode* x;
	x = q->node[q->front];
	return x;
}

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

void serialize(){
	int i=0;
	if(root==NULL){
		printf("the root is NULL,ERROR!\n");
		return ;
	}
	FILE* fp = fopen("serialize.txt","w+");
	bplustreenode* p;
	Queue* q;
	q=initqueue();
	EnQueue(q,root);
	while(!QueueEmpty(q))
	{
		p=GetHead(q);
		if(p->isleaf)
			fprintf(fp,"999999 ");
		fprintf(fp,"%d ",p->key_num);
		for(i=0;i<p->key_num;i++){
			fprintf(fp,"%d ",p->key[i]);
		}
		fprintf(fp,"\n");
		if(!p->isleaf)
			for(i=0;i<p->key_num;i++)
				EnQueue(q,p->child[i]);
		else{
			for(i=0;i<p->key_num;i++){
				fprintf(fp,"%s",(char*)p->child[i]);}
		}
		DeQueue(q,p);
	}
	fclose(fp);
	printf("sucess\n");
}

void deserialize(){
	FILE* fp = fopen("serialize.txt","r+");
	bplustreenode* p;
	bplustreenode* node;
	Queue* q;
	q=initqueue();
	int i,j,k;
	char a[20];
	root = new_bplustreenode();
	root->isroot =true;
	fscanf(fp,"%d",&i);
	root->key_num = i;
	for(i=0;i<root->key_num;i++){
		fscanf(fp,"%d",&root->key[i]);
		//printf("%d\n",root->key[i]);
		node=new_bplustreenode();
		node->father = root;
		root->child[i]=node;
		if(i>0)
		node->last = root->child[i-1];
		EnQueue(q,node);
	}
	for(i=0;i<root->key_num-1;i++){
		node = root->child[i];
		node->next = root->child[i+1];
	}
	while(!QueueEmpty(q)){
		p = GetHead(q);
		fscanf(fp,"%d",&i);
		if(i!=999999){ //非叶子结点
			p->key_num = i;
			for(i=0;i<p->key_num;i++){
				fscanf(fp,"%d",&p->key[i]);
				//printf("%d\n",p->key[i]);
				node=new_bplustreenode();
				node->father = p;
				p->child[i]=node;
				if(i>0)
				node->last = p->child[i-1];
				EnQueue(q,node);
			}
			for(i=0;i<p->key_num-1;i++){
				node = p->child[i];
				node->next = p->child[i+1];
			}

		}
		else{
			p->isleaf = true;
			fscanf(fp,"%d",&j);
			//printf("%d",j);
			p->key_num = j;
			for(j=0;j<p->key_num;j++){
				fscanf(fp,"%d",&p->key[j]);
				//printf("%d\n",p->key[j]);
				}
			for(j=0;j<p->key_num;j++){
				fscanf(fp,"%s",new_st);
				char* value = (char*)malloc(sizeof(char) * strlen(new_st));
				strcpy(value, new_st);
				p->child[j]=value;
				//printf("%s ",p->child[j]);
				}
		}
		DeQueue(q,p);
	}
fclose(fp);
}

void help(){
	printf("0 set the order,the begin order is 10\n");
	printf("1 build the bplustree\n");
	printf("2 find the key\n");
	printf("3 modify the key value\n");
	printf("4 delete key and key value\n");
	printf("5 insert new record\n");
	printf("6 print bplustree\n");
	printf("7 tree serialize\n");
	printf("8 deserialize\n");
	printf("9 quit\n");
}

int main(){
	char input_file[50];
	char output_file[50];
	double starttime,endtime,costtime;
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
				starttime = clock();
				Read_Data_And_Insert();
				endtime = clock();
				costtime = endtime -starttime;
				printf("Valid Records on B+tree is  %d\n", validRecords);
				printf("build success\n");
				printf("build b+tree cost %lf s\n",costtime/CLOCKS_PER_SEC);
				break;
			}
		case 2:{
				printf("input the key: ");
				int key;
				scanf("%d", &key);
				starttime = clock();
				bplustree_key(key);
				endtime = clock();
				costtime = endtime -starttime;
				printf("find key cost %lf s\n",costtime/CLOCKS_PER_SEC);
				break;
			}
		case 3:{
				printf("input key value: ");
				scanf("%d %s", &new_key, new_st);
				char* value = (char*)malloc(sizeof(char) * strlen(new_st));
				strcpy(value, new_st);
				starttime = clock();
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
				endtime = clock();
				costtime = endtime -starttime;
				printf("modify key cost %lf s\n",costtime/CLOCKS_PER_SEC);
				break;
			}
		case 4:{
				printf("input delete key: ");
				int key;
				scanf("%d", &key);
				starttime = clock();
				int pos = bplustree_find(key);
				if (pos != -1) { 
					File_Delete(pos);
					bplustree_delete(key);
					printf("Delete success.\n");
				} else {
					printf("failed, do not have this key on B+tree.\n");
				}
				endtime = clock();
				costtime = endtime -starttime;
				printf("delete key cost %lf s\n",costtime/CLOCKS_PER_SEC);
				break;
			}
		case 5:{
				printf("input insert key value: ");
				scanf("%d %s", &new_key, new_st);
				char* value = (char*)malloc(sizeof(char) * new_len);
				strcpy(value, new_st);
				starttime = clock();
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
				endtime = clock();
				costtime = endtime -starttime;
				printf("insert key cost %lf s\n",costtime/CLOCKS_PER_SEC);
				break;
			}
		case 6:{
			printf("now print the tree:\n");
			bplustree_print();
			break;
		}
		case 7:{
			serialize();
			printf("success serialize\n");
			break;
		}
		case 8:{
			deserialize();
			printf("success deserialize\n");
			break;
		}
		case 9:{
			starttime = clock();
			bplustree_destroy();
			endtime = clock();
			costtime = endtime -starttime;
			printf("insert key cost %lf s\n",costtime/CLOCKS_PER_SEC);
			printf("now quit\n");
			return 0;
		}
		default: break;
	}
	}
	/*Queue* q;
	q = initqueue();
	if(QueueEmpty(q))
	printf("empty");
	else printf("hmmnb");
	return 0;*/
}