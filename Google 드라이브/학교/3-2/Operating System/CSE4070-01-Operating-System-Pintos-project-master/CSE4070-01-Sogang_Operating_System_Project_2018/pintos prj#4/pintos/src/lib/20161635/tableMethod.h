void tableInit() {
	DEBUG0("tableinit start");
    int i;
    for(i = 0; i < 10; i++) {
        table[i].type = 'l';
        list_init(&(foo_list[i]));
        table[i].ptr = &(foo_list[i]);
        table[i].name[0] = '\0';
    }
    for(i = 10; i < 20; i++) {
        table[i].type = 'h';
		DEBUG2("h_hash_func : %p, h_less_func : %p", &h_hash_func, &h_less_func);
		hash_init(&(foo_hash[i - 10]), &h_hash_func, &h_less_func, NULL);
		table[i].ptr = &(foo_hash[i - 10]);
        table[i].name[0] = '\0';
    }
    for(i = 20; i < 30; i++) {
        table[i].type = 'b';
        table[i].name[0] = '\0';
    }	// initialize in insertDS
}

struct dataStructure* getTableAddressOf(char name[100]) {
	DEBUG1("getTableAddressOf start (param : %s)", name);
    int i = 0;
    for (i = 0; i < 30; i++) {
        if(strcmp(table[i].name, name) == 0) {
			DEBUG2("getTableAddressOf success (ptr : %p, idx : %d)", &table[i], i);
            return &(table[i]);
        }
    }
    return NULL;
}

void insertDS(char name[100], char type) {
	DEBUG2("insertDS start (param : %s, %c)", name, type);
    int i;
    switch(type) {
    case 'l':
        for(i = 0; i < 10; i++) {
            if(strlen(table[i].name) == 0) {
				DEBUG1("insertDS success (ptr : %p)", &(table[i]));
                strcpy(table[i].name, name);
				break;
            }
        }
        break;
    case 'h':
        for(i = 10; i < 20; i++) {
            if(strlen(table[i].name) == 0) {
				DEBUG1("insertDS success (ptr : %p)", &(table[i]));
                strcpy(table[i].name, name);
				break;

            }
        }
        break;
    case 'b':
        for(i = 20; i < 30; i++) {
            if(strlen(table[i].name) == 0) {
				int tmp;
				scanf("%d", &tmp);
				table[i].ptr = bitmap_create(tmp);
				DEBUG1("insertDS success (ptr : %p)", &(table[i]));
                strcpy(table[i].name, name);
				break;
            }
        }
    }
}

void deleteDS(char name[100]) {
    char c;
    struct list *l;
    struct hash *h;
    struct bitmap *b;
    struct list_elem *e, *tmp;
	DEBUG1("deleteDS start (param : %s)", name);
    c = getTableAddressOf(name)->type;
    switch(c) {
        case 'l':
            l = (struct list*)(getTableAddressOf(name)->ptr);
            if(list_size(l) == 0)
                break;
            for(e = list_begin(l); e != list_tail(l); ) {
                tmp = e;
                e = list_next(e);
                free(list_entry(tmp, struct foo_list_elem, elem));
            }
            break;
        case 'h':
            h = (struct hash*)(getTableAddressOf(name)->ptr);
            hash_destroy(h, &hash_destructor);
            break;
        case 'b':
            b = (struct bitmap*)(getTableAddressOf(name)->ptr);
            bitmap_destroy(b);
            break;
    }
    getTableAddressOf(name)->name[0] = '\0';
}
