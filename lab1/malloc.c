#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
/////////////////////////////////////////////////BST structures///////////////////////////////////////////////////////////
struct Metadata;
typedef struct Link
{
        struct Metadata *left;
        struct Metadata *right;
}Links;
typedef struct Metadata			//size 48
{
        struct Link links[2];	//0 for size  1 for address
	size_t req_size;	//this is size that is requested
        size_t size;		//this is how much we allocate
}Node;
//initial BST state
Node *root[2]={NULL,NULL};
//////////////////////////////////////////////////BST operations///////////////////////////////////////////////////////////
void *free_list(Node *node,size_t size);

Node * search_Parent(Node *node,Node *target,int mode);
size_t roundoff(size_t size);
void display(Node *node,int i);
Node *getMinNode(Node * node,int mode);

void shortCircuit_size(Node * node);
void removeLeaf_size(Node *node);
void promotion_size(Node * node);
void insertNode_size(Node *node,Node *to_insert);

void shortCircuit_address(Node * node);
void removeLeaf_address(Node *node);
void promotion_address(Node * node);
void insertNode_address(Node *node,Node *to_insert);

void *cs550_malloc(size_t size);
void *cs550_free(void *ptr);
void *cs550_calloc(size_t nmemb, size_t size);
void *cs550_realloc(void *ptr, size_t size);
//////////////////////////////////COMMON TO BOTH FREE AND MALLOC//////////////////////////
size_t roundoff(size_t size)
{
	while(size%8!=0)
		size++;
	return size;
}
void display(Node *node,int mode)
{
	if(node==NULL)
                return;
	display(node->links[mode].left,mode);
	printf(" (%d %d<>%d ) ",node->req_size,node->size,node);
	display(node->links[mode].right,mode);
}
Node * search_Parent(Node *node,Node *target,int mode)
{
	Node *nod;
	if(target < node && node->links[mode].left!=NULL)
	{
   		if((node->links[mode].left)==target)
			return node;
   		else
      			search_Parent(node->links[mode].left,target,mode);
	}
	else if(node->links[mode].right!=NULL)
  	{
    		if((node->links[mode].right)==target)
			return node;
    		else
    			search_Parent(node->links[mode].right,target,mode);
  	}
	return NULL;
}
//////////////////////////////////////malloc operation/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//||||||||||||||||||||||||||||||||BOTH SIZE AND ADDRESS|||||||||||||||||||||||||||||||||||||||||||||||||
//0 for size and 1 for address
Node *getMinNode(Node * node,int mode)
{
	if((node->links[mode].left)->links[mode].left!=NULL)
		getMinNode(node->links[mode].left,mode);
	return node;
}
//||||||||||||||||||||||||||||||||||||SIZE||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void shortCircuit_size(Node * node)
{
	if(root[0]!=node)
	{
		Node *Parent;
        	Parent=search_Parent(root[0],node,0);
        	if(Parent->links[0].left==node)
        	 	if(node->links[0].left!=NULL)
			{
				Parent->links[0].left=node->links[0].left;
			}
			else
			{
        	        	Parent->links[0].left=node->links[0].right;
 			}
		else if(Parent->links[0].right==node)
			if(node->links[0].left!=NULL)
              	  {
				Parent->links[0].right=node->links[0].left;
			}
			else
			{
               		         Parent->links[0].right=node->links[0].right;
			}
	}
	else								//incase node is root and one branched
	{
		if(node->links[0].right!=NULL)
		{
			root[0]=node->links[0].right;
		}
		else if(node->links[0].left!=NULL)
		{
			root[0]=node->links[0].left;
		}
	}
}
void removeLeaf_size(Node *node)
{
	if(root[0]!=node)
	{
		Node *Parent;
		Parent=search_Parent(root[0],node,0);
     //printf("parent of node: %d\n",Parent->size);
		if(Parent!=NULL)
    		{
        		if(Parent->links[0].left==node)
			  	Parent->links[0].left=NULL;
        		if(Parent->links[0].right==node)
          			Parent->links[0].right=NULL;
    		}
     	else
       		root[0]=NULL;
  	}
	else
		root[0]=NULL;
}
void promotion_size(Node * node)
{
	if(node->links[0].left==NULL && node->links[0].right==NULL)		//if leaf
		{removeLeaf_size(node);}
	else if(node->links[0].left==NULL || node->links[0].right==NULL)		//if 1 branch
		shortCircuit_size(node);
	else
	{
		Node *succ_parent=getMinNode(node->links[0].right,0);
		Node *successor=succ_parent->links[0].left;
		if(root[0]==node)						//if root is to be deleted
			root[0]=successor;
		if(node->links[0].left!=successor)				//this check is to avoid linking the successor to itself
			successor->links[0].left=node->links[0].left;
		if(node->links[0].right!=successor)				//this check is to avoid linking the successor to itself
			successor->links[0].right=node->links[0].right;
		node->links[0].right=node->links[0].left=NULL;
		succ_parent->links[0].left=NULL;
	}
}
//||||||||||||||||||||||||||||||||||||ADDRESS||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void shortCircuit_address(Node * node)
{
	if(root[1]!=node)
	{
		Node *Parent;
 	  Parent=search_Parent(root[1],node,1);
 	  if(Parent->links[1].left==node)
    	if(node->links[1].left!=NULL)
	    {Parent->links[1].left=node->links[1].left;}
			else
			{Parent->links[1].left=node->links[1].right;}
		else if(Parent->links[1].right==node)
			if(node->links[1].left!=NULL)
	    {Parent->links[1].right=node->links[1].left;}
			else
			{Parent->links[1].right=node->links[1].right;}
	}
	else								//incase node is root and one branched
	{
		if(node->links[1].right!=NULL)
		{root[1]=node->links[1].right;}
		else if(node->links[1].left!=NULL)
		{root[1]=node->links[1].left;}
	}
}

