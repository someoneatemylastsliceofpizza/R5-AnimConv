#include <vector>

struct stringentry_t{
	char* base;
	char* addr;
	int* ptr;
	const char* string;
	int dupindex;
};

struct s_modeldata_t{
	void* pHdr;
	std::vector<stringentry_t> stringTable;
	char* pBase;
	char* pData;
};

inline s_modeldata_t g_model;

static void BeginStringTable()
{
	g_model.stringTable.clear();
	g_model.stringTable.emplace_back(stringentry_t{ NULL, NULL, NULL, "", -1 });
}

static void AddToStringTable(char* base, int* ptr, const char* string){
	if (!string)
		string = "";

	stringentry_t newString{};

	int i = 0;
	for (auto& it : g_model.stringTable){
		if (!strcmp(string, it.string)){
			newString.base = (char*)base;
			newString.ptr = ptr;
			newString.string = string;
			newString.dupindex = i;
			g_model.stringTable.emplace_back(newString);
			return;
		}
		i++;
	}

	newString.base = (char*)base;
	newString.ptr = ptr;
	newString.string = string;
	newString.dupindex = -1;

	g_model.stringTable.emplace_back(newString);
}

static char* WriteStringTable(char* pData){
	auto& stringTable = g_model.stringTable;
	for (auto& it : stringTable){
		if (it.dupindex == -1){
			it.addr = pData;
			if (it.ptr){
				*it.ptr = pData - it.base;
				int length = strlen(it.string);
				strcpy_s(pData, length + 1, it.string);

				pData += length;
			}
			*pData = '\0';
			pData++;
		}
		else{
			*it.ptr = stringTable[it.dupindex].addr - it.base;
		}
	}
	return pData;
}