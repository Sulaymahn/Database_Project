#pragma warning(disable : 4996)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Attribute {
	char* Name;
} Attribute;

typedef struct FunctionalDependency {
	struct Attribute* lhs;
	struct Attribute* rhs;
} FunctionalDependency;

typedef struct AttributeListNode {
	struct Attribute Value;
	struct AttributeListNode* Next;
} AttributeListNode;

typedef struct AttributeList {
	struct AttributeListNode* Head;
	struct AttributeListNode* Tail;
} AttributeList;

typedef struct FunctionalDependencyListNode {
	struct FunctionalDependency Value;
	struct FunctionalDependencyListNode* Next;
} FunctionalDependencyListNode;

typedef struct FunctionalDependencyList {
	struct FunctionalDependencyListNode* Head;
	struct FunctionalDependencyListNode* Tail;
} FunctionalDependencyList;

Attribute* ATTRIBUTETOFIND;

bool IsSameAttribute(Attribute a, Attribute b) {
	if (strcmp(a.Name, b.Name) == 0) {
		return true;
	}
	return false;
}
bool IsSameFunctionalDependency(FunctionalDependency a, FunctionalDependency b) {
	if (IsSameAttribute(*a.lhs, *b.lhs) && IsSameAttribute(*a.rhs, *b.rhs)) {
		return true;
	}
	return false;
}

bool FunctionalDependencyExist(FunctionalDependencyList* list, FunctionalDependency dependency) {
	if (list != NULL) {
		FunctionalDependencyListNode* tmp = list->Head;
		while (tmp != NULL)
		{
			if (IsSameFunctionalDependency(tmp->Value, dependency)) {
				return true;
			}
			tmp = tmp->Next;
		}
		return false;
	}
	return false;
}

FunctionalDependencyList* AddFunctionalDependency(FunctionalDependencyList* list, FunctionalDependency dependency) {
	FunctionalDependencyListNode* node = (FunctionalDependencyListNode*)malloc(sizeof(FunctionalDependencyListNode));
	node->Next = NULL;
	node->Value = dependency;
	if (list == NULL) {
		FunctionalDependencyList* result = (FunctionalDependencyList*)malloc(sizeof(FunctionalDependencyList));
		result->Head = node;
		result->Tail = node;
		list = result;
	}
	else if (!FunctionalDependencyExist(list, dependency)) {
		list->Tail->Next = node;
		list->Tail = node;
	}
	return list;
}

AttributeList* AddAttribute(AttributeList* list, Attribute attribute) {
	AttributeListNode* node = (AttributeListNode*)malloc(sizeof(AttributeListNode));
	node->Value = attribute;
	node->Next = NULL;
	if (list == NULL) {
		AttributeList* result = (AttributeList*)malloc(sizeof(AttributeList));
		result->Head = node;
		result->Tail = node;
		list = result;
	}
	else if ((list->Head == NULL) || (list->Tail == NULL)) {
		list->Head = node;
		list->Tail = node;
	}
	else
	{
		list->Tail->Next = node;
		list->Tail = node;
	}
	return list;
}

FunctionalDependencyList* AppendFunctionalDependencyList(FunctionalDependencyList* list, FunctionalDependencyList* otherlist) {
	if (list == NULL) {
		return otherlist;
	}
	else if (otherlist == NULL) {
		return list;
	}
	else {
		list->Tail->Next = otherlist->Head;
		list->Tail = otherlist->Tail;
		return list;
	}
}

AttributeList* AppendAttributeList(AttributeList* list, AttributeList* otherlist) {
	if (list == NULL) {
		return otherlist;
	}
	else if (otherlist == NULL) {
		return list;
	}
	else {
		list->Tail->Next = otherlist->Head;
		list->Tail = otherlist->Tail;
		return list;
	}
}

Attribute* GetAttributeByName(AttributeList* list, char* name) {
	if (list == NULL) {
		return NULL;
	}
	AttributeListNode* traverser = list->Head;
	while (traverser != NULL) {
		if (strcmp(traverser->Value.Name, name) == 0) {
			return &traverser->Value;
		}
		traverser = traverser->Next;
	}
	return NULL;
}