void removeLeaf_address(Node *node)
{
	if(root[1]!=node)
	{
		Node *Parent;
		Parent=search_Parent(root[1],node,1);
//    printf("root[1]: %d\n",root[1]->size);
		if(Parent!=NULL)
    		{
        		if(Parent->links[1].left==node)
			  	Parent->links[1].left=NULL;
        		if(Parent->links[1].right==node)
          			Parent->links[1].right=NULL;
    		}
     	else
       		root[1]=NULL;
  	}
	else
		root[1]=NULL;
}
void promotion_address(Node * node)
{
	if(node->links[1].left==NULL && node->links[1].right==NULL)		//if leaf
		{removeLeaf_address(node);}
	else if(node->links[1].left==NULL || node->links[1].right==NULL)		//if 1 branch
		shortCircuit_address(node);
	else
	{
		Node *succ_parent=getMinNode(node->links[1].right,1);
		Node *successor=succ_parent->links[1].left;
		if(root[1]==node)						//if root is to be deleted
			root[1]=successor;
		if(node->links[1].left!=successor)				//this check is to avoid linking the successor to itself
			successor->links[1].left=node->links[1].left;
		if(node->links[1].right!=successor)				//this check is to avoid linking the successor to itself
			successor->links[1].right=node->links[1].right;
		node->links[1].right=node->links[1].left=NULL;
		succ_parent->links[1].left=NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *free_list(Node *node,size_t size)//split here
{
  //printf("size : %zd node->size : %zd\n",size,node->size);
	if(node->size<size)  
  {
		Node *nod;
   
	  if(node->links[0].right!=NULL)
    {
			nod=free_list(node->links[0].right,size);
			return nod;
		}
		else return NULL;
	}
  else if(node->size>=size && node->size<(size+sizeof(Node)+8))//if not splitable
  {
     //printf("almost or exact fit\n");
	   //delete the node from both BSTs
	   promotion_size(node);
     promotion_address(node);
     node->links[0].left=node->links[0].right=NULL;
     node->links[1].left=node->links[1].right=NULL;
     return node;
  }
  else//split
  {
      //delete node
      //printf("split node : %d\n",node->size);
      promotion_size(node);
      promotion_address(node);
      node->links[0].left=node->links[0].right=NULL;
      node->links[1].left=node->links[1].right=NULL;
      
      //we split the node into one to be allocated and other to be reinserted
      void *splitt=(char *)node+size+sizeof(Node);
      Node *split=splitt;
      split->size=(node->size)-size-sizeof(Node);
      node->size=size;
      split->links[0].left=split->links[0].right=NULL;
      split->links[1].left=split->links[1].right=NULL;

      if(root[0]==NULL)
      {  root[0]=root[1]=split;}
      else
      {  
        insertNode_address(root[1],split);
        insertNode_size(root[0],split);
      }
    return node;
  }
}
void *cs550_malloc(size_t size)
{
	if(size==0)
    		return NULL;
  	size_t allocated = roundoff(size);
  	Node *request=NULL;

	//search freelist
  	if(root[0]!=NULL)//if NULL implies both list being empty
	{
		request=free_list(root[0],allocated);
	}
  	//if no appopriate block found
  	if(request==NULL)
  	{
      		request = sbrk(allocated+sizeof(Node));
      	//	printf("allocated by sbrk:%zd at %d\n",allocated,request);
//		      printf(" at:%d \n",request+sizeof(Node));
		      request->req_size=size;
      		request->size=roundoff(size);
      		return request+sizeof(Node);
  	}
    request->req_size=size;
  	return request+sizeof(Node);
}
//////////////////////////////////free operation//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////||||||||||||||||Insert by Size////////////////////////////////////
void insertNode_size(Node *node,Node *to_insert)
{
	if(to_insert->size < node->size)
	{
		if(node->links[0].left!=NULL)
               		insertNode_size(node->links[0].left,to_insert);
		else
		        node->links[0].left=to_insert;
	}
	else if(to_insert->size >= node->size)
     	  {
                if(node->links[0].right!=NULL)
                        insertNode_size(node->links[0].right,to_insert);
                else
                        node->links[0].right=to_insert;
	  }
}
int merge_stat=0;
////||||||||||||||||Insert by Address////////////////////////////////////
void insertNode_address(Node *node,Node *to_insert)
{
	if(node>to_insert)
	{
   if((char *)node==(char *)to_insert+to_insert->size+sizeof(Node))
   { 
   //    |to_insert||node|
      merge_stat=1;
      to_insert->links[0].left=node->links[0].left;//replace to_insert & the node
      to_insert->links[0].right=node->links[0].right;
      to_insert->links[1].left=node->links[1].left;
      to_insert->links[1].right=node->links[1].right;
      to_insert->size=to_insert->size+sizeof(Node)+node->size;
      Node *Parent=search_Parent(root[0],node,0);
      if(node==root[0])
        root[0]=to_insert;
      if(node==root[1])
        root[1]=to_insert;
      if(Parent!=NULL)  //update parent link in BST 1
        if(Parent->links[0].left==node)
          Parent->links[0].left=to_insert;
        else
          Parent->links[0].right=to_insert;
      Parent=search_Parent(root[1],node,1);
      
      if(Parent!=NULL)  //update parent link in BST 2
        if(Parent->links[1].left==node)
          Parent->links[1].left=to_insert;
        else
          Parent->links[1].right=to_insert;
      
      if((char *)Parent==(char *)to_insert+to_insert->size+sizeof(Node))//3 node merge
      {
        // |Parent||to_insert||node|
        Parent->size=Parent->size+sizeof(Node)+node->size;
        promotion_size(to_insert);
        promotion_address(to_insert);
      }
   }
	  else if(node->links[1].left!=NULL)
			insertNode_address(node->links[1].left,to_insert);
		else
			node->links[1].left=to_insert;
	}
	else if(node<to_insert)
	{
    if((char *)to_insert==(char *)node+node->size+sizeof(Node))
    { //    |node||to_insert|
      merge_stat=1;
      node->size=node->size+sizeof(Node)+to_insert->size;      //perform 2 node merge 
      Node *Parent=search_Parent(root[1],node,1);
      //printf("2 Mergable\n Parent:%d\n",Parent);      
      if(Parent!=NULL)
      {
        if((char *)to_insert==(char *)Parent+Parent->size+sizeof(Node))//3 node merge
        {
          // |node||to_insert||Parent|
          node->size=node->size+sizeof(Node)+Parent->size;
          promotion_size(Parent);
          promotion_address(Parent);
          //printf("2 Mergable\n node:%d Parent:%d\n",(char *)to_insert,(char *)Parent+Parent->size+sizeof(Node));
        }
      }
    }
     else if(node->links[1].right!=NULL)
      insertNode_address(node->links[1].right,to_insert);
     else
      node->links[1].right=to_insert;
  }
}

void *cs550_free(void *ptr)
{
        Node *node=(Node *)ptr-sizeof(Node);
	     node->req_size=0; 
       node->size=roundoff(node->size); 
        if(root[0]==NULL)
        {
		      root[0]=node;
		      root[1]=node;
//	      	printf("root : %d\n",node);
      	}
       else
       {
             insertNode_address(root[1],node);
             if(merge_stat==0)
               insertNode_size(root[0],node);
             merge_stat=0;  
//		      printf("node added of size : %d\n",node->size);
       }
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *cs550_calloc(size_t nmemb, size_t size)
{
  size_t allocated=nmemb*size;
  allocated=roundoff(allocated);
  void *call;
  call=cs550_malloc(allocated);
  memset(call,'0',allocated);
  return call;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *cs550_realloc(void *ptr, size_t size)
{
Node *node=(Node *)ptr-sizeof(Node);
  if(node->size>=size)
  {
    node->req_size=size;
    return ptr;
  }
  else if(node->size<size)
  {
    Node *realloc_node=cs550_malloc(size);
    cs550_free(ptr);
//    memcpy(realloc_node,node,roundoff(size));
    return realloc_node;
  }
}
//struct Block { void *ptr; size_t sz;unsigned char val;};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
