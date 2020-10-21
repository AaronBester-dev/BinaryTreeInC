#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include <string.h>

struct Performance * newPerformance(){
    struct Performance * perf = malloc(sizeof(struct Performance));
    perf->frees = 0;
    perf->mallocs = 0;
    perf->reads = 0;
    perf->writes = 0;
    return(perf);
}

void attachNode( struct Performance*performance, struct Node**node_ptr,void *src, unsigned int width ){
    struct Node * newNode = malloc(sizeof(struct Node));

    if(newNode == NULL){
        fprintf(stderr,"Can't allocate enough memory for a new node struct");
    }

    newNode->data = malloc(sizeof(unsigned char) * width);

    if(newNode->data == NULL){
        fprintf(stderr,"Can't allocate enough memory for a new data struct");
    }

    memcpy(newNode->data,src,width);
    newNode->gte = NULL;
    newNode->lt = NULL;

    *node_ptr = newNode;
    performance->mallocs++;
    performance->writes++;
}

int comparNode( struct Performance *performance, struct Node**node_ptr, int (*compar)(const void *, const void *),void *target){
    struct Node * currentNode = *node_ptr;
    performance->reads++;
    return((*compar)(target,currentNode->data));
}

struct Node**next( struct Performance *performance, struct Node**node_ptr, int direction){
    struct Node * currentNode = *node_ptr;
    
    if(currentNode == NULL){
         fprintf(stderr,"Tree is empty");
    }
   
    performance->reads++;

    if(direction >= 0){
        
        return(&(currentNode->gte));
    }
    else{
      
        return(&(currentNode->lt));
    }
}

void readNode( struct Performance *performance, struct Node **node_ptr, void *dest, unsigned int width ){
    struct Node * currentNode = *node_ptr;
    if(currentNode == NULL){
        fprintf(stderr,"Tree is empty");
    }

    memcpy(dest,currentNode->data,width);
    performance->reads++;
}

void detachNode(struct Performance *performance, struct Node**node_ptr){
    struct Node * nodeToBeFreed = *node_ptr;
    if(nodeToBeFreed == NULL){
        fprintf(stderr,"Tree is empty");
    }
    free(nodeToBeFreed->data);
    free(nodeToBeFreed);
    *node_ptr = NULL;
    performance->frees++;
}

int isEmpty(struct Performance * performance, struct Node ** node_ptr){
    if(*node_ptr == NULL){
        return 1;
    }
    else{
        return 0;
    }
}

void addItem( struct Performance *performance, struct Node**node_ptr,int (*compar)(const void *, const void *),void *src, unsigned int width ){
    struct Node ** tempPtr = node_ptr;
    int direction = 0;
     
    while(isEmpty(performance,tempPtr) != 1){
        direction = comparNode(performance,tempPtr,(*compar),src);
        tempPtr = next(performance,tempPtr,direction);
    }
    attachNode(performance,tempPtr,src,width); 
}

void freeTree( struct Performance *performance, struct Node**node_ptr){
    struct Node ** tempPtr = node_ptr;
    if(isEmpty(performance,tempPtr) != 1){
        freeTree(performance,next(performance,tempPtr,-1));
        freeTree(performance,next(performance,tempPtr,1));
        detachNode(performance,tempPtr);
    }
}

int searchItem( struct Performance * performance, struct Node ** node_ptr,int (*compar)(const void *, const void *),void *target, unsigned int width ){
    struct Node ** tempPtr = node_ptr;
    int direction = 0;
    
    if(isEmpty(performance,tempPtr) != 1){
       
        if((direction = comparNode(performance,tempPtr,(*compar),target)) == 0){
            readNode(performance,tempPtr,target,width);
            return(1);
        }
        else{
            return(searchItem(performance,next(performance,tempPtr,direction),(*compar),target,width));
        }
        
    }
    else{
        return(0);
    }
    
    
}