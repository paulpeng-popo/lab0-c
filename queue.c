#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
    }
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, l) {
        q_release_element(list_entry(node, element_t, list));
    }
    free(l);
}

static inline bool q_insert(struct list_head *head,
                            char *s,
                            void (*op)(struct list_head *, struct list_head *))
{
    if (!head || !s) {
        return false;
    }

    element_t *entry = malloc(sizeof(element_t));
    if (!entry) {
        return false;
    }

    size_t len = strlen(s) + 1;
    entry->value = malloc(len);
    if (!entry->value) {
        free(entry);
        return false;
    }

    strncpy(entry->value, s, len);
    (entry->value)[len - 1] = '\0';
    op(&entry->list, head);

    return true;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    return q_insert(head, s, list_add);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert(head, s, list_add_tail);
}

#define q_remove(suffix, list_api)                                 \
    element_t *q_remove_##suffix(struct list_head *head, char *sp, \
                                 size_t bufsize)                   \
    {                                                              \
        if (!head || list_empty(head))                             \
            return NULL;                                           \
        element_t *entry = list_api(head, element_t, list);        \
        list_del_init(&entry->list);                               \
        if (sp) {                                                  \
            strncpy(sp, entry->value, bufsize);                    \
            sp[bufsize - 1] = '\0';                                \
        }                                                          \
        return entry;                                              \
    }

/* Remove an element from head of queue */
q_remove(head, list_first_entry);

/* Remove an element from tail of queue */
q_remove(tail, list_last_entry);

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }

    int len = 0;
    struct list_head *node;
    list_for_each (node, head) {
        len++;
    }

    return len;
}

static void _find_mid(struct list_head **mid, struct list_head *head)
{
    *mid = head->next;
    struct list_head *fast = head->next->next;
    while (fast != head && fast && fast->next != head && fast->next) {
        *mid = (*mid)->next;
        fast = fast->next->next;
    }
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *mid = NULL;
    _find_mid(&mid, head);

    // delete the node pointed by mid
    list_del(mid);
    element_t *entry = list_entry(mid, element_t, list);
    q_release_element(entry);

    return true;
}

static inline int cmpstr(const void *p1, const void *p2)
{
    element_t *first =
        list_entry(*(const struct list_head **) p1, element_t, list);
    element_t *second =
        list_entry(*(const struct list_head **) p2, element_t, list);
    return strcmp(first->value, second->value);
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return false;
    }

    bool dup = false;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (safe != head && !cmpstr(&node, &safe)) {
            list_del(node);
            q_release_element(list_entry(node, element_t, list));
            dup = true;
        } else if (dup) {
            list_del(node);
            q_release_element(list_entry(node, element_t, list));
            dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    LIST_HEAD(reverse_list);
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, &reverse_list);
    }

    list_splice_init(&reverse_list, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k <= 1) {
        return;
    }

    int count = 0;
    LIST_HEAD(rlist);
    LIST_HEAD(tmp);
    struct list_head *node, *safe;
    struct list_head *start = head;

    list_for_each_safe (node, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&rlist, start, node);
            q_reverse(&rlist);
            list_splice_tail_init(&rlist, &tmp);
            start = safe->prev;
            count = 0;
        }
    }
    list_splice_init(&tmp, head);
}

struct list_head *merge_two_lists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; L1 && L2; *node = (*node)->next) {
        if (cmpstr(&L1, &L2) < 0)
            node = &L1;
        else
            node = &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }

    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

struct list_head *merge_sort(struct list_head *list)
{
    if (!list || !list->next) {
        return list;
    }

    LIST_HEAD(head);
    head.next = list;

    struct list_head *slow = NULL, *mid = NULL;
    _find_mid(&slow, &head);
    mid = slow->next;
    slow->next = NULL;

    struct list_head *left = merge_sort(list);
    struct list_head *right = merge_sort(mid);
    return merge_two_lists(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    head->prev->next = NULL;
    head->prev = NULL;
    head->next = merge_sort(head->next);

    struct list_head *prev = head, *cur = head->next;
    while (cur != NULL) {
        cur->prev = prev;
        cur = cur->next;
        prev = prev->next;
    }

    prev->next = head;
    head->prev = prev;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }

    if (list_is_singular(head)) {
        return 1;
    }

    struct list_head *node = head->prev;
    struct list_head *pnode = node->prev;
    char *max = NULL;

    for (; node != head; node = pnode) {
        element_t *entry = list_entry(node, element_t, list);
        pnode = node->prev;
        if (!max || strcmp(entry->value, max) > 0) {
            max = entry->value;
        } else {
            list_del(node);
            q_release_element(entry);
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    if (list_is_singular(head)) {
        return list_first_entry(head, queue_contex_t, chain)->size;
    }

    queue_contex_t *qhead = list_first_entry(head, queue_contex_t, chain);
    list_del_init(&qhead->chain);
    queue_contex_t *cur = NULL;

    list_for_each_entry (cur, head, chain) {
        list_splice_init(cur->q, qhead->q);
        qhead->size += cur->size;
    }

    list_add(&qhead->chain, head);
    q_sort(qhead->q);

    return qhead->size;
}
