#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/random.h>
#include<linux/list_sort.h>

extern void thread_sort_depth(void *priv, struct list_head *head,int (*cmp)(void *priv, struct list_head *a, struct list_head *b), int depth, int count);

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

void struct_process(int item_n, int depth){
    struct list_head list1;
    struct list_head list2;
    INIT_LIST_HEAD(&list1);
    INIT_LIST_HEAD(&list2);
    int i;
    struct timespec t0, t1;
    for(i = 0; i<item_n; i++){
        struct my_node *new1 = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        struct my_node *new2 = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        new1->data = prandom_u32() % item_n;
        new2->data = new1->data;
        list_add(&new1->list, &list1);
        list_add(&new2->list, &list2);
    }
    getnstimeofday(&t0);
    list_sort(NULL, &list1, cmp);
    getnstimeofday(&t1);
    printk("list_sort with n items takes %u secs %u nsecs\n", t1.tv_sec - t0.tv_sec, t1.tv_nsec - t0.tv_nsec);
    getnstimeofday(&t0);
    thread_sort_depth(NULL, &list2, cmp, depth, item_n);
    getnstimeofday(&t1);
    printk("our_sort with n items takes %u secs %u nsecs\n", t1.tv_sec - t0.tv_sec, t1.tv_nsec - t0.tv_nsec);
    //printAll(&list1);
    //printAll(&list2);
    freeAll(&list1);
    freeAll(&list2);
}

int __init my_link_init(void){
    struct_process(1000000,2);
    return 0;
}

void __exit my_link_cleanup(void){
    printk("Bye my link!\n");
}

module_init(my_link_init);
module_exit(my_link_cleanup);
MODULE_LICENSE("GPL");