bool AttributeExists(AttributeList* list, Attribute attribute) {
	AttributeListNode* traverser = list->Head;
	while (traverser != NULL) {
		if (IsSameAttribute(traverser->Value, attribute)) {
			return true;
		}
		traverser = traverser->Next;
	}
	return false;
}

AttributeList* GetAttributesFromUser() {
	printf("How many attributes for this relation? : ");

	int n;
	scanf("%d", &n);

	AttributeList* list = NULL;

	for (int i = 0; i < n; i++) {
		printf("%d. Attribute name: ", i + 1);
		char* name = (char*)malloc(sizeof(char));
		scanf("%s", name);
		Attribute attribute = { name };
		list = AddAttribute(list, attribute);
	}

	return list;
}

FunctionalDependencyList* GetAttributeFunctionalDependency(AttributeList* attributeList, Attribute* attribute)
{
	char otherAttributeName[50];
	FunctionalDependencyList* list = NULL;
	printf("Define the functional dependencies of %s\nWrite the name of the other attributes\nusage: [Attribute]->[OtherAttribute]\nType \"exit()\" to proceed\n", attribute->Name);
	printf("%s -> ", attribute->Name);
	while (true) {
		scanf("%s", otherAttributeName);
		if (strcmp(otherAttributeName, "exit()") == 0) {
			break;
		}

		Attribute* otherAttribute = GetAttributeByName(attributeList, otherAttributeName);
		if (otherAttribute != NULL && !IsSameAttribute(*otherAttribute, *attribute)) {
			FunctionalDependency funcDependency = { attribute, otherAttribute };
			list = AddFunctionalDependency(list, funcDependency);
		}

		printf("%s -> ", attribute->Name);
	}
	return list;
}

FunctionalDependencyList* GetAllAttributesFunctionalDependency(AttributeList* list) {
	AttributeListNode* traverser = list->Head;
	FunctionalDependencyList* funcList = NULL;
	while (traverser != NULL) {
		funcList = AppendFunctionalDependencyList(funcList, GetAttributeFunctionalDependency(list, &traverser->Value));
		traverser = traverser->Next;
	}
	return funcList;
}

AttributeList* CalculateClosure(FunctionalDependencyList* list, Attribute* attribute) {
	if (list == NULL) {
		return NULL;
	}
	FunctionalDependencyListNode* tmp = list->Head;
	AttributeList* result = NULL;
	while (tmp != NULL) {
		if ((tmp->Value.lhs == attribute) && (tmp->Value.rhs != ATTRIBUTETOFIND) && (tmp->Value.rhs != attribute)) {
			result = AddAttribute(result, *tmp->Value.rhs);
			result = AppendAttributeList(result, CalculateClosure(list, tmp->Value.rhs));
		}
		tmp = tmp->Next;
	}
	return result;
}

void PrintList(AttributeList* list) {
	AttributeListNode* tmp = list->Head;
	printf("{");
	while (tmp != NULL) {
		printf(" %s ", tmp->Value.Name);
		tmp = tmp->Next;
	}
	printf("}\n");
}

void GetClosure(AttributeList* attributes, FunctionalDependencyList* dependencies) {
	printf("What attribute do you want closure calculation? : ");
	char* nameofAttributeToFind = (char*)malloc(sizeof(char));
	int dmp = scanf("%s", nameofAttributeToFind);
	printf("The closure \"{%s}+\" = ", nameofAttributeToFind);
	ATTRIBUTETOFIND = GetAttributeByName(attributes, nameofAttributeToFind);
	if (ATTRIBUTETOFIND != NULL) {
		AttributeList* self = AddAttribute(NULL, *ATTRIBUTETOFIND);
		AttributeList* closure = AppendAttributeList(self, CalculateClosure(dependencies, ATTRIBUTETOFIND));
		PrintList(closure);
	}
	else {
		printf("Attribute does not exist");
	}
}

void PrintIntro() {
	printf("EasyR Mapper\n");
}

int main() {
	PrintIntro();
	AttributeList* attributes = GetAttributesFromUser();
	FunctionalDependencyList* dependencies = GetAllAttributesFunctionalDependency(attributes);
	GetClosure(attributes, dependencies);
	return 0;
}