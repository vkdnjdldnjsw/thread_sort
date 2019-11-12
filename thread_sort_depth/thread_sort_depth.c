#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/list_sort.h>
#include<linux/kthread.h>


void thread_sort_depth(void *priv, struct list_head *head, int (*cmp)(void *priv, struct list_head *a, struct list_head *b), int depth, int count);

void merge(void *priv,int (*cmp)(void *priv, struct list_head *a, struct list_head *b),struct list_head *head, struct list_head *a, struct list_head *b){
    struct list_head *tail = head;
    struct list_head *head_a = a;
    struct list_head *head_b = b;
    a = a->next;
    b = b->next;
    while(a != head_a && b!= head_b){
        if(cmp(priv, a,b) <= 0){
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
        }else{
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
        }
    }
    if(b == head_b && a != head_a){
        tail->next = a;
        a->prev = tail;
        tail = head_a->prev;
    }else if(a == head_a && b != head_b){
        tail->next = b;
        b->prev = tail;
        tail = head_b->prev;
    }
    tail->next = head;
    head->prev = tail;
}
struct thread_sort_info{
    bool is_done;
    struct list_head *head;
    int (*cmp)(void *priv, struct list_head *a, struct list_head *b);
    int depth;
    void *priv;
};
int do_sort(void *_arg){
    struct thread_sort_info *info = (struct thread_sort_info *)_arg;
	if (info->depth <= 1) {
    	list_sort(info->priv, info->head, info->cmp);
	} else {
		thread_sort_depth(info->priv, info->head, info->cmp, info->depth);
	}
    info->is_done = true;
}

void thread_sort_depth(void *priv, struct list_head *head,int (*cmp)(void *priv, struct list_head *a, struct list_head *b), int depth, int count){
    struct list_head my_list;
    INIT_LIST_HEAD(&my_list);
    struct list_head *cur;

    struct list_head *list_heads = kmalloc(sizeof(struct list_head) * 2, GFP_KERNEL);
    struct thread_sort_info *thread_infos = kmalloc(sizeof(struct thread_sort_info) * 2, GFP_KERNEL);

    int i;
	int j = 0;
    cur = head;
    struct list_head *first = cur->next;
    struct list_head *last = cur->prev;
    struct list_head *middle;
    list_heads[0].next = first;
    first->prev = &list_heads[0];
    list_heads[1].prev = last;
    last->next = &list_heads[1];
    cur = first;
    for(i = 1; i < count/2; i++) {
        cur = cur->next;
    }
    middle = cur->next;
    list_heads[0].prev = cur;
    cur->next = &list_heads[0];
    list_heads[1].next = middle;
    middle->prev = &list_heads[1];
    for(i = 0; i < 2; i++) {
        thread_infos[i].head = &list_heads[i];
        thread_infos[i].is_done = false;
        thread_infos[i].cmp = cmp;
		thread_infos[i].depth = depth-1;
        thread_infos[i].priv = priv;
        kthread_run(&do_sort, (void *)&thread_infos[i], "sort_thread");
    }
    head->next = head;
    head->prev = head;
    for( i = 0; i< 2; i++){
        while(!thread_infos[i].is_done){
            schedule();
        }
		thread_infos[i].is_done = true;
    }
	merge(priv, cmp, head, &list_heads[0], &list_heads[1]);
    kfree(thread_infos);
    kfree(list_heads);
}


MODULE_LICENSE("GPL");
EXPORT_SYMBOL(thread_sort_depth);
