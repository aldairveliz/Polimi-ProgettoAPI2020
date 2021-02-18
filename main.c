#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 1025
typedef struct{
    int a;
    int b;
    char type;
    int numRows;
    int true;
    char **history;
}mem;
//lettura numero ind nell'input
int getNum(char *input, char *type){
    char *first;
    int num;
    first = strtok(input,type);
    num = atoi(first);
    return num;
}
//esegue la redo
char ** doRedo(char **editor,mem *memory, int redo, int *num, int *numRows){
    int a,b, swapp;
    char *swap=NULL;
    char **swop=NULL;
    for(int j = 0; j < redo; j++){
        a = memory[*num+j].a;
        b = memory[*num+j].b;
        if(memory[*num+j].type=='d') {
            if(memory[*num + j].true == 1) {
                swop = editor;
                swapp = *numRows;
                *numRows = memory[*num + j].numRows;
                memory[*num + j].numRows = swapp;
                editor = memory[*num + j].history;
                memory[*num + j].history = swop;
            }
        }
        else{
            if(editor[a]==NULL){
                for(int q=0; q<b-a+1; q++){
                    editor[a + q] = memory[*num + j].history[q];
                }
                memory[*num + j].history = NULL;
            }
            else{
                for(int q=0; q<b-a+1; q++){
                    if(editor[a+q]!=NULL) {
                        swap = editor[a + q];
                        editor[a+q]=memory[*num+j].history[q];
                        memory[*num+j].history[q] = swap;
                    }
                    else{
                        editor[a + q] = memory[*num + j].history[q];
                        memory[*num + j].history[q] = NULL;
                    }
                }
            }
        }
    }
    *num = *num + redo;
    return editor;
}
//esegue la undo
char ** doUndo(char **editor,mem *memory, int undo, int *num, int *numRows){
    int a,b, swapp;
    char *swap=NULL;
    char **swop=NULL;
    for(int j = 0; j < undo; j++) {
        a = memory[*num - 1 - j].a;
        b = memory[*num - 1 - j].b;
        if (memory[*num - 1 - j].type == 'c'){
            if (memory[(*num) - 1 - j].history == NULL) {
                memory[(*num) - 1 - j].history = malloc((b - a + 1) * sizeof(char *));
                if (memory[(*num) - 1 - j].history == NULL) {
                    exit(0);
                }
                for (int k = 0; k < b - a + 1; k++) {
                    memory[(*num) - 1 - j].history[k] = editor[a + k];
                    editor[a + k] = NULL;
                }
            } else {
                for (int k = 0; k < b - a + 1; k++) {
                    swap = editor[a + k];
                    if (memory[(*num) - 1 - j].history[k] != NULL) {
                        editor[a + k] = memory[(*num) - 1 - j].history[k];
                    } else {
                        editor[a + k] = NULL;
                    }
                    memory[(*num) - 1 - j].history[k] = swap;
                }
            }
        }
        else{
            if(memory[*num - 1 - j].true == 1) {
                swop = editor;
                swapp = *numRows;
                *numRows = memory[*num - 1 - j].numRows;
                editor = memory[*num - 1 - j].history;
                memory[*num - 1 - j].history = swop;
                memory[*num - 1 - j].numRows = swapp;
            }
        }
    }
    *num = *num-undo;
    return editor;
}
//accorpa redo di fila
int accorpaRedo(char *input, int *num, int *numUndo) {
    int r = getNum(input, "r");
    int u = 0;
    int len;
    fgets(input, MAX_LENGTH, stdin);
    len = strlen(input);
    while (input[len - 2] == 'u' || input[len - 2] == 'r') {
        while (input[len - 2] == 'r') {
            r = r + getNum(input, "r");
            fgets(input, MAX_LENGTH, stdin);
            len = strlen(input);
        }
        if (r > *numUndo) {
            r = *numUndo;
        }
        while (input[len - 2] == 'u') {
            u = u + getNum(input, "u");
            fgets(input, MAX_LENGTH, stdin);
            len = strlen(input);
        }
        if (u > *num + r) {
            u = *num + r;
        }
        r = r - u;
        u = 0;
    }
    return r;
}
//accorpa undo di fila
int accorpaUndo(char *input, int *num, int *numUndo){
    int u = getNum(input,"u");
    int r = 0;
    int len;
    fgets(input, MAX_LENGTH, stdin);
    len = strlen(input);
    while(input[len-2]=='u'||input[len-2]=='r'){
        while(input[len-2]=='u'){
            u = u + getNum(input,"u");
            fgets(input, MAX_LENGTH, stdin);
            len = strlen(input);
        }
        if(u > *num){
            u = *num;
        }
        while(input[len-2]=='r'){
            r = r + getNum(input,"r");
            fgets(input, MAX_LENGTH, stdin);
            len = strlen(input);
        }
        if(r > *numUndo + u){
            r = *numUndo + u;
        }
        u = u - r;
        r = 0;
    }
    return u;
}
//memorizzo i comandi eseguiti che saranno utili al momento di fare la undo/redo
mem FillMem(mem *memory, int a, int b, char type, int numRows){
    memory->a = a;
    memory->b = b;
    memory->type = type;
    memory->numRows = numRows;
    memory->history = NULL;
    memory->true = 1;
    return *memory;
}
//estende array dinamico contenente le righe dell'editor
char ** expandEditor(char **editor, int *numRows){
    *numRows = *numRows * 2;
    editor = realloc(editor, *numRows * sizeof(char *));
    if(editor!=NULL) {
        return editor;
    }
}
//inizializza array dinamico per contenere le righe dell'editor
char ** createEditor(char **editor, int *numRows) {
    editor = malloc(*numRows * sizeof(char *));
    if(editor!=NULL)
        return editor;
}
//esegue comando change
char ** doChange(char **editor, char *input, char *buffer, int *numRows, mem *memory, int *num){
    int a, b, len;
    char *first;
    char *second;
    first = strtok(input,",");
    a = atoi(first);
    second = strtok(NULL,"c");
    b = atoi(second);
    while(b>=*numRows){
        editor = expandEditor(editor, numRows);
    }
    memory[*num] = FillMem(&memory[*num],a,b,'c',*numRows);
    if(editor[a]==NULL){
        memory[*num].history=NULL;
    }
    else{
        memory[*num].history=malloc((b-a+1) * sizeof(char *));
        if(memory[*num].history==NULL)
            exit(0);
    }
    for(int i=0; i<b-a+1; i++){
        fgets(buffer, MAX_LENGTH, stdin);
        len = strlen(buffer);
        if(editor[a+i]!=NULL) {
            memory[*num].history[i] = editor[a + i];
            editor[a+i]=NULL;
            editor[a+i] = malloc(len+1);
            if(editor[a+i]!=NULL) {
                strcpy(editor[a + i], buffer);
            }
        }else {
            if (memory[*num].history != NULL){
                memory[*num].history[i] = NULL;
            }
            editor[a + i] = malloc(len + 1);
            if (editor[a + i] != NULL) {
                strcpy(editor[a + i], buffer);
            }
        }
    }
    fgets(input, MAX_LENGTH, stdin);
    return editor;
}
//esegue comando print
void doPrint(char **editor, char *input, int *numRows){
    int a, b;
    char *first;
    char *second;
    first = strtok(input,",");
    a = atoi(first);
    second = strtok(NULL,"p");
    b = atoi(second);
    int l=0;
    for (int i = 0; i < b - a + 1; i++) {
        if(!editor[a+i] || a+i>=*numRows) {
            fputs(".\n",stdout);
        }else {
            fputs(editor[a + i], stdout);
        }
    }
}
//esegue comando delete
char ** doDelete(char **editor, char *input, int *numRows, mem *memory, int *num){
    int a, b;
    int i = 0;
    int j = 0;
    int l = 0;
    char *first;
    char *second;
    first = strtok(input,",");
    a = atoi(first);
    second = strtok(NULL,"d");
    b = atoi(second);
    memory[*num]=FillMem(&memory[*num],a,b,'d', *numRows);
    if(a>=*numRows){
        memory[*num].true = 0;
        return editor;
    }
    if(b>=*numRows){
        b=*numRows-1;
    }
    if(a==0) {
        a=1;
        if(b==0) {
            memory[*num].true = 0;
            return editor;
        }
    }
    if(editor[a]==NULL) {
        memory[*num].true = 0;
        return editor;
    }
    memory[*num].history = malloc(*numRows * sizeof(char *));
    if(memory[*num].history==NULL){
        exit(0);
    }
    while(editor[1+l]!=NULL){
        memory[*num].history[1+l]= malloc(strlen(editor[1+l])+1);
        if(memory[*num].history[1+l]){
            strcpy(memory[*num].history[1+l],editor[1+l]);
        }
        l++;
    }
    while(b+1+i< *numRows && editor[b+1+i]!=NULL){
        if(editor[a+i]!=NULL) {
            //free(editor[a+i]);
            editor[a + i] = editor[b + 1 + i];
            editor[b+1+i] = NULL;
        }else{
            editor[a+i] = editor[b+1+i];
            editor[b+1+i] = NULL;
        }
        i++;
    }
    while(a+i+j < *numRows && editor[a+i+j]!=NULL){
        //free(editor[a+i+j]);
        editor[a+i+j] = NULL;
        j++;
    }
    return editor;
}
int main() {
    char input[100]="";
    char buffer[MAX_LENGTH]="";
    char **editor=NULL;
    mem *memory=NULL;
    int undo, redo, len;
    int numRows=200000;
    int numUndo = 0;
    int num = 0;
    int numMemory=10000;
    fgets(input, MAX_LENGTH, stdin);
    //primo comando non è q allora inizializzo editor
    if(strcmp(input,"q\n")!=0){
        editor = createEditor(editor, &numRows);
        memory = malloc(numMemory*sizeof(mem));
        if(memory == NULL) {
            exit(0);
        }
    }
    //ciclo finche comando non sia q per terminare
    while(strcmp(input,"q\n")!=0){
        len = strlen(input);
        if(num==numMemory){
            numMemory = numMemory * 2;
            memory = realloc(memory, numMemory * sizeof(mem));
            if(memory==NULL)
                exit(0);
        }
        if(input[len-2]=='c'){
            editor = doChange(editor,input, buffer, &numRows, memory, &num);
            num=num+1;
            numUndo = 0;
            fgets(input, MAX_LENGTH, stdin);
        }else {
            if (input[len - 2] == 'p') {
                doPrint(editor, input, &numRows);
                fgets(input, MAX_LENGTH, stdin);
            }else {
                if (input[len - 2] == 'd') {
                    editor = doDelete(editor, input, &numRows, memory, &num);
                    num=num+1;
                    numUndo = 0;
                    fgets(input, MAX_LENGTH, stdin);
                }
                else{
                    if (input[len - 2] == 'u') {
                        undo = accorpaUndo(input, &num, &numUndo);
                        if(undo > num){
                            undo = num;
                            editor = doUndo(editor, memory, undo, &num, &numRows);
                        }
                        else{
                            if(undo<0){
                                editor = doRedo(editor, memory, -undo, &num, &numRows);
                            }
                            else{
                                editor = doUndo(editor, memory, undo, &num, &numRows);
                            }
                        }
                        numUndo = numUndo + undo;
                    }
                    else{
                        if (input[len - 2] == 'r') {
                            redo = accorpaRedo(input, &num, &numUndo);
                            if(redo>numUndo){
                                redo = numUndo;
                                editor = doRedo(editor, memory, redo, &num, &numRows);
                            }
                            else{
                                if(redo<0){
                                    editor = doUndo(editor, memory, -redo, &num, &numRows);
                                }
                                else{
                                    editor = doRedo(editor, memory, redo, &num, &numRows);
                                }
                            }
                            numUndo = numUndo - redo;
                        }
                    }
                }
            }
        }
    }
    return 0;
}