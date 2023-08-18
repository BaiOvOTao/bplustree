#include "bplustree.h"
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

struct bplustreenode* root;

int maxchild = 10;
int totalnodes;
int queryansnum;

bplustreenode* new_bplustreenode(){
    struct bplustreenode* p =(struct bplustreenode*)malloc(sizeof(struct bplustreenode));
    p->isroot = false;
    p->isleaf = false;
    p->key_num = 0;
    p->child[0] = NULL;
    p->father = NULL;
    p->next = NULL;
    p->last = NULL;
    totalnodes++;
    return p;
}

int binary_search(bplustreenode* cur,int key){
    int l = 0,r = cur->key_num;
    if(key<cur->key[l]) return l;
    if(cur->key[r-1]<=key) return r-1;
    while(l<r-1){
        int mid = (l+r)>>1;
        if(cur->key[mid]>key)
            r=mid;
        else
            l=mid;
    }
    return l;
}

void insert(bplustreenode*,int,int,void*);
void spilt(bplustreenode* cur){
    bplustreenode* temp = new_bplustreenode();
    bplustreenode* ch;
    int mid = maxchild>>1;
    temp->isleaf = cur->isleaf;
    temp->key_num = maxchild-mid;
    int i;
    for(i=mid;i<maxchild;i++){
        temp->child[i-mid] = cur->child[i];
        temp->key[i-mid] = cur->key[i];
        if(temp->isleaf){
            temp->pos[i-mid] = cur->pos[i];
        }else{
            ch = (bplustreenode*)temp->child[i-mid];
            ch->father = temp;
        }
    }
    cur ->key_num = mid;
    if(cur->isroot){
        root = new_bplustreenode();
        root->key_num = 2;
        root->isroot = true;
        root->key[0]=cur->key[0];
        root->child[0] = cur;
        root->key[1] = temp->key[0];
        root->child[1] = temp;
        cur->father = temp->father = root;
        cur->isroot = false;
        if(cur->isleaf){
            cur->next=temp;
            temp->last = cur;
        }
    }else{
        temp->father = cur->father;
        insert(cur->father,cur->key[mid],-1,(void*)temp);
    }

}

void insert(bplustreenode* cur,int key,int pos,void* value){
    int i,ins;
    if(key<cur->key[0]) ins = 0;else ins = binary_search(cur,key)+1;
    for(i=cur->key_num;i>ins;i--){
        cur->key[i]=cur->key[i-1];
        cur->child[i]=cur->child[i-1];
        if (cur->isleaf)cur->pos[i]=cur->pos[i-1];
    }
    cur->key_num++;
    cur->key[ins] = key;
    cur->child[ins] = value;
    cur->pos[ins] = pos;
    if(cur->isleaf == false){
        bplustreenode* firstchild = (bplustreenode*)(cur->child[0]);
        if(firstchild->isleaf==true){
            bplustreenode* temp = (bplustreenode*)(value);
            if(ins>0){
                bplustreenode* prevchild;
                bplustreenode* succchild;
                prevchild = (bplustreenode*)cur->child[ins-1];
                succchild = prevchild->next;
                prevchild->next = temp;
                temp->next = succchild;
                temp->last = prevchild;
                if(succchild !=NULL) succchild->last = temp;
            }else{
                temp->next = cur->child[1];
                printf("this happens\n");
            }
        }
    }
    if(cur->key_num == maxchild)
        spilt(cur);
}

void resort(bplustreenode* left, bplustreenode* right){
    int total = left->key_num + right->key_num;
    bplustreenode* temp;
    if(left->key_num<right->key_num){
        int leftsize = total>>1;
        int i=0,j=0;
        while(left->key_num<leftsize){
            left->key[left->key_num]=right->key[i];
            left->child[left->key_num] = right->child[i];
            if(left->isleaf){
                left->pos[left->key_num] = right->pos[i];
            }else{
                temp = (bplustreenode*)(right->child[i]);
                temp->father = left;
            }
            left->key_num++;
            i++;
        }
        while (i<right->key_num){
            right->key[j] = right->key[i];
            right->child[j] = right->child[i];
            if (right->isleaf) right->pos[j] = right->pos[i];
            i++;
            j++;
        }
        right->key_num = j;
    }else{
        int leftsize = total>>1;
        int i,move = left->key_num - leftsize, j = 0;
        for (i= right->key_num-1;i>=0;i--){
            right->key[i+move] = right->key[i];
            right->child[i+move] = right->child[i];
            if(right->isleaf) right->pos[i+move] = right->pos[i];
        }
        for (i = leftsize; i<left->key_num;i++){
            right->key[j]= left->key[i];
            right->child[j] = left->child[i];
            if(right->isleaf){
                right->pos[j] = left->pos[i];
            }else{
                temp = (bplustreenode*)left->child[i];
                temp->father = right;
            }
            j++;
        }
        left->key_num = leftsize;
        right->key_num = total - leftsize;
    }
}

