#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char name[32];
	double score;
}Data;

typedef struct Node{
	struct Node* next;
	struct Node* prev;
	Data* data;
}Node;

typedef struct {
	Node* head;
	Node* tail;
	size_t size;
} List;

Data* makeData_interactive();
Data* makeData(const char* name, double score);
List* makeList();
void list_clear(List* list);
void destroy_list(List* list);
void dspData(Data* p);
void dspList(List* list);
Data* list_getData(List* list, int index);
void list_append(List* list, Data* data);
void list_insert(List* list, Data* data, int index);
void list_drop(List* list);
void list_remove(List* list, int index);


Data* makeData_interactive() {
	Data *data;
	char name[32];
	double h;

	data = (Data*)malloc(sizeof(Data));

	if (data == NULL) {
		exit(1);
	}

	printf("name> ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = '\0';  // 改行除去

	printf("score> ");
	scanf("%lf", &h);
	while (getchar() != '\n');  // 残りを消す

	strcpy(data->name, name);
	data->score = h;

	return data;
}

Data* makeData(const char* name, double score) {
	Data* data = (Data*)malloc(sizeof(Data));
	if (data == NULL) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}

	// 安全にコピー（最大長に収める）
	strncpy(data->name, name, sizeof(data->name) - 1);
	data->name[sizeof(data->name) - 1] = '\0';  // null終端の保証

	data->score = score;
	return data;
}

List* makeList() {
	List* list = (List*)malloc(sizeof(List));
	list->head = list->tail = NULL;
	list->size = 0;
	return list;
}

void destroy_list(List* list) {
	Node* cur = list->head;
	while (cur) {
		Node* next = cur->next;
		free(cur->data);
		free(cur);
		cur = next;
	}
	free(list);
}

void list_clear(List* list) {
	while (list->size > 0) {
		list_drop(list);
	}
}

void dspData(Data *p) {
	printf("name: %s, score: %lf\n", p->name,p->score);
}

void dspList(List* list) {
	Node* cur = list->head;
	while (cur) {
		dspData(cur->data);
		cur = cur->next;
	}
}

void dspList_detail(List* list) {
	printf("head* %p\n", list->head);
	printf("tail* %p\n", list->tail);
	printf("size %d\n", list->size);
	printf("\n");

	Node* cur = list->head;
	while (cur) {
		dspData(cur->data);
		printf("%p, %p, %p\n", cur->prev, cur, cur->next);
		cur = cur->next;
	}
}

void list_append(List* list, Data *data) {
	Node* node = (Node*)malloc(sizeof(Node));
	node->data	= data;
	node->next = NULL;
	node->prev = list->tail;

	if (list->tail) { 
		list->tail->next = node; 
	}
	else { 
		list->head = node; 
	}

	list->tail = node;
	list->size++;
}

void list_insert(List* list, Data* data, int index) {
	// pythonの組み込み関数 insert のような挙動をする
	// index は 0 以上 list->size 以下
	if (index < 0 || index > list->size) {
		printf("error: Invalid index\n");
		return;
	}


	// 末尾に追加
	if (index == list->size) {
		list_append(list, data);
		return;
	}

	Node* node = (Node*)malloc(sizeof(Node));
	node->data = data;

	// 先頭に追加
	if (index == 0) {
		node->next = list->head;
		node->prev = NULL;

		if (list->head) {
			list->head->prev = node;
		}
		else {
			list->tail = node;
		}

		list->head = node;
		list->size++;
		return;
	}
	// 中間に追加
	else {
		Node* cur = list->head;
		for (int i = 0; i < index; i++) {
			cur = cur->next;
		}
		// curの直前に追加
		node->prev = cur->prev;
		node->next = cur;
		if (cur->prev) { 
			cur->prev->next = node;
		}
		cur->prev = node;
	}
	list->size++;
}

void list_drop(List* list) {
	if (list->size == 0) {
		return;
	}
	
	Node* to_delete = list->tail;

	if (list->size == 1) {
		list->head = list->tail = NULL;
	}
	else {
		list->tail = to_delete->prev;
		list->tail->next = NULL;
	}

	free(to_delete->data);
	free(to_delete);
	list->size--;
}

void list_remove(List* list, int index) {
	// pythonの del のような挙動をする
	// index は 0 以上 list->size -1  以下
	if (index < 0 || index > list->size-1) {
		printf("error: Invalid index\n");
		return;
	}

	// 最後のnodeを削除
	if (index == list->size - 1) {
		list_drop(list);
		return;
	}

	// 
	Node* to_delete;

	// 先頭のnodeを削除
	if (index == 0) {
		to_delete = list->head;
		if (list->size == 1) {
			list->head = list->tail = NULL;
		}
		else {
			list->head = to_delete->next;
			list->head->prev = NULL;
		}
	}
	// 中間のnodeを削除
	else {
		to_delete = list->head;
		for (int i = 0;i < index;i++) {
			to_delete = to_delete->next;
		}
		to_delete->prev->next = to_delete->next;
		to_delete->next->prev = to_delete->prev;
	}

	free(to_delete->data);
	free(to_delete);
	list->size--;
}

Data* list_getData(List* list, int index) {
	if (index < 0 || list->size - 1 < index) {
		printf("error: Invalid index\n");
		return NULL;
	}

	Node* cur = list->head;
	for (int i = 0;i < index;i++) {
		cur = cur->next;
	}
	return cur->data;
}

int main() {
	List* list = makeList();

	Data* data[3] = {
		makeData("hoge1",95.5),
		makeData("hoge2",20.5),
		makeData("hoge3",80.5)
	};

	for (int i = 0;i < 3;i++) {
		list_append(list, data[i]);
	}

	Data *data4 = makeData("hoge4",60.95);

	list_insert(list, data4, 2);

	dspList(list);

	printf("\n");

	for (int i = 0;i <list->size;i++) {
		dspData(list_getData(list, i));
	}

	printf("\n");


	list_remove(list, 4);

	dspList(list);

	return 0;
}

