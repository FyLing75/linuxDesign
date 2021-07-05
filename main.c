#include "list.h"
#include "stdio.h"
#include "string.h"

typedef struct cntWord {
    int cnt;
    char word[30];
    struct list_head myList;
} Word;

typedef struct StructWord {
    char word[30];
    struct StructWord *next;
} myWord;

int numWord = 0;
int sumWord = 0;

// �����ļ�·��
void getFilePath(char *filePath) {
    puts("Please enter the file path:");
    //    strcpy(filePath, "E:\\Projects\\CLion\\linuxDesign\\LinuxFile.txt");
    scanf("%s", filePath);
}

// �ж��ǲ���ĳ��������ַ������� 0-9 a-z A-Z
int inSection(char character, int begin, int end) {
    return character >= begin && character <= end;
}

// �ж��ǲ�����ĸ
int isLetter(char character) {
    return inSection(character, 'A', 'Z') ||
           inSection(character, 'a', 'z');
}

// �ж��ǲ��ǵ��ʵ����ӷ���
int isWordSymbol(char character) {
    return character == '-' || character == '\'' || character == '.';
}

// �ǲ��ǽ�β��
int isEndChar(char character) {
    return character == '.' || character == '?' ||
           character == ',' || character == ';' ||
           character == ':' || character == '-' ||
           character == '!';
}

// ����ȡ���ַ����ʻ�
myWord *toTheWord(char word[30]) {
    myWord *h = (myWord *) malloc(sizeof(myWord));
    h->next = NULL;
    //ɾ����ͷ���з����֡���ĸ���ַ�
    while (!(isLetter(word[0]) ||
             inSection(word[0], '0', '9')) &&
           word[0])
        strcpy(word, word + 1);
    strcpy(h->word, word);
    for (int i = 0; word[i]; ++i) {
        //���д�д��Сд
        if (inSection(word[i], 'A', 'Z')) {
            word[i] += 32;
            strcpy(h->word, word);
        }
            //ɾ�� !(- ' ��ĸ)
        else if (!inSection(word[i], 'a', 'z') &&
                 !inSection(word[i], '0', '9') &&
                 !isWordSymbol(word[i]) ||
                 (word[i] == '.' && !inSection(word[i - 1], '0', '9'))) {
            word[i] = 0;
            strcpy(h->word, word);
            h->next = toTheWord(word + i + 1);
            break;
        }
    }
    for (int lastIndex = (int) strlen(h->word) - 1;
         isEndChar(h->word[lastIndex]);
         lastIndex = (int) strlen(h->word) - 1)
        h->word[lastIndex] = 0;
    return h;
}

// ���ͳ�ƽ��
void printWordList(struct list_head *head) {
    int i = 0;
    int line;
    puts("\n\n=========================================================================================================");
    puts("Please enter the number of times you want to display the top N of the word frequency");
    printf("(input 0 output the top 50, input -1 output all) : ");
    scanf("%d", &line);
    if (line == 0)
        line = 50;
    // ����������м�ϵͳ�ڵ�p���û���ʹ��
    struct list_head *p;
    puts("\n\n=========================================================================================================");
    list_for_each(p, head) {
        if (i % 10 == 0) {
            puts("\n\nIf the frequency less than 10, and the histogram will be show 'empty'!");
            puts("Else the histogram will show the result of dividing the occurrence of the word by 5!");
            puts("+=========+====================+============================+===========+============+");
            // ���������ʣ�����ͼ��Ƶ�ʣ�����
            printf("| %-7s | %18s | %-26s | %-9s | %-10s |",
                   "ranking", "word", "histogram", "frequency", "ratios");
            puts("\n+=========+====================+============================+===========+============+");
        }
        Word *entry = list_entry(p, Word, myList);
        printf("| %7d | %18s | ", ++i, entry->word);
        if (entry->cnt < 10) {
            printf("%-26s", "empty");
        } else {
            int j = 0;
            for (j = 0; j < entry->cnt / 5; j++)
                printf("=");
            while (j++ < 26)
                printf(" ");
        }
        printf(" | %-9d | %-9.5f%% |", entry->cnt, entry->cnt * 100.0 / sumWord);
        puts("\n+---------+--------------------+----------------------------+-----------+------------+");
        if (i == line)
            break;
    }
    puts("\n\n=========================================================================================================");
    printf("Total number of words is: %-5d! Number of words is: %-5d\n", sumWord, numWord);
}

