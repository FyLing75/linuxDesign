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

// 输入文件路径
void getFilePath(char *filePath) {
    puts("Please enter the file path:");
    //    strcpy(filePath, "E:\\Projects\\CLion\\linuxDesign\\LinuxFile.txt");
    scanf("%s", filePath);
}

// 判断是不是某个区间的字符，比如 0-9 a-z A-Z
int inSection(char character, int begin, int end) {
    return character >= begin && character <= end;
}

// 判断是不是字母
int isLetter(char character) {
    return inSection(character, 'A', 'Z') ||
           inSection(character, 'a', 'z');
}

// 判断是不是单词的连接符号
int isWordSymbol(char character) {
    return character == '-' || character == '\'' || character == '.';
}

// 是不是结尾符
int isEndChar(char character) {
    return character == '.' || character == '?' ||
           character == ',' || character == ';' ||
           character == ':' || character == '-' ||
           character == '!';
}

// 将获取的字符单词化
myWord *toTheWord(char word[30]) {
    myWord *h = (myWord *) malloc(sizeof(myWord));
    h->next = NULL;
    //删除开头所有非数字、字母的字符
    while (!(isLetter(word[0]) ||
             inSection(word[0], '0', '9')) &&
           word[0])
        strcpy(word, word + 1);
    strcpy(h->word, word);
    for (int i = 0; word[i]; ++i) {
        //所有大写变小写
        if (inSection(word[i], 'A', 'Z')) {
            word[i] += 32;
            strcpy(h->word, word);
        }
            //删除 !(- ' 字母)
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

// 输出统计结果
void printWordList(struct list_head *head) {
    int i = 0;
    int line;
    puts("\n\n=========================================================================================================");
    puts("Please enter the number of times you want to display the top N of the word frequency");
    printf("(input 0 output the top 50, input -1 output all) : ");
    scanf("%d", &line);
    if (line == 0)
        line = 50;
    // 遍历链表的中间系统节点p，用户不使用
    struct list_head *p;
    puts("\n\n=========================================================================================================");
    list_for_each(p, head) {
        if (i % 10 == 0) {
            puts("\n\nIf the frequency less than 10, and the histogram will be show 'empty'!");
            puts("Else the histogram will show the result of dividing the occurrence of the word by 5!");
            puts("+=========+====================+============================+===========+============+");
            // 排名，单词，柱形图，频率，比例
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
 * 模糊字符串相等比较算法
 * @param p1 字符长度 >= p2
 * @param p2
 * @return 如果p2的字符能与p1前半部分相等，则返回相等1，否则返回不相等0
 */
int strEqu_unclear(char p1[30], char p2[30]) {
    // 保护源值不变
    char a[30], b[30];
    // a始终保存字符串较长的
    if (strlen(p1) > strlen(p2)) {
        strcpy(a, p1);
        strcpy(b, p2);
    } else {
        strcpy(a, p2);
        strcpy(b, p1);
    }
    // 避免出现  xxx's xxx xxxs'视作同一单词
    int lastIndex = (int) strlen(a);
    if (a[lastIndex] == '\'' || a[lastIndex - 1] == '\'') {
        return strlen(a) == strlen(b);
    }
    // 算法核心，模糊匹配
    for (int i = 0; i <= (strlen(a) + 1) / 2 && b[i]; ++i) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

struct list_head *addWord(struct list_head *pHead, Word *pWord) {
    // 总单词个数+1
    sumWord++;
    // 遍历链表的中间系统节点p，用户不使用
    struct list_head *p;
    // 遍历节点匹配新单词是否已加入链表
    list_for_each(p, pHead) {
        // 获取当前节点的宿主节点
        Word *entry_curr = list_entry(p, Word, myList);
        // 两字符串相等，不需要入队，数量 +1 即可
        if (strEqu_unclear(entry_curr->word, pWord->word)) {
            // 次数 + 1
            entry_curr->cnt++;
            // 标记节点，用以循环终止条件，不更改所有值
            Word *end = list_entry(p, Word, myList);
            struct list_head *q = entry_curr->myList.prev;
            Word *entry_prev = list_entry(q, Word, myList);
            // +1之后的单词需要进行降序排序
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
    // 全新单词，数量 = 1。
    pWord->cnt = 1;
    // 单词类型数量+1
    numWord++;
    // 加入链尾
    list_add_tail(&(pWord->myList), pHead);
    return pHead;
}

// 内调函数 - 判断是不是非虚词的单词，是返回 1
int strcmp_EmptyWord(char *word, char a[][12], int n) {
    int i;
    for (i = 0; i < n; ++i)
        if (strcmp(word, a[i]) == 0)
            return 1;
    return 0;
}

// 内调函数 - 判断是不是非虚词的单词，是返回 1
int isEmptyWord(char *word) {
    switch (word[0]) {
        // a 开头的虚词
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
            // b 开头的虚词
        case 'b': {
            char a[][12] = {
                    "because", "before", "behind", "between", "beyond",
                    "but", "by"
            };
            if (strcmp_EmptyWord(word, a, 7))
                return 1;
        }
            break;
            // c 开头的虚词
        case 'c': {
            char a[][12] = {"can", "could"};
            if (strcmp_EmptyWord(word, a, 2))
                return 1;
        }
            // d 开头的虚词
        case 'd': {
            char a[][12] = {"despite", "down", "during"};
            if (strcmp_EmptyWord(word, a, 3))
                return 1;
        }
            break;
            // e 开头的虚词
        case 'e': {
            char a[][12] = {"each", "even", "ever", "every"};
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // f 开头的虚词
        case 'f': {
            char a[][12] = {"for", "from"};
            if (strcmp_EmptyWord(word, a, 2))
                return 1;
        }
            break;
            // h 开头的虚词
        case 'h': {

            char a[][12] = {"have", "hello", "hey", "hi"};
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // i 开头的虚词
        case 'i': {
            char a[][12] = {"if", "in", "into"};
            if (strcmp_EmptyWord(word, a, 3))
                return 1;
        }
            break;
            // m 开头的虚词
        case 'm': {
            char a[][12] = {"many", "may", "might", "more", "much", "must"};
            if (strcmp_EmptyWord(word, a, 6))
                return 1;
        }
            break;
            // o 开头的虚词
        case 'o': {
            char a[][12] = {
                    "of", "off", "oh", "on", "only",
                    "onto", "other", "out", "over"};
            if (strcmp_EmptyWord(word, a, 9))
                return 1;
        }
            break;
            // s 开头的虚词
        case 's': {
            char a[][12] = {
                    "since", "so", "some", "such"};
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // t 开头的虚词
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
            // u 开头的虚词
        case 'u': {
            char a[][12] = {
                    "under", "until", "up", "upon"
            };
            if (strcmp_EmptyWord(word, a, 4))
                return 1;
        }
            break;
            // w 开头的虚词
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
            // y 开头的虚词
        case 'y': {
            if (strcmp(word, "yet") == 0)
                return 1;
        }
            break;
    }
    // 不是虚词
    return 0;
}

// 外调函数 - 判断是不是非虚词的单词，是返回 1
int isWord(char word[]) {
    if (strlen(word) > 1) {
        if (!isEmptyWord(word))
            return 1;
    } else if (strlen(word) == 1)
        // a是虚词，所以此处要特别处理
        return inSection(word[0], 'b', 'z') ||
               inSection(word[0], 'A', 'Z');
    return 0;
}

// 扫描文档
void scanDocuments(char path[], struct list_head *pHead) {
    FILE *fp;
    // 容错处理，保证文件存在，文件不存在则重新输入文件路径
    while ((fp = fopen(path, "rt")) == NULL) {
        puts("Can't open this file and enter the file path again!");
        scanf("%s", path);
    }
    // 处理文本不符合规范一次性读取多个单词，例如：open(String str)等等。
    myWord *hTmpWord;
    //  遍历整个文件
    while (!feof(fp)) {
        char word[30];
        fscanf(fp, "%s", word);
        printf("%s ", word);
        // 读入的字符单词化，可能有多个单词，所以链表存储
        hTmpWord = toTheWord(word);
        // 是一个非虚词的单词，则加入链表
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
