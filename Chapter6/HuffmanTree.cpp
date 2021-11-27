#include<iostream>
#include<fstream>
#include<cstdlib>
#define MAX 10000
const int SIZE = 256 ;
using namespace std ;

//二级指针分配数组来存编码
typedef char **HuffmanCode ;

/*这段结构的定义采用ascii码为下标存取字符，C数组代表存取的字符；weight数组代表字符的权值。
count记录不同的字符数，Total_num负责记录文章所有字符数。*/
typedef struct TmpNode{
    char C[150] ;
    double weight[150] ;
    int count ;
    double Total_num ;
}TmpNode , *CodeChart ;

//Huffman树的结点结构
typedef  struct{
    int weight ;
    int parent , lchild , rchild ;
}HTNode , *HuffmanTree ;

typedef struct{
    HuffmanTree HT ;
    HuffmanCode HC ;
    char *c ;
    int length ;
}Huffman ;

//词典初始化
void Init_TmpNode(CodeChart &Chart){
    Chart = new TmpNode ;
    Chart -> count = 0 ;
    for(int i=0 ; i < 150 ; i++)
    {
        Chart -> weight[i] = 0 ;
        Chart -> C[i] = 0;
    }
    Chart -> Total_num = 0 ;
}

//文件转词典
void file2Dict(CodeChart &Chart){
    char filename[SIZE] ;
    string s ;
    ifstream inFile ;
    cout << "Enter the name of file :" << endl ;
    cin.getline(filename , SIZE) ;
    inFile.open(filename) ;
    if(!inFile.is_open()){
        cout << "Could not open the file." << filename << endl ;
        cout << "Program terminating\n" ;
        exit(EXIT_FAILURE) ;
    }
    char ch ;
    inFile.get(ch) ;
    while(inFile.good()){
        int i = (int)ch ;
        Chart -> C[i] = ch ;
        Chart -> weight[i]++ ;
        if(Chart -> weight[i] == 1) Chart -> count++ ; 
        Chart -> Total_num++ ;
        inFile.get(ch) ;
    }
    for(int i = 0 ; i < 150 ; i++){
        if(Chart -> C[i] && Chart -> weight[i]){
            cout << Chart -> C[i] << '\t' << Chart -> weight[i] << endl ;
        }
    }
    cout << "Different characters' number: " << Chart -> count << endl ;
    cout << "Total characters: " << Chart -> Total_num << endl ;
    inFile.close() ;
    cout << "Read file successfully." << endl ;
}

/*通过一次循环遍历找到两个最小的结点，记作S1，S2。
注意这是一个优秀的O(n)算法。*/
void select(HTNode *HT , int n , int &s1 , int &s2){
	int min = MAX ;
    int min2 = MAX + 1 ;
	int p , q ;
	for(int i = 1 ; i <= n ; i++){
		if(HT[i].parent == 0){
            if(HT[i].weight <= min && HT[i].weight < min2){
                min2 = min ; 
                min = HT[i].weight ;
                q = p ;
                p = i ;
            }
            if(HT[i].weight < min2 && HT[i].weight > min){
                min2 = HT[i].weight ;
                q = i ;
            }	
        }
	}
	s1 = p ;
	s2 = q ;
}

//哈夫曼编码
Huffman HuffmanCoding(Huffman Hfm){
    //存放n个字符的权值（均〉0），构造哈夫曼树HT，并求出n个字符的构造哈夫曼编码HC
    int i , n , m , s1 , s2 , start ;
    int c , f ;
    char *cd ;
    n = Hfm.length ;
    if (n <= 1)  return Hfm ;
    m = 2*n - 1 ;
    for(i = n+1 ; i <= m ; ++i) {//选择HT[1....i-1]中无双亲且权值最小的两个节点，其序号为s1，s2
        select(Hfm.HT, i-1 , s1 , s2) ;
        Hfm.HT[s1].parent = i ;//修改父亲位置
        Hfm.HT[s2].parent = i ;
        Hfm.HT[i].lchild = s1 ;//修改孩子位置
        Hfm.HT[i].rchild = s2 ;
        Hfm.HT[i].weight = Hfm.HT[s1].weight + Hfm.HT[s2].weight ;//父亲结点权值为左右孩子权值之和
    }
    //从叶子结点到根逆向求每个字符的哈夫曼编码
    Hfm.HC = (HuffmanCode)malloc((n+1)*sizeof(char *)) ;//分配n个字符编码的头指针向量
    cd = (char *)malloc(n*sizeof(char)) ;//分配求编码的工作空间
    cd[n-1] = '\0' ;//编码结束符
    for (i = 1 ; i <= n ; ++i){//逐个字符求哈夫曼编码
        start = n - 1 ;//编码结束符位置
        for(c = i , f = Hfm.HT[i].parent ; f != 0 ; c = f , f = Hfm.HT[f].parent){
            //从叶子到根逆向求编码
            if(c == Hfm.HT[f].lchild){
                cd[--start] = '0' ;
            }
            else cd[--start] = '1' ;
        }
        Hfm.HC[i] = (char *)malloc((n-start)*sizeof(char)) ;
        strcpy(Hfm.HC[i], &cd[start]) ;//从cd复制编码到Hfm.HC
    }
    free(cd) ;//释放工作空间
    return Hfm ;
}

