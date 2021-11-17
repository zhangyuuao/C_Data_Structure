#include<cstdio>
#include<cstdlib>
#include<iostream>
#define STACK_INIT_SIZE 100
#define STACKINCREMENT   50
#define MAXQSIZE        100 
#define Overflow         -1
using namespace std ;

//二叉树结点结构
typedef struct BTNode{
    char data ;
    struct BTNode *LChild ;
    struct BTNode *RChild ;
}BinaryTreeNode , *BiTree ; 

//栈的结构
typedef struct{
	BiTree* base ;
	BiTree* top ;
	int stacksize ; 
}SqStack ;

//循环队列的结构
typedef struct{
	BiTree* base ;
    int front ;
    int rear ;
}CircularQueue ;

//栈初始化
void InitStack(SqStack &s){
	s.base = (BiTree*)malloc(STACK_INIT_SIZE * sizeof(BiTree)) ;
	if(!s.base) exit(Overflow) ;
	s.top = s.base ;
	s.stacksize = STACK_INIT_SIZE ;
	return ;
}

//销毁栈
void DestroyStack(SqStack &s){
    s.top = s.base ;
    free(s.base) ;
    s.stacksize = 0 ;
    return ;
}

//栈空
int StackEmpty(SqStack s){
    if(s.base == s.top) return 1 ;
    return 0 ;
}

//获得栈顶元素
BiTree GetTop(SqStack s , BiTree &p){
	if(s.top == s.base){
		cout << "ERROR When get top" << endl ;
        return 0 ;
	}
	BiTree e = *(s.top - 1) ;
    p = e ;
	return e ;
}

//进栈
void Push(SqStack &s , BiTree T){
	if(s.top - s.base >= s.stacksize){
		s.base = (BiTree*)realloc(s.base , (s.stacksize + STACKINCREMENT) * sizeof(BiTree)) ;
		if(!s.base) exit(Overflow) ;
		s.top = s.base + s.stacksize ;
		s.stacksize += STACKINCREMENT ;
	}
	*s.top++ = T ;
	return ;
}

//弹栈
void Pop(SqStack &s , BiTree &T){
    	if(s.top == s.base){
		cout << "ERROR When pop" << endl ;
		return ;
	}
	T = * --s.top ;
}

//队列初始化
void InitQueue(CircularQueue &Q){
    Q.base = (BiTree*)malloc(MAXQSIZE * sizeof(BiTree)) ;
    if(!Q.base) exit(Overflow) ;
    Q.front = 0 ;
    Q.rear = 0 ;
}

//销毁队列
void DestroyQueue(CircularQueue &Q){
    Q.front = 0 ;
    Q.rear = 0 ;
    free(Q.base) ;
}

//进队
void InsertQueue(CircularQueue &Q , BiTree T){
    if ((Q.rear + 1) % MAXQSIZE == Q.front) return ;
    Q.base[Q.rear] = T ;
    Q.rear = (Q.rear + 1) % MAXQSIZE ;
}

//出队
void DeleteQueue(CircularQueue &Q , BiTree &T){
    if(Q.front == Q.rear) return ;
    T = Q.base[Q.front] ;
    Q.front = (Q.front + 1) % MAXQSIZE ;
}

//查看队列长度
int GetQueueLen(CircularQueue Q){
    return (Q.rear - Q.front + MAXQSIZE) % MAXQSIZE ;
}

//创建二叉树
void Create_BinaryTree(BiTree &T){
    char ch ;
    cin >> ch ;
    if(ch == '1') T = NULL ;
    else{
        if(!(T = (BiTree)malloc(sizeof(BinaryTreeNode)))) exit(Overflow) ;
        T -> data = ch ;
        //cout << "请输入" << ch << "的左结点" << endl ;
        Create_BinaryTree(T -> LChild) ;
        //cout << "请输入" << ch << "的右结点" << endl ;
        Create_BinaryTree(T -> RChild) ;
    }
    return ;
}

//释放空间
void FreeTree(BiTree &T){
    if(T){
        FreeTree(T -> LChild) ;
        FreeTree(T -> RChild) ;
        free(T) ;
        T = NULL ;
    }
    return ;
}

//访问结点
bool Visit(char e){
    cout << e ;
    return true ;
}

//先序遍历
void PreOrderTraverse(BiTree T ,  bool(*Visit)(char e)){
    if(T){
        Visit(T -> data) ;
        PreOrderTraverse(T -> LChild , Visit) ;
        PreOrderTraverse(T -> RChild , Visit) ;
    }
    return ;
}

