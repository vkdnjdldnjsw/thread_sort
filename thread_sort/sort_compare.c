#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/random.h>
#include<linux/list_sort.h>

extern void thread_sort(void *priv, struct list_head *head,int (*cmp)(void *priv, struct list_head *a, struct list_head *b), int num_of_thread);
extern void thread_sort_depth(void *priv, struct list_head *head, int (*cmp)(void *priv, struct list_head *a, struct list_head *b), int depth, int count);

struct my_node{
    struct list_head list;
    int data;
};
void printAll(struct list_head *head){
    struct list_head *cur;
    int count = 0;
    for(cur = head->next; cur->next != head; cur = cur->next){
        struct my_node *node_cur;
        node_cur = container_of(cur, struct my_node, list);
        printk("%d : %d\n", count, node_cur->data);
        count++;
    }
    struct my_node *node_cur;
    node_cur = container_of(cur, struct my_node, list);
    printk("%d : %d\n", count, node_cur->data);
}

int cmp(void *priv, struct list_head *a, struct list_head *b){
    struct my_node *node_a, *node_b;

    node_a = container_of(a, struct my_node, list);
    node_b = container_of(b, struct my_node, list);
    return node_a->data - node_b->data;
}
void freeAll(struct list_head *my_list){
    struct my_node *tmp, *current_node;
    list_for_each_entry_safe(current_node, tmp, my_list, list){
        list_del(&current_node->list);
        kfree(current_node);
    }
}

void struct_process(int item_n, int thread_num, int depth){
    struct list_head list1;
    struct list_head list2[8];
    struct list_head list3[3];
    int i, j;
    
    INIT_LIST_HEAD(&list1);
    for(i = 0; i < thread_num; i++){
    	INIT_LIST_HEAD(&list2[i]);
    }
    for(i = 0; i < thread_num; i++){
    	INIT_LIST_HEAD(&list3[i]);
    }
    struct timespec t0, t1;
    for(i = 0; i<item_n; i++){
        struct my_node *new1 = kmalloc(sizeof(struct my_node), GFP_KERNEL);
	new1->data = prandom_u32() % item_n;
        list_add(&new1->list, &list1);
	for(j = 0; j < thread_num; j++){
        	struct my_node *new2 = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        	new2->data = new1->data;
        	list_add(&new2->list, &list2[j]);
	}
	for(j = 0; j < depth; j++){
        	struct my_node *new3 = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        	new3->data = new1->data;
        	list_add(&new3->list, &list3[j]);
	}
    }
    printk("%d",item_n);
    getnstimeofday(&t0);
    list_sort(NULL, &list1, cmp);
    getnstimeofday(&t1);
    printk("%u.%8u", t1.tv_sec - t0.tv_sec, t1.tv_nsec - t0.tv_nsec);
   
    for(i = 1; i <= thread_num; i++){
    	getnstimeofday(&t0);
    	thread_sort(NULL, &list2[i-1], cmp, i);
    	getnstimeofday(&t1);
    	printk("%u.%8u", t1.tv_sec - t0.tv_sec, t1.tv_nsec - t0.tv_nsec);
    }

    for(i = 1; i <= depth; i++){
    	getnstimeofday(&t0);
    	thread_sort_depth(NULL, &list3[i-1], cmp, i, item_n);
    	getnstimeofday(&t1);
    	printk("%u.%8u", t1.tv_sec - t0.tv_sec, t1.tv_nsec - t0.tv_nsec);
    }
    printk("!");
    freeAll(&list1);
    for(i = 1; i <= thread_num; i++){
    	freeAll(&list2[i-1]);
    }
    for(i = 1; i <= depth; i++){
    	freeAll(&list3[i-1]);
    }
}

int __init my_link_init(void){
    int i;
    int repeat_count = 30;
    int node_num = 1000000;
    for(i = 0; i < repeat_count; i ++){
    	struct_process(node_num,8,3);
    }
    return 0;
}

void __exit my_link_cleanup(void){
    printk("Bye my link!\n");
}

module_init(my_link_init);
module_exit(my_link_cleanup);
MODULE_LICENSE("GPL");
