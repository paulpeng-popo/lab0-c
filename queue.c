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
    if (!head) {
        return NULL;
    }

    INIT_LIST_HEAD(head);
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
        element_t *entry = list_entry(node, element_t, list);
        q_release_element(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *entry = malloc(sizeof(element_t));
    if (!entry) {
        return false;
    }

    size_t len = strlen(s) + 1;
    entry->value = malloc(sizeof(char) * len);
    if (!entry->value) {
        free(entry);
        return false;
    }

    strncpy(entry->value, s, len - 1);
    (entry->value)[len - 1] = '\0';
    list_add(&entry->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *entry = malloc(sizeof(element_t));
    if (!entry) {
        return false;
    }

    size_t len = strlen(s) + 1;
    entry->value = malloc(sizeof(char) * len);
    if (!entry->value) {
        free(entry);
        return false;
    }

    strncpy(entry->value, s, len - 1);
    (entry->value)[len - 1] = '\0';
    list_add_tail(&entry->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *entry = list_first_entry(head, element_t, list);
    list_del_init(&entry->list);

    if (sp) {
        strncpy(sp, entry->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *entry = list_last_entry(head, element_t, list);
    list_del_init(&entry->list);

    if (sp) {
        strncpy(sp, entry->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return entry;
}

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

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *front = head->next;
    struct list_head *back = head->prev;
    while (front != back && front->next != back) {
        front = front->next;
        back = back->prev;
    }

    // delete the node which is pointed by front
    list_del_init(front);
    element_t *entry = list_entry(front, element_t, list);
    q_release_element(entry);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    // only valid for sorted list

    // empty and singular should do no action
    if (!head || list_empty(head) || list_is_singular(head)) {
        return false;
    }

    bool dup = false;
    struct list_head *del_list = q_new();
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (&safe->list != head && !strcmp(entry->value, safe->value)) {
            list_move(&entry->list, del_list);
            dup = true;
        } else if (dup) {
            list_move(&entry->list, del_list);
            dup = false;
        }
    }
    q_free(del_list);

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
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
        list_del(node);
        list_add(node, &reverse_list);
    }

    list_splice(&reverse_list, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