//中序遍历
void InOrderTraverse(BiTree T , bool(*Visit)(char e)){
    if(T){
        InOrderTraverse(T -> LChild , Visit) ;
        Visit(T -> data) ;
        InOrderTraverse(T -> RChild , Visit) ; 
    }
    return ;
}

//后序遍历
void PostOrderTraverse(BiTree T , bool(*Visit)(char e)){
    if(T){
    PostOrderTraverse(T -> LChild , Visit) ;
    PostOrderTraverse(T -> RChild , Visit) ;
    Visit(T -> data) ;
    }
    return ;
}

//获取叶子结点个数
int GetLeavesCount_BinaryTree(BiTree T){
	int count = 0 ;
    int lcount = 0 ;
    int rcount = 0 ;
	if (T != NULL){
		if (T -> LChild == NULL && T-> RChild == NULL) count = 1 ;
        else{
		lcount = GetLeavesCount_BinaryTree(T -> LChild) ;
		rcount = GetLeavesCount_BinaryTree(T -> RChild) ;
        count = lcount + rcount ;
        }
	}
    return count ;
}

//获取二叉树深度
int GetDepth_BinaryTree(BiTree T){
	int BiTreeDepth = 0 ;
	int LeftChildTreeDepth = 0 ;
	int RightChileTreeDepth = 0 ;
	if (T != NULL){
		LeftChildTreeDepth = GetDepth_BinaryTree(T -> LChild) ;
		RightChileTreeDepth = GetDepth_BinaryTree(T -> RChild);
		BiTreeDepth = (LeftChildTreeDepth >= RightChileTreeDepth ? LeftChildTreeDepth + 1 : RightChileTreeDepth + 1) ;
	}
	return BiTreeDepth ;
}

//非递归的中序遍历算法1
void InOrderTraverse2(BiTree T , bool(*Visit)(char e)){
    SqStack S ;
    BiTree p  = T ;
    InitStack(S) ;
    Push(S , T) ;
    p = T ;
    while(!StackEmpty(S)){
        while(GetTop(S  , p) && p) Push(S , p -> LChild) ;
        Pop(S , p) ;
        if(!StackEmpty(S)){
            Pop(S , p) ;
            if(!Visit(p -> data)) return ;
            Push(S , p -> RChild) ;
        }
    }
    DestroyStack(S) ;
    return ;
}

//非递归的中序遍历算法2
void InOrderTraverse3(BiTree T , bool(*Visit)(char e)){
    SqStack S ;
    InitStack(S) ;
    BiTree p = T ;
    while (p || !StackEmpty(S)){
        if (p){
            Push(S, p) ;
            p = p -> LChild ;
        } //根指针进栈，遍历左子树
        else{
            Pop(S, p) ;
            if (!Visit(p -> data)) return ;
            p = p -> RChild ;
        }
    }
    DestroyStack(S) ;
    return ;
}

//用队列实现二叉链树的层次遍历
void LevelTraverse(BiTree T , bool(*Visit)(char e)){
    CircularQueue Q ;
    BinaryTreeNode *p ;
    InitQueue(Q) ;
    InsertQueue(Q , T) ;
    while(GetQueueLen(Q)!= 0){
        DeleteQueue(Q , p) ;
        Visit(p -> data) ;
        if(p -> LChild) InsertQueue(Q , p -> LChild) ;
        if(p -> RChild) InsertQueue(Q , p -> RChild) ;
    }
    DestroyQueue(Q) ;
    return ;
}

/*测试函数-将实现的各个功能进行测试
测试样例：AB1DF11G11C1E1H11，直接复制输入即可，注意不要打空格*/
void test(){
    BiTree T ;
    Create_BinaryTree(T) ;
    cout << "先序遍历:" ;
    PreOrderTraverse(T , Visit);
    cout << '\n' ;
    cout << "采用递归的中序遍历:" ;
    InOrderTraverse(T , Visit) ;
    cout << '\n' ;
    cout << "用栈实现的第一种中序遍历:" ;
    InOrderTraverse2(T , Visit) ;
    cout << '\n' ;
    cout << "用栈实现的第二种中序遍历:" ;
    InOrderTraverse3(T , Visit) ;
    cout << '\n' ;
    cout << "后序遍历:" ;
    PostOrderTraverse(T , Visit) ;
    cout << '\n' ;
    cout << "层次遍历:" ;
    LevelTraverse(T , Visit) ;
    cout << '\n' ;
    cout << "该树的叶子结点数:" ;
    cout << GetLeavesCount_BinaryTree(T) << endl ;
    cout << "该树的深度:" ;
    cout << GetDepth_BinaryTree(T) << endl ;
}

int main(){
    test() ;
}
