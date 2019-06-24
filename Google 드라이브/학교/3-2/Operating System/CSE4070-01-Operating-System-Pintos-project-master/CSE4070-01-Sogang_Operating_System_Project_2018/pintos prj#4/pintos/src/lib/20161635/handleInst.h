void handleListInst(char str[100]) {
	DEBUG1("processInstInList start (param : %s)", str);
    struct foo_list_elem *f;
    struct list *listtmp1, *listtmp2;
	struct list_elem *le1, *le2, *le3;
	void* aux = NULL;
	int tmp1, tmp2;
	char tmpstr1[100], tmpstr2[100], tmpstr3[100];
    if(strcmp(str, "list_insert") == 0) {
        
		scanf("%s", tmpstr1);
        listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
        f = (struct foo_list_elem*)malloc(sizeof(struct foo_list_elem));
		f->elem.next = NULL;
		f->elem.prev = NULL;
        scanf("%d %d", &tmp1, &tmp2);
        f->data = tmp2;
        
        list_insert(list_nth_elem(listtmp1, tmp1), &f->elem);
    }
    else if (strcmp(str, "list_shuffle") == 0) {
        scanf("%s", tmpstr1);
        listtmp1 = (struct list*)(getTableAddressOf(tmpstr1)->ptr);
        list_shuffle(listtmp1);
    }
	else if (strcmp(str, "list_splice") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		le1 = list_nth_elem(listtmp1, tmp1);
		scanf("%s %d %d", tmpstr1, &tmp1, &tmp2);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		le2 = list_nth_elem(listtmp1, tmp1);
		le3 = list_nth_elem(listtmp1, tmp2);
		list_splice(le1, le2, le3);
	}
	else if (strcmp(str, "list_push_front") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		f = (struct foo_list_elem*)malloc(sizeof(struct foo_list_elem));
		f->elem.next = NULL;
		f->elem.prev = NULL;
		scanf("%d", &tmp1);
		f->data = tmp1;

		list_push_front(listtmp1, &f->elem);
	}
	else if (strcmp(str, "list_push_back") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		f = (struct foo_list_elem*)malloc(sizeof(struct foo_list_elem));
		f->elem.next = NULL;
		f->elem.prev = NULL;
		scanf("%d", &tmp1);
		f->data = tmp1;

		list_push_back(listtmp1, &f->elem);
	}
	else if (strcmp(str, "list_remove") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		le1 = list_nth_elem(listtmp1, tmp1);
		list_remove(le1);
	}
	else if (strcmp(str, "list_pop_front") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		list_pop_front(listtmp1);
	}
	else if (strcmp(str, "list_pop_back") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		list_pop_back(listtmp1);
	}
	else if (strcmp(str, "list_front") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		if (!list_empty(listtmp1))
			printf("%d\n", list_entry(list_front(listtmp1), struct foo_list_elem, elem)->data);
	}
	else if (strcmp(str, "list_back") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		if (!list_empty(listtmp1))
			printf("%d\n", list_entry(list_back(listtmp1), struct foo_list_elem, elem)->data);
	}
	else if (strcmp(str, "list_size") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		printf("%d\n", list_size(listtmp1));
	}
	else if (strcmp(str, "list_empty") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		if (list_empty(listtmp1)) {
			printf("true\n");
		}
		else {
			printf("false\n");
		}
	}
	else if (strcmp(str, "list_reverse") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		list_reverse(listtmp1);
	}
	else if (strcmp(str, "list_sort") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		list_sort(listtmp1, &l_less_func, aux);
	}
	else if (strcmp(str, "list_insert_ordered") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		struct foo_list_elem *f = (struct foo_list_elem*)malloc(sizeof(struct foo_list_elem));
		scanf("%d", &tmp1);
		f->elem.next = NULL;
		f->elem.prev = NULL;
		f->data = tmp1;
		list_insert_ordered(listtmp1, &f->elem, &l_less_func, aux);
	}
	else if (strcmp(str, "list_unique") == 0) {
		tmpstr3[0] = '\0';
		scanf("%[^\n]s", tmpstr1);
		DEBUG1("In list_unique, tmpstr1 : %s", tmpstr1);
		sscanf(tmpstr1, "%s %s", tmpstr2, tmpstr3);
		DEBUG2("In list_unique, tmpstr2 : %s, tmpstr3 : %s", tmpstr2, tmpstr3);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr2)->ptr));
		listtmp2 = NULL;
		if (getTableAddressOf(tmpstr3) != NULL) {
			listtmp2 = (struct list*)(getTableAddressOf(tmpstr3)->ptr);
		}
		DEBUG2("In list_unique, ptr1 : %p, ptr2 : %p", listtmp1, listtmp2);
		list_unique(listtmp1, listtmp2, &l_less_func, aux);
	}
	else if (strcmp(str, "list_max") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		if (!list_empty(listtmp1))
			printf("%d\n", list_entry(list_max(listtmp1, &l_less_func, aux), struct foo_list_elem, elem)->data);
	}
	else if (strcmp(str, "list_min") == 0) {
		scanf("%s", tmpstr1);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		if (!list_empty(listtmp1))
			printf("%d\n", list_entry(list_min(listtmp1, &l_less_func, aux), struct foo_list_elem, elem)->data);
	}
	else if (strcmp(str, "list_swap") == 0) {
		scanf("%s %d %d", tmpstr1, &tmp1, &tmp2);
		listtmp1 = ((struct list*)(getTableAddressOf(tmpstr1)->ptr));
		le1 = list_nth_elem(listtmp1, tmp1);
		le2 = list_nth_elem(listtmp1, tmp2);
		DEBUG2("in list_swap, le1 : %p, le2 : %p", le1, le2);
		list_swap(le1, le2);
	}
}

