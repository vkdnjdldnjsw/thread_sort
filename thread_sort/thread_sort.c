#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/list_sort.h>
#include<linux/kthread.h>


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
    void *priv;
};
int do_sort(void *_arg){
    struct thread_sort_info *info = (struct thread_sort_info *)_arg;
    list_sort(info->priv, info->head, info->cmp);
    info->is_done = true;
}

void thread_sort(void *priv, struct list_head *head,int (*cmp)(void *priv, struct list_head *a, struct list_head *b), int num_of_thread){
    struct list_head my_list;
    INIT_LIST_HEAD(&my_list);
    struct list_head *cur;
    int count = 0;
    for(cur = head->next; cur->next != head; cur = cur->next){
        count++;
    }
    if(head->next != head){
        count++;
    }

    struct list_head *list_heads = kmalloc(sizeof(struct list_head) * num_of_thread, GFP_KERNEL);
    struct thread_sort_info *thread_infos = kmalloc(sizeof(struct thread_sort_info) * num_of_thread, GFP_KERNEL);

    int sliceNum = count / num_of_thread;
    int left = count % num_of_thread;
    int i, j;
    cur = head;
    struct list_head *first = cur->next;
    for(i = 0; i < num_of_thread; i++){
        list_heads[i].next = first;
        first->prev = &list_heads[i];
        if(i == num_of_thread -1){
            list_heads[i].prev = head->prev;
            head->prev->next = &list_heads[i];
        } else{
            for(j = 0; j < sliceNum; j++){
                if(cur == NULL){
                    cur = first;
                } else{
                    cur = cur->next;
                }
            }
            if(left > 0){
                if(cur == NULL){
                    cur = first;
                } else{
                    cur = cur->next;
                }
                left--;
            }
            first = cur->next;
            cur->next = &list_heads[i];
            list_heads[i].prev = cur;
            cur = NULL;
        }
        thread_infos[i].head = &list_heads[i];
        thread_infos[i].is_done = false;
        thread_infos[i].cmp = cmp;
        thread_infos[i].priv = priv;
        kthread_run(&do_sort, (void *)&thread_infos[i], "sort_thread");
    }
    head->next = head;
    head->prev = head;
    for( i = 0; i< num_of_thread; i++){
        while(!thread_infos[i].is_done){
            schedule();
        }
        merge(priv,cmp, head, head, thread_infos[i].head);
    }
    kfree(thread_infos);
    kfree(list_heads);
}

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(thread_sort);
