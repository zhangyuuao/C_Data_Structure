#include<iostream>
#include<cstdio>
#define TRUE        1
#define FALSE       0
#define OK          1
#define ERROR       0
#define INFEASIBLE -1
#define OVERFLOW   -2
typedef int Status ;
typedef int ElemType ;
typedef ElemType *Triplet ;
using namespace std ;
Status InitTriplet (Triplet &T, ElemType v1, ElemType v2, ElemType v3){
    T = (ElemType *) malloc(3 * sizeof(ElemType)) ;
    if (!T) exit(OVERFLOW) ;
    T[0] = v1 ;
    T[1] = v2 ;
    T[2] = v3 ;
    return OK ;
} //构造三元组T

Status DestroyTriplet(Triplet &T){
    free(T) ;
    T = NULL ;
    return OK ;
}//销毁三元组

Status Get(Triplet &T, int i, ElemType &e){
    if (i < 1 || i > 3) return ERROR ;
    e = T[i - 1] ;
    return OK ;
}//获得三元组的值

Status Put(Triplet &T, int i , ElemType e){
    if (i < 1 || i > 3) return ERROR ;
    T[i - 1] = e ;
    return OK ;
}

Status ISAscending(Triplet T){
    return (T[0] <= T[1]) && (T[1] <= T[2]) ;
}

Status ISDescending(Triplet T){
    return (T[0] >= T[1]) && (T[1] >= T[2]) ;
}

Status Max(Triplet T , ElemType &e){
    e = T[0] > T[1] ? T[0] : T[1] ;
    e = e > T[2] ? e : T[2] ;
    return OK ;
}

Status Min(Triplet T , ElemType &e){
    e = T[0] < T[1] ? T[0] : T[1] ;
    e = e < T[2] ? e : T[2] ;
    return OK ;
}

int sum(Triplet T1, Triplet T2, Triplet &SUM){
    SUM[0] = T1[0] + T2[0] ;
    SUM[1] = T1[1] + T2[1] ;
    SUM[2] = T1[2] + T2[2] ; 
    return OK ;
}

int subtract(Triplet T1, Triplet T2, Triplet &SUB){
    SUB[0] = T1[0] - T2[0] ;
    SUB[1] = T1[1] - T2[1] ;
    SUB[2] = T1[2] - T2[2] ; 
    return OK ;
}

int main(){
    Triplet T1, T2, SUM, SUB ;
    ElemType e ;
    int v1,v2,v3,v4,v5,v6;
    cout << "请输入第一个三元组的三个元素：" << endl ; 
    cin >> v1 >> v2 >> v3 ;
    InitTriplet(T1,v1,v2,v3) ;
    cout << "请输入第二个三元组的三个元素：" << endl ; 
    cin >> v4 >> v5 >> v6 ;
    InitTriplet(T2,v4,v5,v6) ;
    InitTriplet(SUM,0,0,0) ;
    InitTriplet(SUB,0,0,0) ;
    sum(T1,T2,SUM) ;
    subtract(T1,T2,SUB) ;
    cout <<"和:" ; 
    for(int i = 0 ; i < 3 ; i++){
        cout << SUM[i] << ' ' ;
    }
    cout << endl ;
    cout << "差：" ;
    for(int i = 0 ; i < 3 ; i++){
        cout << SUB[i] << ' ' ;
    }
    cout << endl ;
    Get(T1, 2, e) ;
    cout << "获得第一个三元组的第二个元素："<< e << endl ;
    Put(T2, 3, 5) ;
    cout << "将第二个三元组第三个元素变为5后的结果：" ;
    for (int i = 0 ; i < 3 ; i++){
        cout << T2[i] << ' ' ;
    }
    cout << endl ;
    cout << "第一个三元组是降序嘛？"<< ISDescending(T1) << endl ;
    cout << "第二个三元组是升序嘛？" << ISAscending(T2) << endl ;
    Max(T1, e) ;
    cout << "第一个三元组的最大值：" << e << endl ;
    Min(T2, e) ; 
    cout << "第二个三元组的最小值：" << e << endl ;    
    DestroyTriplet(T1) ; 
    DestroyTriplet(T2) ; 
    DestroyTriplet(SUM) ;
    DestroyTriplet(SUB) ;
    return 0 ;
}