void handleHashInst(char str[100]) {
	DEBUG1("handleHashInst start (param : %s", str);
	struct foo_hash_elem *f;
	char tmpstr1[100], tmpstr2[100];
	int tmp1, tmp2;
	struct hash *hashtmp; struct hash_elem *he1, *he2;

	if (strcmp(str, "hash_insert") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		f = (struct foo_hash_elem*)malloc(sizeof(struct foo_hash_elem));
		f->data = tmp1;
		f->elem.list_elem.next = NULL;
		f->elem.list_elem.prev = NULL;
		hash_insert(hashtmp, &f->elem);
	}
	else if (strcmp(str, "hash_replace") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		f = (struct foo_hash_elem*)malloc(sizeof(struct foo_hash_elem));
		f->data = tmp1;
		f->elem.list_elem.next = NULL;
		f->elem.list_elem.prev = NULL;
		hash_replace(hashtmp, &f->elem);
	}
	else if (strcmp(str, "hash_find") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		f = (struct foo_hash_elem*)malloc(sizeof(struct foo_hash_elem));
		f->data = tmp1;
		f->elem.list_elem.next = NULL;
		f->elem.list_elem.prev = NULL;
		he1 = hash_find(hashtmp, &f->elem);
		if (he1 == NULL) {
			return;
		}
		f = hash_entry(he1, struct foo_hash_elem, elem);
		printf("%d\n", f->data);
	}
	else if (strcmp(str, "hash_delete") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		f = (struct foo_hash_elem*)malloc(sizeof(struct foo_hash_elem));
		f->data = tmp1;
		f->elem.list_elem.next = NULL;
		f->elem.list_elem.prev = NULL;

		hash_delete(hashtmp, &f->elem);
	}
	else if (strcmp(str, "hash_clear") == 0) {
		scanf("%s", tmpstr1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		hash_clear(hashtmp, &hash_destructor);
	}
	else if (strcmp(str, "hash_size") == 0) {
		scanf("%s", tmpstr1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		printf("%d\n", hash_size(hashtmp));
	}
	else if (strcmp(str, "hash_empty") == 0) {
		scanf("%s", tmpstr1);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		if (hash_empty(hashtmp)) {
			printf("true\n");
		}
		else {
			printf("false\n");
		}
	}
	else if (strcmp(str, "hash_apply") == 0) {
		scanf("%s %s", tmpstr1, tmpstr2);
		hashtmp = (struct hash *)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "square") == 0) {
			hash_apply(hashtmp, &hash_square);
		}
		else if (strcmp(tmpstr2, "triple") == 0) {
			hash_apply(hashtmp, &hash_triple);
		}
	}
}

