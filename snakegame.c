#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#define CHECK_MALLOC(s)                                                       \
    if ((s) == NULL)                                                          \
    {                                                                         \
        printf("memory allocation failed in %s: line%d", __FILE__, __LINE__); \
        perror(":");                                                          \
        exit(-1);                                                             \
    }
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
struct snake
{
    int x, y;
    char value;
    struct snake *next;
};
struct food
{
    int x, y;
    char value;
    struct food *next;
};
struct food *create_food(char a[][80], struct food *start)
{
    int i = (rand() % (18 - 1 + 1)) + 1, j = (rand() % (78 - 1 + 1)) + 1;
    if (a[i][j] != ' ')
    {
        create_food(a, start);
    }
    struct food *n = (struct food *)malloc(sizeof(struct food));
    n->next = NULL;
    start = n;
    start->value = '$';
    start->x = i;
    start->y = j;
    return start;
}
struct food *create_mine(char a[][80], struct food *start)
{
    int i = (rand() % (18 - 1 + 1)) + 1, j = (rand() % (78 - 1 + 1)) + 1;
    if (a[i][j] != ' ')
    {
        create_mine(a, start);
    }
    struct food *n = (struct food *)malloc(sizeof(struct food));
    n->next = NULL;
    n->value = '+';
    n->x = i;
    n->y = j;
    if (start == NULL)
    {
        start = n;
    }
    else
    {
        struct food *ptr = start;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = n;
    }
    return start;
}
struct snake *create_snake(struct snake *start)
{
    struct snake *n, *ptr;
    int i;
    n = (struct snake *)malloc(sizeof(struct snake));
    n->value = 'A';
    n->x = 10;
    n->y = 40;
    n->next = NULL;
    start = n;
    ptr = start;
    for (i = 0; i < 4; i++)
    {
        n = n = (struct snake *)malloc(sizeof(struct snake));
        if (i == 3)
            n->value = 'H';
        else
            n->value = '*';
        n->x = 10;
        n->y = 41 + i;
        n->next = NULL;
        ptr->next = n;
        ptr = ptr->next;
    }
    return start;
}
void print(struct snake *start)
{
    while (start != NULL)
    {
        printf(">>%c %d %d<<\t", start->value, start->x, start->y);
        start = start->next;
    }
    printf("\n");
}
char get_snake(int i, int j, struct snake *start)
{
    while (start != NULL)
    {
        if (start->x == i && start->y == j)
        {
            return start->value;
        }
        start = start->next;
    }
    return ' ';
}
void print_board(char a[][80])
{
    int i, j;
    printf("\t\t\t\tIK SANP ASA BHI\n\n");
    printf("INSTRUCTIONS :\npress w a s d for up left down right and r for saving the game\npress * to exit\n\n");
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 80; j++)
        {
            printf("%c", a[i][j]);
        }
        printf("\n");
    }
}
void create_board(char a[][80], struct snake *start)
{
    int i, j;
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 80; j++)
        {
            if (i == 0 || i == 19)
            {
                a[i][j] = '-';
            }
            else if (j == 0 || j == 79)
            {
                a[i][j] = '|';
            }
            else
            {
                a[i][j] = get_snake(i, j, start);
            }
            //printf("%c",a[i][j]);
        }
        //printf("\n");
    }
}
void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
struct snake *adjust(struct snake *start, int i, int j)
{
    struct snake *p = start->next;
    while (p != NULL)
    {
        swap(&(p->x), &i);
        swap(&(p->y), &j);
        p = p->next;
    }
    return start;
}
struct snake *increase_snake(struct snake *start, char a[][80])
{
    int i = 0, j = 0;
    struct snake *n = (struct snake *)malloc(sizeof(struct snake));
    n->value = 'H';
    struct snake *l = start, *sl;
    while (l->next != NULL)
    {
        sl = l;
        l = l->next;
    }
    //printf("%d %d", l->x, l->y);
    if (sl->x == l->x)
    {
        if (a[l->x][(l->y) - 1] == ' ')
        {
            i = l->x;
            j = (l->y) - 1;
        }
        else if (a[l->x][(l->y) + 1] == ' ')
        {
            i = l->x;
            j = (l->y) + 1;
        }
    }
    if (sl->y == l->y)
    {
        if (a[(l->x) - 1][(l->y)] == ' ')
        {
            i = (l->x) - 1;
            j = (l->y);
        }
        else if (a[(l->x) + 1][(l->y)] == ' ')
        {
            i = (l->x) + 1;
            j = (l->y);
        }
    }
    //printf("%d %d", i, j);
    n->x = i;
    n->y = j;
    l->value = '*';
    n->next = NULL;
    l->next = n;
    return start;
}
struct food *create_dwall(char a[][80], struct food *start, int i)
{
    static int p = 0, j;
    int k = (rand() % (78 - 1 + 1)) + 1;
    ;
    if (p == 0)
    {
        if (a[i][k] != ' ')
        {
            create_dwall(a, start, 8);
        }
    }
    if (p == 1)
    {
        j = k;
    }
    p++;
    struct food *n = (struct food *)malloc(sizeof(struct food));
    n->next = NULL;
    n->value = '&';
    n->x = i;
    n->y = j;
    if (start == NULL)
    {
        start = n;
    }
    else
    {
        struct food *ptr = start;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = n;
    }
    j++;
    return start;
}
struct snake *check_move(char d, struct snake *head, char a[][80], int *f, int wall)
{
    int i = head->x, j = head->y;
    switch (d)
    {
    case 'a':
        if (a[i][j - 1] == '|' || a[i][j - 1] == '-')
        {
            if (wall == 0)
            {
                head->y = 77;
                head = adjust(head, i, j);
                break;
            }
            else
            {
                if (*f == 3)
                    *f = 1;
                else
                    *f = 2;
            }
        }
        else if (a[i][j - 1] == '$')
        {
            *f = 0;
        }
        else if (a[i][j - 1] == '*' || a[i][j - 1] == 'H' || a[i][j - 1] == '+' || a[i][j - 1] == '&')
        {
            if (*f == 3)
                *f = 1;
            else
                *f = 2;
        }
        head->y -= 1;
        head = adjust(head, i, j);
        break;
    case 'w':
        if (a[i - 1][j] == '|' || a[i - 1][j] == '-')
        {
            if (wall == 0)
            {
                head->x = 18;
                head = adjust(head, i, j);
                break;
            }
            else
            {
                if (*f == 3)
                    *f = 1;
                else
                    *f = 2;
            }
        }
        else if (a[i - 1][j] == '$')
        {
            *f = 0;
        }
        else if (a[i - 1][j] == '*' || a[i - 1][j] == 'H' || a[i - 1][j] == '+' || a[i - 1][j] == '&')
        {
            if (*f == 3)
                *f = 1;
            else
                *f = 2;
        }
        head->x = (head->x) - 1;
        head = adjust(head, i, j);
        break;
    case 's':
        if (a[i + 1][j] == '|' || a[i + 1][j] == '-')
        {
            if (wall == 0)
            {
                head->x = 1;
                head = adjust(head, i, j);
                break;
            }
            else
            {
                if (*f == 3)
                    *f = 1;
                else
                    *f = 2;
            }
        }
        else if (a[i + 1][j] == '$')
        {
            *f = 0;
        }
        else if (a[i + 1][j] == '*' || a[i + 1][j] == 'H' || a[i + 1][j] == '+' || a[i + 1][j] == '&')
        {
            if (*f == 3)
                *f = 1;
            else
                *f = 2;
        }
        head->x += 1;
        head = adjust(head, i, j);
        break;
    case 'd':
        if (a[i][j + 1] == '|' || a[i][j + 1] == '-')
        {
            if (wall == 0)
            {
                head->y = 1;
                head = adjust(head, i, j);
                break;
            }
            else
            {
                if (*f == 3)
                    *f = 1;
                else
                    *f = 2;
            }
        }
        else if (a[i][j + 1] == '$')
        {
            *f = 0;
        }
        else if (a[i][j + 1] == '*' || a[i][j + 1] == 'H' || a[i][j + 1] == '+' || a[i][j + 1] == '&')
        {
            if (*f == 3)
                *f = 1;
            else
                *f = 2;
        }
        head->y += 1;
        head = adjust(head, i, j);
        break;
    }
    return head;
}
void itoa(int num, char *result)
{
    int r, i = 0, j = 0;
    char t;
    while (num != 0)
    {
        r = num % 10;
        r = r + 48;
        result[i++] = r;
        num = num / 10;
    }
    result[i] = '\0';
    i--;
    while (i >= j)
    {
        t = result[i];
        result[i] = result[j];
        result[j] = t;
        i--;
        j++;
    }
}
void write_to_file(char *d, char *str)
{
    FILE *f;
    f = fopen(d, "w");
    fprintf(f, "%s", str);
    //printf("%s", str);
    fclose(f);
}
void save(struct snake *head, struct food *point, int c)
{
    char sx[10], sy[10], *str = (char *)malloc(sizeof(char) * 10000), score[4];
    itoa(c, score);
    strcat(str, score);
    while (head != NULL)
    {
        itoa(head->x, sx);
        itoa(head->y, sy);
        strcat(str, "s");
        strcat(str, sx);
        strcat(str, ",");
        strcat(str, sy);
        head = head->next;
    }
    while (point != NULL)
    {
        itoa(point->x, sx);
        itoa(point->y, sy);
        strcat(str, "f");
        strcat(str, sx);
        strcat(str, ",");
        strcat(str, sy);
        point = point->next;
    }
    write_to_file("save_file.txt", str);
    //printf("\n%s\n", str);
}
int retro_check(char pre, char new)
{
    switch (new)
    {
    case 'w':
        if (pre == 's')
            return 0;
        break;
    case 'a':
        if (pre == 'd')
            return 0;
        break;
    case 's':
        if (pre == 'w')
            return 0;
        break;
    case 'd':
        if (pre == 'a')
            return 0;
        break;
    }
    return 1;
}
void load(char a[][80], struct snake *head, struct food *point, int f, int c, char direction, struct food *mine)
{
    long int i = 1;
    int SPEED = 250000;
    int wall = 0, level = 1;
    char pred = direction;
    struct food *ptr, *dwall = NULL, *dwall1 = NULL;
    while (direction != '*')
    {
        head = check_move(direction, head, a, &f, wall);
        create_board(a, head);
        if (f == 2)
        {
            printf("\ngame over\n\n\nyour score was %d\n\n", c);
            break;
        }
        if (f == 0 || f == 3)
        {
            if (f == 0)
            {
                head = increase_snake(head, a);
                create_board(a, head);
                point = create_food(a, point);
                c += 10; //score
            }
            if (c >= 20 && c < 40)
            {
                SPEED = 200000;
                level = 2;
                mine = NULL;
                mine = create_mine(a, mine);
            }
            else if (c >= 40)
            {
                SPEED = 150000;
                level = 3;
                int mn = rand() % (20 + 1 - 5) + 5;
                mine = NULL;
                for (int u = 0; u < mn; u++)
                {
                    mine = create_mine(a, mine);
                }
            }
            if (c >= 60)
            {
                SPEED = 100000;
                level = 4;
                wall = 1;
            }
            if (c >= 80)
            {
                SPEED = 50000;
                level = 5;
                dwall = NULL;
                for (int u = 0; u < 7; u++)
                {
                    dwall = create_dwall(a, dwall, 8);
                }
                dwall1 = NULL;
                for (int u = 0; u < 7; u++)
                {
                    dwall1 = create_dwall(a, dwall1, 15);
                }
            }
            f = 1;
        }
        a[point->x][point->y] = point->value;
        ptr = mine;
        while (ptr != NULL)
        {
            a[ptr->x][ptr->y] = ptr->value;
            ptr = ptr->next;
        }
        ptr = dwall;
        while (ptr != NULL)
        {
            a[ptr->x][ptr->y] = ptr->value;
            ptr = ptr->next;
        }
        ptr = dwall1;
        while (ptr != NULL)
        {
            a[ptr->x][ptr->y] = ptr->value;
            ptr = ptr->next;
        }
        print_board(a);
        printf("\n\nyour score is %d\n\nLEVEL = %d\n\n", c, level);
        //scanf("%c", &direction);
        usleep(SPEED);
        if (kbhit())
        {
            pred = direction;
            direction = getchar();
            if (retro_check(pred, direction) == 0)
            {
                direction = pred;
            }
        }
        system("clear");
        if (direction == 'r' || c == 100)
        {
            break;
        }
    }
    if (direction == 'r')
    {
        save(head, point, c);
        printf("\n\n\t\tYOUR GAME HAS BEEN SAVED\n\n\t\tTHANK YOU FOR PLAYING\n\n");
    }
    if (c == 100)
    {
        printf("\n\n\t\tYOU WON\n\n");
    }
    if (direction == '*')
    {
        printf("\n\n\t\tTHANK YOU\n\n");
    }
}
char *get_in_string(char *dir)
{
    struct stat statbuf;
    stat(dir, &statbuf);
    long int n = (statbuf.st_size) + 2;
    char *buff = (char *)malloc(sizeof(char) * n);
    CHECK_MALLOC(buff);
    long int i = 0;
    FILE *fptr;
    char c;
    fptr = fopen(dir, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }
    c = fgetc(fptr);
    while (c != EOF)
    {
        //printf("\n%c", c);
        buff[i] = c;
        i++;
        c = fgetc(fptr);
    }
    strcat(buff, "\0");
    fclose(fptr);
    return buff;
}
void insert_snake(struct snake **start, int i, int j)
{
    struct snake *n = (struct snake *)malloc(sizeof(struct snake)), *ptr = *start;
    n->next = NULL;
    n->value = '*';
    n->x = i;
    n->y = j;
    if (*start == NULL)
    {
        *start = n;
    }
    else
    {
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = n;
    }
}
void insert_food(struct food **start, int i, int j)
{
    struct food *n = (struct food *)malloc(sizeof(struct food)), *ptr = *start;
    n->next = NULL;
    n->value = '$';
    n->x = i;
    n->y = j;
    if (*start == NULL)
    {
        *start = n;
    }
    else
    {
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = n;
    }
}
int myatoi(int s, int e, char *str)
{
    int n = 0;
    while (s < e)
    {
        n = n * 10 + (str[s] - 48);
        s++;
    }
    return n;
}
void reload(struct snake **head, struct food **point, int *c)
{
    char *str = get_in_string("save_file.txt");
    // printf("\n%s\n", str);
    *head = NULL;
    *point = NULL;
    int i = 0, j, X, Y, flag = 0;
    while (str[i] != 's')
    {
        printf("%d", i);
        i++;
    }
    *c = myatoi(0, i, str);
    while (str[i] != '\0')
    {
        flag = 0;
        if (str[i] == 's' || str[i] == 'f')
        {
            if (str[i] == 'f')
            {
                flag++;
            }
            i++;
            j = i;
            while (str[i] != ',')
            {
                i++;
            }
            X = myatoi(j, i, str);
            i++;
            j = i;
            while (str[i] != 's' && str[i] != 'f' && str[i] != '\0')
            {
                i++;
            }
            Y = myatoi(j, i, str);
            //printf("%d %d\n", X, Y);
            if (flag == 0)
                insert_snake(head, X, Y);
            else
                insert_food(point, X, Y);
        }
    }
    (*head)->value = 'A';
    struct snake *ptr = *head;
    while (ptr->next != NULL)
    {
        ptr = ptr->next;
    }
    ptr->value = 'H';
}
int check_file(char *dir)
{
    struct stat statbuf;
    stat(dir, &statbuf);
    if (statbuf.st_size > 0)
        return 1;
    return 0;
}
int main()
{
    int f = 0, e = 0, size, p, c = 0;
    char a[20][80], direction = 'a';
    struct snake *head = NULL;
    struct food *point = NULL, *mine = NULL;
    system("clear");
    printf("\n\t\tIK SANP ASA BHI\n\n\n\tOPTION\t\t\tBUTTON\n");
    if (check_file("save_file.txt") == 1)
    {
        printf("\tResume Game\t\t0\n");
    }
    printf("\tNew Game\t\t1\n");
    printf("\tExit\t\t\t2\n");
    scanf("%d", &p);
    switch (p)
    {
    case 0:
        reload(&head, &point, &c);
        load(a, head, point, 3, c, direction, mine);
        break;
    case 1:
        head = create_snake(head);
        system("clear");
        create_board(a, head);
        point = create_food(a, point);
        mine = NULL;
        FILE *fptr = fopen("save_file.txt", "w");
        load(a, head, point, 1, c, direction, mine);
        break;
    case 2:
        system("clear");
        printf("\n\n\t\tTHANK YOU\n\n");
        break;
    default:
        system("clear");
        printf("\n\n\t\tunknown key pressed\n\n");
        usleep(1000000);
        main();
        break;
    }
    return 0;
}