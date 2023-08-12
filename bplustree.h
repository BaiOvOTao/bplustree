#define __bplustree_H__

#define max_child 1000

typedef struct bplustreenode
{
    int isroot, isleaf;
    int key_num;
    int key[max_child];
    int pos[max_child];
    void* child[max_child];
    struct bplustreenode* father;
    struct bplustreenode* next;
    struct bplustreenode* last;
}bplustreenode;

extern void bplustree_setmaxchild(int);
extern void bplustree_init();
extern void bplustree_destroy();
extern int bplustree_insert(int,int,void*);
extern int bplustree_totalnodes();
extern void bplustree_key(int);
extern void bplustree_modify(int,void*);
extern void bplustree_delete(int);
extern void bplustree_print();
extern int bplustree_find(int);