void handleBitmapInst(char str[100]) {
	struct bitmap *bitmaptmp;
	char tmpstr1[100], tmpstr2[100];
	int tmp1, tmp2;

	if (strcmp(str, "bitmap_size") == 0) {
		scanf("%s", tmpstr1);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		printf("%u\n", bitmap_size(bitmaptmp));
	}
	else if (strcmp(str, "bitmap_set") == 0) {
		scanf("%s %d %s", tmpstr1, &tmp1, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0)
			bitmap_set(bitmaptmp, tmp1, true);
		else
			bitmap_set(bitmaptmp, tmp1, false);
	}
	else if (strcmp(str, "bitmap_mark") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		bitmap_mark(bitmaptmp, tmp1);
	}
	else if (strcmp(str, "bitmap_reset") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		bitmap_reset(bitmaptmp, tmp1);
	}
	else if (strcmp(str, "bitmap_flip") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		bitmap_flip(bitmaptmp, tmp1);
	}
	else if (strcmp(str, "bitmap_test") == 0) {
		scanf("%s %d", tmpstr1, &tmp1);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (bitmap_test(bitmaptmp, tmp1)) {
			printf("true\n");
		}
		else
			printf("false\n");
	}
	else if (strcmp(str, "bitmap_set_all") == 0) {
		scanf("%s %s", tmpstr1, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0)
			bitmap_set_all(bitmaptmp, true);
		else
			bitmap_set_all(bitmaptmp, false);
	}
	else if (strcmp(str, "bitmap_set_multiple") == 0) {
		scanf("%s %d %d %s", tmpstr1, &tmp1, &tmp2, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0)
			bitmap_set_multiple(bitmaptmp, tmp1, tmp2, true);
		else
			bitmap_set_multiple(bitmaptmp, tmp1, tmp2, false);
	}
	else if (strcmp(str, "bitmap_count") == 0) {
		scanf("%s %d %d %s", tmpstr1, &tmp1, &tmp2, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0)
			printf("%u\n", bitmap_count(bitmaptmp, tmp1, tmp2, true));
		else
			printf("%u\n", bitmap_count(bitmaptmp, tmp1, tmp2, false));
	}
	else if (strcmp(str, "bitmap_contains") == 0) {
		scanf("%s %d %d %s", tmpstr1, &tmp1, &tmp2, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0) {
			if (bitmap_contains(bitmaptmp, tmp1, tmp2, true))
				printf("true\n");
			else
				printf("false\n");
		}
		else {
			if (bitmap_contains(bitmaptmp, tmp1, tmp2, false))
				printf("true\n");
			else
				printf("false\n");
		}
	}
	else if (strcmp(str, "bitmap_any") == 0) {
		scanf("%s %d %d", tmpstr1, &tmp1, &tmp2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (bitmap_any(bitmaptmp, tmp1, tmp2))
			printf("true\n");
		else
			printf("false\n");
		
	}
	else if (strcmp(str, "bitmap_none") == 0) {
		scanf("%s %d %d", tmpstr1, &tmp1, &tmp2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (bitmap_none(bitmaptmp, tmp1, tmp2))
			printf("true\n");
		else
			printf("false\n");
	}
	else if (strcmp(str, "bitmap_all") == 0) {
		scanf("%s %d %d", tmpstr1, &tmp1, &tmp2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (bitmap_all(bitmaptmp, tmp1, tmp2))
			printf("true\n");
		else
			printf("false\n");
	}
	else if (strcmp(str, "bitmap_scan") == 0) {
		scanf("%s %d %d %s", tmpstr1, &tmp1, &tmp2, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0)
			printf("%u\n", bitmap_scan(bitmaptmp, tmp1, tmp2, true));
		else
			printf("%u\n", bitmap_scan(bitmaptmp, tmp1, tmp2, false));
	}
	else if (strcmp(str, "bitmap_scan_and_flip") == 0) {
		scanf("%s %d %d %s", tmpstr1, &tmp1, &tmp2, tmpstr2);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		if (strcmp(tmpstr2, "true") == 0)
			printf("%u\n", bitmap_scan_and_flip(bitmaptmp, tmp1, tmp2, true));
		else
			printf("%u\n", bitmap_scan_and_flip(bitmaptmp, tmp1, tmp2, false));
	}
	else if (strcmp(str, "bitmap_dump") == 0) {
		scanf("%s", tmpstr1);
		bitmaptmp = (struct bitmap*)(getTableAddressOf(tmpstr1)->ptr);

		bitmap_dump(bitmaptmp);
	}
    else if (strcmp(str, "bitmap_expand") == 0) {
        scanf("%s %d", tmpstr1, &tmp1);
        getTableAddressOf(tmpstr1)->ptr = bitmap_expand((struct bitmap*)(getTableAddressOf(tmpstr1)->ptr), tmp1);
    }
}

void dumpdata(char name[100]) {
	DEBUG1("dumpdata start (param : %s)", name);
	struct dataStructure *target = getTableAddressOf(name);

	if (target->type == 'l') {
		struct list_elem *e;
		struct list* l = (struct list*)(target->ptr);
		if (list_empty(l))	return;
		for (e = list_begin(l); e != list_end(l); e = list_next(e))
		{
			struct foo_list_elem *f = list_entry(e, struct foo_list_elem, elem);
			printf("%d ", f->data);
		}
		printf("\n");
	}
	else if (target->type == 'h') {
		struct hash *h = (struct hash*)(target->ptr);
		struct hash_iterator i;
		if (hash_empty(h))	return;
		hash_first(&i, h);
		while (hash_next(&i))
		{
			struct foo_hash_elem *f = hash_entry(hash_cur(&i), struct foo_hash_elem, elem);
			printf("%d ", f->data);
		}
		printf("\n");
	}
	else if (target->type == 'b') {
		struct bitmap *b = (struct bitmap*)(target->ptr);
		int i;
		if (bitmap_size(b) == 0)	return;
		for (i = 0; i < bitmap_size(b); i++) {
			printf("%d", bitmap_test(b, i));
		}
		printf("\n");
	}
}
