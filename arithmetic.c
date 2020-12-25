#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<ctype.h>
#include<string.h>
#define Malloc(type,name,size) ({\
    name = (type*)malloc(sizeof(type)*size);\
    assert(name);})
#define CREATTREE(name) ({\
    name = (struct tree_node*)malloc(sizeof(struct tree_node));\
    assert(name);\
    name->left = name->right = NULL;})
#define isaritflags(c)  ((c == '+') || (c == '-') || (c == '*') ||\
         (c == '/') || (c == '(') || (c == ')'))
struct tree_node {
    char * expression;
    struct tree_node * left;
    struct tree_node * right;
};
static inline int 
illegal(char * experssion,int len){
    for(int i=0;i < len;i++){
        if(!isdigit(experssion[i]) || !isspace(experssion[i]) || 
            !isaritflags(experssion[i]))
            return 1;
    }
    return 0;
}
static void selfcpy(char * str,int offset){
    char tmpstr[256];
    bzero(tmpstr,256);
    strcpy(tmpstr,&(str[offset]));
    bzero(str,sizeof(str));
    strcpy(str,tmpstr);
}
static int __build(struct tree_node * t_head){

    int len;
    int i,j;
    int tmp;
    char flag[100][2];
    int count;
    char tmpstr[256];

    //remove space at sides
    len = strlen(t_head->expression);
    for(i = 0;i < len;i++){
        //printf("%c\n\r",t_head->expression[i]);
        if(!isaritflags(t_head->expression[i]) &&
             !isdigit(t_head->expression[i]) && !isspace(t_head->expression[i])){
            printf("%s is an illegal expression\n\r",t_head->expression);
            return -1;
        }
    }

    for(i = (len - 1);i >= 0;i--)
        if(!isspace(t_head->expression[i]))
            break;
    for(j = 0;j < i;j++)
        if(!isspace(t_head->expression[j]))
            break;
    if(i < len-1)t_head->expression[i+1] = 0;
    selfcpy(t_head->expression,j);

    //a number,stop recursive
    len = strlen(t_head->expression);
    tmp = 1;
    
    for(i = 1;i < len;i++)
        if(!isdigit(t_head->expression[i])){
            tmp = 0;
            break;
        }
    if(tmp)return 0;

    //make (...) as a part
    i = 0;
    count = 0;
loop:
    for(tmp = 0,j = -1;i < len;i++){
        if(t_head->expression[i] == '(' && j == -1)j = i;   //first '('
        else if(t_head->expression[i] == '(')tmp++;         //inner '('
        else if(t_head->expression[i] == ')'){
            if(j == -1){                                    //illegal ()
                printf("%s is an illegal expression\n\r",t_head->expression);
                return -1;
            }
            else if(tmp)tmp--;                              //inner ')'
            else break;                                     //the matched )
        }
    }
    if(tmp){
        printf("%s i an illegal expression\n\r",t_head->expression);
        return -1;                                       //inner () not matching
    }
    //record position of () 
    if(j != -1){
       flag[count][0] = j;
       flag[count++][1] = i;
    }

    if(++i < len)goto loop;

    //find '+' or '-'
    for(i = 0;i < len;i++){
        if(t_head->expression[i] == '-' || t_head->expression[i] == '+'){
            tmp = 1;
            for(j = 0;j < count;j++){
                if(i > flag[j][0] && i < flag[j][1])tmp = 0;
            }
            if(tmp){
                struct tree_node *leftn,*rightn;
                Malloc(struct tree_node,leftn,1);
                Malloc(struct tree_node,rightn,1);
                Malloc(char,leftn->expression,i+1);
                Malloc(char,rightn->expression,len-i);
                
                if(i == 0){
                    strcpy(leftn->expression,"0");
                    strcpy(rightn->expression,&(t_head->expression[i+1]));
                }
                else if(i == (len-1)){
                    printf("%s is an illegal expression\n\r",t_head->expression);
                }
                else{
                    strncpy(leftn->expression,t_head->expression,i);
                    strncpy(rightn->expression,&(t_head->expression[i+1]),len-i);
                }
                //tree head's expressin is a flag
                sprintf(t_head->expression,"%c",t_head->expression[i]);
                t_head->left = leftn;
                t_head->right = rightn;

                //printf("left:%s|head:%s|right:%s\n\r",leftn->expression,t_head->expression,rightn->expression);

                __build(leftn);
                __build(rightn);
                return 0;
            }
        }
    }

    //no +or-,find *or/
    for(i = 0;i < len;i++){
        if(t_head->expression[i] == '*' || t_head->expression[i] == '/'){
            tmp = 1;
            for(j = 0;j < count;j++){
                if(i > flag[j][0] && i < flag[j][1])tmp = 0;
            }
            if(tmp){
                struct tree_node *leftn,*rightn;
                Malloc(struct tree_node,leftn,1);
                Malloc(struct tree_node,rightn,1);
                Malloc(char,leftn->expression,i+1);
                Malloc(char,rightn->expression,len-i+1);
                if(i == 0 || i == (len-1)){
                    printf("%s is an illegal expression\n\r",t_head->expression);
                    return -1;
                }
                strncpy(leftn->expression,t_head->expression,i);
                strncpy(rightn->expression,&(t_head->expression[i+1]),len-i);
                sprintf(t_head->expression,"%c",t_head->expression[i]);

                t_head->left = leftn;
                t_head->right = rightn;

                //printf("left:%s|head:%s|right:%s\n\r",leftn->expression,t_head->expression,rightn->expression);

                __build(leftn);
                __build(rightn);
                return 0;
            }
        }
    }

    //no *or/
    if(count > 1){
        printf("%s is an illegal expression\n\r",t_head->expression);
        return -1;
    }

    //a pair of (),remove it
    if(flag[0][0] != 0 || flag[0][1] != (len-1)){
        printf("%s is an illegal expression\n\r",t_head->expression);
        return -1;
    }

    t_head->expression[len-1] = 0;
    selfcpy(t_head->expression,1);

    __build(t_head);
    return 0;
}
void print_tree(struct tree_node * t_head){
    if(t_head == NULL)return;
    print_tree(t_head->left);
    printf("%s\n",t_head->expression);
    print_tree(t_head->right);
    return;
}
float arithmetic(struct tree_node * t_head){
    if(strcmp(t_head->expression,"+") == 0)
        return arithmetic(t_head->left) + arithmetic(t_head->right);
    else if(strcmp(t_head->expression,"-") == 0)
        return arithmetic(t_head->left) - arithmetic(t_head->right);
    else if(strcmp(t_head->expression,"*") == 0)
        return arithmetic(t_head->left) * arithmetic(t_head->right);
    else if(strcmp(t_head->expression,"/") == 0)
        return arithmetic(t_head->left) / arithmetic(t_head->right);
    else return (atof(t_head->expression));
}
int main(){
    int len;
    char buff[256];
    struct tree_node * aritree;
    
    fgets(buff,256,stdin);
    len = strlen(buff) - 1;
    buff[len] = 0;

    CREATTREE(aritree);
    Malloc(char,aritree->expression,len+1);
    strcpy(aritree->expression,buff);
    __build(aritree);

    print_tree(aritree);

    printf("%f\n",arithmetic(aritree));

    return 0;
}