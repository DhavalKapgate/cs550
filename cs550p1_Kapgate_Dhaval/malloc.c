#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#ifdef CS550_RENAME
    #define cs550_malloc malloc
    #define cs550_free free
    #define cs550_calloc calloc
    #define cs550_realloc realloc
#endif
/////////////////////////////////////////////////BST structures///////////////////////////////////////////////////////////
struct Metadata;
typedef struct Link
{
      	struct Metadata *parent;
        struct Metadata *left;
        struct Metadata *right;
}Links;
typedef struct Metadata			//size 64
{
        struct Link links[2];//0 for size  1 for address
	      size_t req_size;
        size_t size;
}Node;
//initial BST state
Node *root[2]={NULL,NULL};
//////////////////////////////////////////////////BST operations///////////////////////////////////////////////////////////
void *free_list(Node *node,size_t size);

size_t roundoff(size_t size);

void shortCircuit_size(Node * node);
void removeLeaf_size(Node *node);
void promotion_size(Node * node);
void insertNode_size(Node *node,Node *to_insert);

void shortCircuit_address(Node * node);
void removeLeaf_address(Node *node);
void promotion_address(Node * node);
void insertNode_address(Node *node,Node *to_insert);

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
//////////////////////////////////COMMON TO BOTH FREE AND MALLOC//////////////////////////
size_t roundoff(size_t size)  //convert to next multiple of 8
{
	while(size%8!=0)
		size++;
  	return size;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Node * search_Parent_size(Node *node,Node *target)
{
 	if(target->size < node->size && node->links[0].left!=NULL)
 	{
   		node=node->links[0].left;
   		if((node)==target)
			return node;
   		else
      		search_Parent_size(node,target);
 	}
  	else if(node->links[0].right!=NULL)
  	{
    		node=node->links[0].right;
    		if((node)==target)
			return node;
    		else
    			search_Parent_size(node,target);
  	}
  	return node;
}
Node * search_Parent_add(Node *node,Node *target)
{
 	if(target< node&& node->links[0].left!=NULL)
 	{
   		node=node->links[0].left;
   		if((node)==target)
			return node;
   		else
      			search_Parent_add(node,target);
 	}
  	else if(node->links[0].right!=NULL)
  	{
    		node=node->links[0].right;
    		if((node)==target)
			return node;
    		else
    			search_Parent_add(node,target);
  	}
  	return node;
}
//////////////////////////////////////malloc operation/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//||||||||||||||||||||||||||||||||BOTH SIZE AND ADDRESS|||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||SIZE||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void shortCircuit_size(Node * node)
{
	if(root[0]!=node)    //if node is not root
	{
      		Node *Parent;
        	Parent=node->links[0].parent;
        	if(Parent->links[0].left==node)//(node)<-(Parent)
          	{
         	 	if(node->links[0].left!=NULL)//(node.left)<-(node)<-(Parent)
			{
				Parent->links[0].left=node->links[0].left;
              			node->links[0].left->links[0].parent=Parent;
              			node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
			}
			else                        ////(node.right)<-(node)<-(Parent)
			{
        	        	Parent->links[0].left=node->links[0].right;
                     		node->links[0].right->links[0].parent=Parent;
                    		node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
 			}
          	}
		else if(Parent->links[0].right==node)//(Parent)->(node)
		{
            		if(node->links[0].left!=NULL)  //(Parent)->(node)->(node.left)
            		{
				Parent->links[0].right=node->links[0].left;
                		node->links[0].left->links[0].parent=Parent;
                		node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
		      	}
		      	else                            ////(Parent)->(node)->(node.right)
		      	{
               	         	Parent->links[0].right=node->links[0].right;
                       		node->links[0].right->links[0].parent=Parent;
              			node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
	        	}
      		}
	}
	else								//incase node is root and one branched
	{
		if(node->links[0].right!=NULL)//([root]node)->(node.right)
		{
			root[0]=node->links[0].right;
      			node->links[0].right->links[0].parent=NULL;
      			node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
		}
		else if(node->links[0].left!=NULL)//(node.left)<-([root]node)
		{
			root[0]=node->links[0].left;
      			node->links[0].left->links[0].parent=NULL;
      			node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
		}
	}
}
void removeLeaf_size(Node *node)
{
	if(root[0]!=node)
	{
		Node *Parent;
		Parent=node->links[0].parent;
		if(Parent!=NULL)
    		{
        		if(Parent->links[0].left==node)
      			  	Parent->links[0].left=NULL;
        		if(Parent->links[0].right==node)
          			Parent->links[0].right=NULL;
             		node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
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
		removeLeaf_size(node);
	else if(node->links[0].left==NULL || node->links[0].right==NULL)		//if 1 branch
		shortCircuit_size(node);
	else
	{
		Node *successor=node->links[0].right;
     		while(successor->links[0].left!=NULL)
       			successor=successor->links[0].left;
		Node *succ_parent=successor->links[0].parent;

	    	if(successor!=node->links[0].right&&succ_parent->links[0].left!=successor) //check parent link for anamoality
      			succ_parent=search_Parent_size(node,successor);
    		if(root[0]==node)						//if root is to be deleted
			root[0]=successor;

		if(successor->links[0].left!=NULL)				//managing left node of node being deleted
			if(successor->links[0].left->links[0].parent==node)
				successor->links[0].left->links[0].parent=successor;

		Node *succ_right=successor->links[0].right;
		if(successor->links[0].right!=NULL)				//managing right node of node being deleted
			if(successor->links[0].right->links[0].parent==node)
		  	    successor->links[0].right->links[0].parent=successor;

	    	if(node->links[0].left!=successor)				//this check is to avoid linking the successor to itself
		{
        		successor->links[0].left=node->links[0].left;
        		node->links[0].left->links[0].parent=successor;
       		}
		if(node->links[0].right!=successor)				//this check is to avoid linking the successor to itself
		{
			successor->links[0].right=node->links[0].right;
      			node->links[0].right->links[0].parent=successor;
      		}
		if(succ_parent!=NULL)
		{
      			if(succ_parent->links[0].left==successor)
        			succ_parent->links[0].left=succ_right;
      			if(succ_parent->links[0].right==successor)
        			succ_parent->links[0].right=NULL;
     		}

   		successor->links[0].parent=node->links[0].parent;
    		if(node->links[0].parent!=NULL)
    		{
    			if(node->links[0].parent->links[0].left==node)
        			node->links[0].parent->links[0].left=successor;
    			else if(node->links[0].parent->links[0].right==node)
        			node->links[0].parent->links[0].right=successor;
    		}
		node->links[0].right=node->links[0].left=node->links[0].parent=NULL;
  	}
}
//||||||||||||||||||||||||||||||||||||ADDRESS||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void shortCircuit_address(Node * node)
{
	if(root[1]!=node)
	{
		Node *Parent;
 	  	Parent=node->links[1].parent;
 	  	if(Parent->links[1].left==node)
    		{
    			if(node->links[1].left!=NULL)
	    		{
        			Parent->links[1].left=node->links[1].left;
        			node->links[1].left->links[1].parent=Parent;
        			node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
     			}
			else
			{
        			Parent->links[1].left=node->links[1].right;
        			node->links[1].right->links[1].parent=Parent;
        			node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
       			}
   		}
		else if(Parent->links[1].right==node)
		{
      			if(node->links[1].left!=NULL)
	    		{
        			Parent->links[1].right=node->links[1].left;
        			node->links[1].left->links[1].parent=Parent;
        			node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
        		}
			else
			{
       	 			Parent->links[1].right=node->links[1].right;
        			node->links[1].right->links[1].parent=Parent;
              			node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
        		}
	  	}
  	}
	else								//incase node is root and one branched
	{
		if(node->links[1].right!=NULL)
		{
      			root[1]=node->links[1].right;
      			node->links[1].right->links[1].parent=NULL;
      			node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
    		}
		else if(node->links[1].left!=NULL)
		{
      			root[1]=node->links[1].left;
      			node->links[1].left->links[1].parent=NULL;
      			node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
    		}
	}
}

void removeLeaf_address(Node *node)
{
	if(root[1]!=node)
	{
		Node *Parent;
		Parent=node->links[1].parent;
		if(Parent!=NULL)
    		{
        		if(Parent->links[1].left==node)
			  	Parent->links[1].left=NULL;
        		if(Parent->links[1].right==node)
          			Parent->links[1].right=NULL;
             		node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
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
		Node *successor=node->links[1].right;
   		while(successor->links[1].left!=NULL)
       			successor=successor->links[1].left;

		Node *succ_parent=successor->links[1].parent;
   		if(successor!=node->links[1].right&&succ_parent->links[1].left!=successor)
    			succ_parent=search_Parent_add(node,successor);
		if(root[1]==node)						//if root is to be deleted
			root[1]=successor;

		Node *succ_right=successor->links[1].right;
		if(successor->links[1].left!=NULL)				//managing left node of node being deleted
			if(successor->links[1].left->links[1].parent==node)
				successor->links[1].left->links[1].parent=successor;

		if(successor->links[1].right!=NULL)				//managing right node of node being deleted
			if(successor->links[1].right->links[1].parent==node)
  	    			successor->links[1].right->links[1].parent=successor;

    		if(node->links[1].left!=successor)				//this check is to avoid linking the successor to itself
		{
			successor->links[1].left=node->links[1].left;
		  	node->links[1].left->links[1].parent=successor;
       		}
    		if(node->links[1].right!=successor)				//this check is to avoid linking the successor to itself
		{
			successor->links[1].right=node->links[1].right;
      			node->links[1].right->links[1].parent=successor;
       		}
   		if(succ_parent!=NULL)
		{
      			if(succ_parent->links[1].left==successor)
        			succ_parent->links[1].left=succ_right;
      			if(succ_parent->links[1].right==successor)
        			succ_parent->links[1].right=NULL;
     		}
		successor->links[1].parent=node->links[1].parent;
    		if(node->links[1].parent!=NULL)
    		{
    			if(node->links[1].parent->links[1].left==node)
        			node->links[1].parent->links[1].left=successor;
    			else if(node->links[1].parent->links[1].right==node)
        			node->links[1].parent->links[1].right=successor;
    		}
    	node->links[1].right=node->links[1].left=node->links[1].parent=NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void *free_list(Node *node,size_t size)//split 
{
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
	   	//delete the node from both BSTs
	   	promotion_size(node);
	   	promotion_address(node);
		node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
     		node->links[1].left=node->links[1].right=node->links[1].parent=NULL;
     		return node;
  	}
  	else//split
  	{
      	//delete node
      		promotion_size(node);
      		promotion_address(node);
      		node->links[0].left=node->links[0].right=node->links[0].parent=NULL;
      		node->links[1].left=node->links[1].right=node->links[1].parent=NULL;

      		//we split the node into one to be allocated and other to be reinserted
      		void *splitt=(char *)node+size+sizeof(Node);
      		Node *split=splitt;
      		split->size=(node->size)-size-sizeof(Node);
      		node->size=size;
      		split->links[0].left=split->links[0].right=split->links[0].parent=NULL;
      		split->links[1].left=split->links[1].right=split->links[1].parent=NULL;

      		if(root[0]==NULL)
      		{
			root[0]=root[1]=split;
		}
      		else
      		{
        		insertNode_address(root[1],split);
        		insertNode_size(root[0],split);
      		}
    		return node;
  	}
}
void *malloc(size_t size)
{
  	if(size==0)
    		return NULL;
  	size_t allocated = roundoff(size);
  	Node *request=NULL;

  	//search freelist
  	if(root[0]!=NULL)//if NULL implies both list being empty
		request=free_list(root[0],allocated);
  	//if no appopriate block found
  	if(request==NULL)
  	{
      		request = sbrk(allocated+sizeof(Node));
    		request->req_size=size;
		request->size=roundoff(size);
      		return (char *)request+sizeof(Node);
  	}
	request->req_size=size;
  	return (char *)request+sizeof(Node);
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
		{
      			node->links[0].left=to_insert;
      			to_insert->links[0].parent=node;
  		}
	}
	else if(to_insert->size >= node->size)
     	  {
                if(node->links[0].right!=NULL)
                        insertNode_size(node->links[0].right,to_insert);
                else{
                        node->links[0].right=to_insert;
                        to_insert->links[0].parent=node;
                }
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
      			merge_stat=21;
      			to_insert->size=to_insert->size+sizeof(Node)+node->size;
      			//BST 1
			promotion_size(node);
			if(root[0]!=NULL)
				insertNode_size(root[0],to_insert);
			else
				root[0]=to_insert;

			//BST 2
			promotion_address(node);
			if(root[1]!=NULL)
				insertNode_address(root[1],to_insert);
			else
				root[1]=to_insert;

			Node *Parent=node->links[1].parent;
      			if((char *)Parent==(char *)to_insert+to_insert->size+sizeof(Node))//3 node merge
      			{
				merge_stat=31;
        			// |Parent||to_insert||node|
        			Parent->size=Parent->size+sizeof(Node)+node->size;

	          		promotion_size(to_insert);
	          		promotion_size(Parent);
				insertNode_size(root[0],Parent);

        			promotion_address(to_insert);
				promotion_address(Parent);
        			insertNode_address(root[1],Parent);
      			}
   		}
		else if(node->links[1].left!=NULL)
			insertNode_address(node->links[1].left,to_insert);
		else
		{
        		node->links[1].left=to_insert;
        		to_insert->links[1].parent=node;
   		}
	}
	else if(node<to_insert)
	{
    		if((char *)to_insert==(char *)node+node->size+sizeof(Node))
    		{
		 //    |node||to_insert|
      			merge_stat=22;
      			node->size=node->size+sizeof(Node)+to_insert->size;      //perform 2 node merge 
      			//BST 1
      	 		promotion_size(node);
        		if(root[0]!=NULL)
			      	insertNode_size(root[0],node);
			else
				        root[0]=node;
        		//BST 2
        		promotion_address(node);
        		if(root[1]!=NULL)
		          	insertNode_address(root[1],node);
			else
	            		root[1]=node;
			Node *Parent=node->links[1].parent;   
      			if(Parent!=NULL)
      			{
        			if((char *)node==(char *)Parent+Parent->size+sizeof(Node))//3 node merge
        			{merge_stat=32;
          			// [|node||to_insert|][|Parent|]
          			node->size=node->size+sizeof(Node)+Parent->size;          
          			promotion_size(Parent);
          			promotion_address(Parent);
        		}
      		}
    	}
     	else if(node->links[1].right!=NULL)
      		insertNode_address(node->links[1].right,to_insert);
     	else
      		{node->links[1].right=to_insert;to_insert->links[1].parent=node;}
  }
}

void free(void *ptr)
{
	if(ptr!=NULL)
	{
        Node *node=(Node *)((char*)ptr-sizeof(Node));
	node->req_size=0;
	node->size=roundoff(node->size); 
        if(root[0]==NULL)
        {
		root[0]=node;
		root[1]=node;
      	}
       else
       {
             insertNode_address(root[1],node);
             if(merge_stat==0)
             {
               insertNode_size(root[0],node);
             }
             merge_stat=0;
       }
  }
}
void *calloc(size_t nmemb, size_t size)
{
  	size_t allocated=nmemb*size;
  	allocated=roundoff(allocated);
  	void *call;
  	call=malloc(allocated);
  	memset(call,'0',allocated);
 	return call;
}
void *realloc(void *ptr, size_t size)
{
 	Node *node=(Node *)((char*)ptr-sizeof(Node));
  	if(node->size>=size)
  	{
    		node->req_size=size;
    		return (char *)node+sizeof(Node);
  	}
  	else if(node->size<size)
  	{
    		Node *realloc_node=malloc(size);
	 	memcpy(realloc_node,ptr,roundoff(size));
	  	free(ptr);
    		return realloc_node;
  	}
  	return NULL;
}
