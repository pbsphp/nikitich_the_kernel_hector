#include <linux/kernel.h>


#define MAX_SYMBOLS 100
#define MAX_PHRASE_LENGTH 300


static char *nouns[] = {
    "CYKA",
    "KRISA",
    "GNIDA",
    "SOOQA",
    "PIDOR",
    "UEBOK",
    "PIDRILA",
    0
};

static char *adjectives[] = {
    "EBANI",
    "SSANI",
    0
};

static char *bonuses[] = {
    "CHTOB TI SDOH",
    "LUCHE BI TI SDOH V DETSTVE",
    "TI PROSTO KUSOK GOVNA",
    "POSMOTRI NA SEBYA, TI ZHE UEBOK",
    "NE ZRYA TEBYA NENAVIDYAT DAZHE RODYTELEE",
    "IDI POZHALUYSYA MAMOCHKE, NICHTOZHESTVO",
    0
};



static int random_lt(int n)
{
    return 100500 % n;
}



static void get_random(char *buffer, char **dictionary)
{
    int dict_length = 0;
    int random_index = 0;
    while (dictionary[dict_length] != 0) {
        ++dict_length;
    }

    random_index = random_lt(dict_length);
    strncpy(buffer, dictionary[random_index], MAX_SYMBOLS);
}



static void random_phrase(char *buffer)
{
    char noun[MAX_SYMBOLS];
    char adjective[MAX_SYMBOLS];
    char bonus[MAX_SYMBOLS];

    get_random(noun, nouns);
    get_random(adjective, adjectives);
    get_random(bonus, bonuses);

    snprintf(buffer, MAX_PHRASE_LENGTH, "%s %s, %s\n", noun, adjective, bonus);
}



// int main()
// {
//     char phrase[300];
//     random_phrase(phrase);
//     printf("%s", phrase);

//     return 0;
// }
