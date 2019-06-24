struct list_elem *list_nth_elem(struct list *foo_list, int n) {
    int i;
	DEBUG2("list_nth_elem start (params : %p, %d)", foo_list, n);
    struct list_elem* tmp;
	
	if (list_empty(foo_list)) {
		//ASSERT(n == 0);
		return list_begin(foo_list);
	}

	tmp = list_begin(foo_list);
	if (n == 0) {
        DEBUG1("list_nth_elem end (return : %p)", tmp);
        return tmp;
    }
    for(i = 0; i < n; i++) {
        tmp = tmp->next;
    }
    DEBUG1("list_nth_elem end (return : %p)", tmp);
    return tmp;
}

list_less_func l_less_func;

bool l_less_func(const struct list_elem *a,
	const struct list_elem *b,
	void *aux) {
	int aa, bb;
	aa = (list_entry(a, struct foo_list_elem, elem))->data;
	bb = (list_entry(b, struct foo_list_elem, elem))->data;
	if (aa < bb)	return true;
	else			return false;
}

hash_less_func h_less_func;

bool h_less_func(const struct hash_elem *a,
	const struct hash_elem *b, 
	void *aux) {
	int aa, bb;
	aa = (hash_entry(a, struct foo_hash_elem, elem))->data;
	bb = (hash_entry(b, struct foo_hash_elem, elem))->data;
	if (aa < bb)		return true;
	else				return false;
}

hash_hash_func h_hash_func;

unsigned h_hash_func(const struct hash_elem *e, void *aux) {
	DEBUG1("in h_hash_func, e : %p", e);
	DEBUG1("in h_hash_func, hash : %p", (hash_entry(e, struct foo_hash_elem, elem))->data);
	return hash_int((hash_entry(e, struct foo_hash_elem, elem))->data);
}

hash_action_func hash_destructor;

void hash_destructor(struct hash_elem *e, void *aux) {
	struct foo_hash_elem *f;
	f = hash_entry(e, struct foo_hash_elem, elem);
	free(f);
}

hash_action_func hash_square;

void hash_square(struct hash_elem *e, void *aux) {
	struct foo_hash_elem *f;
	f = hash_entry(e, struct foo_hash_elem, elem);
	f->data = f->data * f->data;
}

hash_action_func hash_triple;

void hash_triple(struct hash_elem *e, void *aux) {
	struct foo_hash_elem *f;
	f = hash_entry(e, struct foo_hash_elem, elem);
	f->data = f->data * f->data * f->data;
}

struct list_elem *list_find_right(struct list_elem *a, struct list_elem *b) 
{
    struct list_elem *i;
    if (a == b)     return a;
    if (a->next == NULL)   return a;
    for(i = a->next; i != NULL; i = i->next) {
        if (i == b)
            return b;
    }
    return a;
}

void list_swap(struct list_elem *a, struct list_elem *b) {
	struct list_elem* first, *last, *first_prev, *last_next;
	//ASSERT(a != NULL && b != NULL);
    DEBUG2("list_swap start (params : %p, %p)", a, b);

    if(a == b)  return;
    
    last = list_find_right(a, b);
    if (last == b)  first = a;
    else            first = b;
    first_prev = first->prev;
    last_next = last->next;
    list_remove(first);
    list_remove(last);
    list_insert(last_next, first);
    list_insert(first_prev->next, last);

    DEBUG2("list_swap end (params : %p, %p)", a, b);
    return;
}

void list_shuffle(struct list *l) {
    int i, b;
//    printf("%d\n", list_size(l));
    struct list_elem *le1, *le2;
    for(i = 0; i < list_size(l); i++) {
        b = rand();
        //printf("%d\n", b);
        b = b % list_size(l);
        //printf("%d\n", b);
        DEBUG1("in list_shuffle, b = %d", b);
        le1 = list_nth_elem(l, i);
        le2 = list_nth_elem(l, b);
        //printf("%p %p\n", le1, le2);
//        b = rand() % list_size(l);
//        printf("%d /", b);
        list_swap(le1, le2);
    }
}