//输入数据
Huffman InputHuffman(Huffman Hfm , CodeChart c){  //输入函数，控制用户输入字符和相应权值
    int i , j = 0 , n ;
    n = c -> count ;
    Hfm.HT = (HuffmanTree)malloc((2 * n) * sizeof(HTNode)) ;
    Hfm.c = (char *)malloc((n + 1) * sizeof(char)) ;
    for (i = 1; i <= n; i++) {
        for(j = j ; j < 150 ; j++){
            if(c -> weight[j] != 0){
                Hfm.c[i] = c -> C[j] ;
                Hfm.HT[i].weight = c -> weight[j] ;
                break ;
            }
        }
        j++ ;
        Hfm.HT[i].parent = 0 ;
        Hfm.HT[i].lchild = 0 ;
        Hfm.HT[i].rchild = 0 ;
    }
    for(; i <= 2 * n - 1 ; ++i) {
        Hfm.HT[i].weight = 0 ;
        Hfm.HT[i].parent = 0 ;
        Hfm.HT[i].lchild = 0 ;
        Hfm.HT[i].rchild = 0 ;
    }
    Hfm.length = n ;
    return Hfm ;
}

/*这个函数运行时会将你编码的词典保存在一个叫CodeDict.txt的文件中。*/
void CodeDictStore(Huffman hfm){
    int i = 0 ;
    int n = hfm.length ;
    ofstream outFile ;
    outFile.open("CodeDict.txt") ;
    for(i = 1 ; i <= n ; i++){
        cout << hfm.c[i] << '\t' << hfm.HC[i] << endl ;
        outFile << hfm.c[i] << '\t' << hfm.HC[i] << endl ;
    }
    outFile.close() ;
    cout << "Store the dict into file: CodeDict.txt." << endl ;
}

/*编码函数。运行这个函数你将输入一个待编码文件的名称，
然后它将根据已经创建好的哈夫曼编码树为你生成一个codefile.txt。*/
void Encoding(Huffman hfm){
    int i = 0 ;
    int j = 0 ;
    int n = hfm.length ;
    int k = 0 ;
    char ch ;
    char filename[SIZE] ;
    ofstream outFile ;
    ifstream inFile ;
    cout << "\n--Encoding--\n" << endl ;
    cout << "Enter the name of file :" << endl ;
    cin.getline(filename , SIZE) ;
    inFile.open(filename) ;
    outFile.open("codefile.txt") ;
    if(!inFile.is_open()){
        cout << "Could not open the file." << filename << endl ;
        cout << "Program terminating\n" ;
        exit(EXIT_FAILURE) ;
    }
    inFile.get(ch) ;
    while(inFile.good()){
        for(i = 1 ; i <= n ; i++){
            if(ch == hfm.c[i]){
                cout << hfm.HC[i] ;
                outFile << hfm.HC[i] ;
            }
        }
        inFile.get(ch) ;
    }
    cout << "\nStore the code into file: codefile.txt." << endl ;
    inFile.close() ;
    outFile.close() ;
}

/*解码文件。会将结果先输出在终端，再保存在一个Text.txt文件中。*/
void Decoding(Huffman hfm , string filename){
    HuffmanTree p ;
    int i ;
    int j = 0 ;
    int n = hfm.length ;
    ifstream inFile ;
    ofstream outFile ;
    cout << "\n--Decoding--\n" << endl ;
    inFile.open(filename) ;
    string d ;
    getline(inFile , d) ;
    inFile.close() ;
    outFile.open("Text.txt") ;
    while(d[j]){
        p = &hfm.HT[2 * n - 1] ;
        while(p -> lchild || p -> rchild){
            if(d[j] == '0'){
                i = p -> lchild ;
                p = &hfm.HT[i] ;
            }
            else{
                i = p -> rchild ;
                p = &hfm.HT[i] ;
            }
            j++ ;
        }
        cout << hfm.c[i] ;
        outFile << hfm.c[i] ;
    }
    outFile.close() ;
}

int WPL(Huffman hfm){
    int n = hfm.length ;
    int wpl = 0 ;
    for(int i = 1 ; i <= n ; i++){
        wpl += hfm.HT[i].weight * strlen(hfm.HC[i]) ;
    }
    return wpl ;
}

int main(){
    cout << "--Huffman--" << endl ;
    char c ;
    c = toupper(c) ;
    CodeChart Chart ;
    Huffman hfm ;
    Init_TmpNode(Chart) ;
    file2Dict(Chart) ;
    hfm = InputHuffman(hfm , Chart) ;
    hfm = HuffmanCoding(hfm) ;
    // Encoding(hfm) ;
    Decoding(hfm , "codefile.txt") ;
    // CodeDictStore(hfm) ;
    // cout << WPL(hfm) << endl ;
    free(hfm.c) ;
    free(hfm.HC) ;
    free(hfm.HT) ;
}