void delete(bplustreenode*,int);
void redistribute(bplustreenode* cur){
    if(cur->isroot){
        if(cur->key_num == 1 && !cur->isleaf){
            root = cur->child[0];
            root->isroot = true;
            free(cur);
        }
        return;
    }
    bplustreenode* father = cur->father;
    bplustreenode* prevchild;
    bplustreenode* succchild;
    bplustreenode* temp;
    int my_index = binary_search(father,cur->key[0]);
    if(my_index + 1<father->key_num){
        succchild = father->child[my_index+1];
        if((succchild->key_num-1)*2>=maxchild){
            resort(cur,succchild);
            father->key[my_index+1] = succchild->key[0];
            return;
        }
    }
    if(my_index -1>=0){
        prevchild = father->child[my_index - 1];
        if((prevchild->key_num-1)*2>=maxchild){
            resort(prevchild,cur);
            father->key[my_index] = cur ->key[0];
            return;
        }
    }
    if(my_index + 1 < father->key_num ){
        int i = 0;
        while(i<succchild->key_num){
            cur->key[cur->key_num]= succchild->key[i];
            cur->child[cur->key_num] = succchild ->child[i];
            if(cur->isleaf){
                cur->pos[cur->key_num] = succchild->pos[i];
            }else{
                temp = (bplustreenode*)(succchild->child[i]);
                temp->father = cur;
            }
            cur->key_num++;
            i++;
        }
        delete(father,succchild->key[0]);
        return;
    }
    if(my_index-1>=0){
        int i = 0;
        while(i< cur->key_num){
            prevchild->key[prevchild->key_num] = cur->key[i];
            prevchild->child[prevchild->key_num] = cur->child[i];
            if(cur->isleaf){
                prevchild->pos[prevchild->key_num] = cur->pos[i];
            }else{
                temp = (bplustreenode*)(cur->child[i]);
                temp->father = prevchild;
            }
            prevchild->key_num++;
            i++;
        }
        delete(father,cur->key[0]);
        return;
    }
    //printf("337845818\n");
}

void delete(bplustreenode* cur,int key){
    int i,del = binary_search(cur,key);
    void* delchild = cur->child[del];
    for (i = del;i<cur->key_num-1;i++){
        cur->key[i] = cur->key[i+1];
        cur->child[i] = cur->child[i+1];
        if(cur->isleaf)cur->pos[i] = cur->pos[i+1];
    }
    cur->key_num--;
    if(cur->isleaf == false){
        bplustreenode* firstchild = (bplustreenode*)(cur->child[0]);
        if(firstchild->isleaf == true){
            bplustreenode* temp = (bplustreenode*)delchild;
            bplustreenode* prevchild = temp->last;
            bplustreenode* succchild = temp->next;
            if(prevchild!=NULL)prevchild->next = succchild;
            if(succchild!=NULL)succchild->last = prevchild;
        }
    }
    if(del == 0 && !cur->isroot){
        bplustreenode* temp = cur;
        while(!temp->isroot && temp == temp->father->child[0]){
            temp->father->key[0] = cur->key[0];
            temp = temp->father;
        }
        if(!temp->isroot){
            temp = temp->father;
            int i = binary_search(temp,key);
            temp->key[i] = cur-> key[0];
        }
    }
    free(delchild);
    if(cur->key_num*2<maxchild)
        redistribute(cur);
}

bplustreenode* find(int key,int modify){
    bplustreenode* cur = root;
    while(1){
        if(cur->isleaf==true)
            break;
        if(key<cur->key[0]){
            if(modify == true)cur->key[0] = key;
            cur = cur->child[0];
        }else{
            int i = binary_search(cur,key);
            cur = cur->child[i];
        }
    }
    return cur;
}

void destroy(bplustreenode* cur){
    if(cur->isleaf == true){
        int i;
        for(i = 0;i<cur->key_num;i++)
            free(cur->child[i]);
    }else{
        int i;
        for(i = 0;i<cur->key_num;i++)
            destroy(cur->child[i]);
    }
    free(cur);
}

void print(bplustreenode* cur){
    int i;
    for(i = 0;i<cur->key_num;i++)
        printf("%d ",cur->key[i]);
    printf("\n");
    if(!cur->isleaf){
        for(i = 0;i<cur->key_num;i++)
            print(cur->child[i]);
    }
}

int bplustree_insert(int key,int pos,void* value){
    bplustreenode* leaf = find(key,true);
    int i = binary_search(leaf,key);
    if(leaf->key[i]==key) return false;
    insert(leaf,key,pos,value);
    return true;
}

void bplustree_key(int key){
    bplustreenode* leaf = find(key,false);
    int i;
    for(i=0;i<leaf->key_num;i++){
        //printf("%d ",leaf->key[i]);
        if(leaf->key[i] == key){
        printf("key = %d,value = %s\n",leaf->key[i],(char*)leaf->child[i]);
        break;
        }
        if(leaf->key[i] != key&&i==leaf->key_num-1)
        printf("the key don't exist\n");
    }
}


int bplustree_find(int key){
    bplustreenode* leaf = find(key,false);
    int i = binary_search(leaf,key);
    if (leaf->key[i]!=key) return -1;
    return leaf ->pos[i];
}

void bplustree_modify(int key,void* value){
    bplustreenode* leaf =find(key,false);
    int i = binary_search(leaf,key);
    if(leaf->key[i]!=key)return;
    printf("new value =%s\n",(char*)(value));
    free(leaf->child[i]);
    leaf->child[i]= value;
}

void bplustree_delete(int key){
    bplustreenode* leaf =find(key,false);
    int i = binary_search(leaf,key);
    if(leaf->key[i]!=key)return;
    printf("delete key = %d,value = %s\n",key,(char*)(leaf->child[i]));
    delete(leaf,key);
}

void bplustree_destroy(){
    if(root == NULL)return;
    printf("now destroying b+tree..\n");
    destroy(root);
    root = NULL;
    printf("done.\n");
}

void bplustree_init(){
    bplustree_destroy();
    root = new_bplustreenode();
    root->isroot = true;
    root->isleaf = true;
    totalnodes = 0;
}

void bplustree_setmaxchild(int number){
    maxchild = number;
}

void bplustree_print(){
   if(root==NULL)return;
   print(root);
}

int bplustree_gettotalnodes(){
    return totalnodes;
}