/**
 * ģ���ַ�����ȱȽ��㷨
 * @param p1 �ַ����� >= p2
 * @param p2
 * @return ���p2���ַ�����p1ǰ�벿����ȣ��򷵻����1�����򷵻ز����0
 */
int strEqu_unclear(char p1[30], char p2[30]) {
    // ����Դֵ����
    char a[30], b[30];
    // aʼ�ձ����ַ����ϳ���
    if (strlen(p1) > strlen(p2)) {
        strcpy(a, p1);
        strcpy(b, p2);
    } else {
        strcpy(a, p2);
        strcpy(b, p1);
    }
    // �������  xxx's xxx xxxs'����ͬһ����
    int lastIndex = (int) strlen(a);
    if (a[lastIndex] == '\'' || a[lastIndex - 1] == '\'') {
        return strlen(a) == strlen(b);
    }
    // �㷨���ģ�ģ��ƥ��
    for (int i = 0; i <= (strlen(a) + 1) / 2 && b[i]; ++i) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

struct list_head *addWord(struct list_head *pHead, Word *pWord) {
    // �ܵ��ʸ���+1
    sumWord++;
    // ����������м�ϵͳ�ڵ�p���û���ʹ��
    struct list_head *p;
    // �����ڵ�ƥ���µ����Ƿ��Ѽ�������
    list_for_each(p, pHead) {
        // ��ȡ��ǰ�ڵ�������ڵ�
        Word *entry_curr = list_entry(p, Word, myList);
        // ���ַ�����ȣ�����Ҫ��ӣ����� +1 ����
        if (strEqu_unclear(entry_curr->word, pWord->word)) {
            // ���� + 1
            entry_curr->cnt++;
            // ��ǽڵ㣬����ѭ����ֹ����������������ֵ
            Word *end = list_entry(p, Word, myList);
            struct list_head *q = entry_curr->myList.prev;
            Word *entry_prev = list_entry(q, Word, myList);
            // +1֮��ĵ�����Ҫ���н�������
            while (q != (&end->myList) && entry_prev->cnt < entry_curr->cnt) {
                int tmpCnt = entry_curr->cnt;
                entry_curr->cnt = entry_prev->cnt;
                entry_prev->cnt = tmpCnt;
                char tmpWord[30];
                strcpy(tmpWord, entry_curr->word);
                strcpy(entry_curr->word, entry_prev->word);
                strcpy(entry_prev->word, tmpWord);
                entry_curr = entry_prev;
                q = entry_curr->myList.prev;
                entry_prev = list_entry(q, Word, myList);
            }
            return pHead;
        }
    }
    // ȫ�µ��ʣ����� = 1��
    pWord->cnt = 1;
    // ������������+1
    numWord++;
    // ������β
    list_add_tail(&(pWord->myList), pHead);
    return pHead;
}

// �ڵ����� - �ж��ǲ��Ƿ���ʵĵ��ʣ��Ƿ��� 1
int strcmp_EmptyWord(char *word, char a[][12], int n) {
    int i;
    for (i = 0; i < n; ++i)
        if (strcmp(word, a[i]) == 0)
            return 1;
    return 0;
}

// �ڵ����� - �ж��ǲ��Ƿ���ʵĵ��ʣ��Ƿ��� 1
int isEmptyWord(char *word) {
    switch (word[0]) {
        // a ��ͷ�����
        case 'a': {
            char a[][12] = {
                    "a", "about", "above", "across", "after",
                    "ahead", "along", "also", "although", "always",
                    "among", "an", "and", "any", "around",
                    "as", "at", "away"
            };
            if (strcmp_EmptyWord(word, a, 18))
                return 1;
        }
            break;
            // b ��ͷ�����
        case 'b': {
            char a[][12] = {
                    "because", "before", "behind", "between", "beyond",
                    "but", "by"
            };
            if (strcmp_EmptyWord(word, a, 7))
                return 1;
        }
            break;
            // c ��ͷ�����
        case 'c': {
            char a[][12] = {"can", "could"};
            if (strcmp_EmptyWord(word, a, 2))
                return 1;
        }
            // d ��ͷ�����
        case 'd': {
            char a[][12] = {"despite", "down", "during"};
            if (strcmp_EmptyWord(word, a, 3))
                return 1;
        }
            break;
            // e ��ͷ�����
        case 'e': {
            char a[][12] = {"each", "even", "ever", "every"};
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // f ��ͷ�����
        case 'f': {
            char a[][12] = {"for", "from"};
            if (strcmp_EmptyWord(word, a, 2))
                return 1;
        }
            break;
            // h ��ͷ�����
        case 'h': {

            char a[][12] = {"have", "hello", "hey", "hi"};
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // i ��ͷ�����
        case 'i': {
            char a[][12] = {"if", "in", "into"};
            if (strcmp_EmptyWord(word, a, 3))
                return 1;
        }
            break;
            // m ��ͷ�����
        case 'm': {
            char a[][12] = {"many", "may", "might", "more", "much", "must"};
            if (strcmp_EmptyWord(word, a, 6))
                return 1;
        }
            break;
            // o ��ͷ�����
        case 'o': {
            char a[][12] = {
                    "of", "off", "oh", "on", "only",
                    "onto", "other", "out", "over"};
            if (strcmp_EmptyWord(word, a, 9))
                return 1;
        }
            break;
            // s ��ͷ�����
        case 's': {
            char a[][12] = {
                    "since", "so", "some", "such"};
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // t ��ͷ�����
        case 't': {
            char a[][12] = {
                    "than", "that", "the", "then", "there",
                    "therefore", "these", "this", "those", "through",
                    "throughout", "till", "to"
            };
            if (strcmp_EmptyWord(word, a, 13))
                return 1;
        }
            break;
            // u ��ͷ�����
        case 'u': {
            char a[][12] = {
                    "under", "until", "up", "upon"
            };
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // w ��ͷ�����
        case 'w': {
            char a[][12] = {
                    "well", "what", "when", "where", "whether",
                    "which", "while", "who", "whom", "whose",
                    "why", "with", "within", "without", "would",
            };
            if (strcmp_EmptyWord(word, a, 15))
                return 1;
        }
            break;
            // y ��ͷ�����
        case 'y': {
            if (strcmp(word, "yet") == 0)
                return 1;
        }
            break;
    }
    // �������
    return 0;
}

// ������� - �ж��ǲ��Ƿ���ʵĵ��ʣ��Ƿ��� 1
int isWord(char word[]) {
    if (strlen(word) > 1) {
        if (!isEmptyWord(word))
            return 1;
    } else if (strlen(word) == 1)
        // a����ʣ����Դ˴�Ҫ�ر���
        return inSection(word[0], 'b', 'z') ||
               inSection(word[0], 'A', 'Z');
    return 0;
}

// ɨ���ĵ�
void scanDocuments(char path[], struct list_head *pHead) {
    FILE *fp;
    // �ݴ�����֤�ļ����ڣ��ļ������������������ļ�·��
    while ((fp = fopen(path, "rt")) == NULL) {
        puts("Can't open this file and enter the file path again!");
        scanf("%s", path);
    }
    // �����ı������Ϲ淶һ���Զ�ȡ������ʣ����磺open(String str)�ȵȡ�
    myWord *hTmpWord;
    //  ���������ļ�
    while (!feof(fp)) {
        char word[30];
        fscanf(fp, "%s", word);
        printf("%s ", word);
        // ������ַ����ʻ��������ж�����ʣ���������洢
        hTmpWord = toTheWord(word);
        // ��һ������ʵĵ��ʣ����������
        while (hTmpWord && isWord(hTmpWord->word)) {
            Word *tWord = (Word *) malloc(sizeof(Word));
            strcpy(tWord->word, hTmpWord->word);
            pHead = addWord(pHead, tWord);
            myWord *p = hTmpWord->next;
            free(hTmpWord);
            hTmpWord = p;
        }
    }
    fclose(fp);
}

int main() {
    struct list_head head;
    INIT_LIST_HEAD(&head);
    char filePath[100];
    getFilePath(filePath);
    scanDocuments(filePath, &head);
    printWordList(&head);
    return 0;
}
