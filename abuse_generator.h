#include <linux/kernel.h>
#include <linux/random.h>


#define MAX_SYMBOLS 100
#define MAX_PHRASE_LENGTH 1000


static char *nouns[] = {
    "CYKA",
    "KRISA",
    "GNIDA",
    "SOOQA",
    "PIDOR",
    "UEBOK",
    "PIDRILA",
    "GANDILA",
    0
};

static char *adjectives[] = {
    "EBANI",
    "SSANI",
    "ZASHKVARINY",
    "OBOSSANY",
    0
};

static char *etc[] = {
    "CHTOB TI SDOH",
    "LUCHE BI TI SDOH V DETSTVE",
    "TI PROSTO KUSOK GOVNA",
    "POSMOTRI NA SEBYA, TI ZHE UEBOK",
    "NE ZRYA TEBYA NENAVIDYAT DAZHE RODYTELEE",
    "IDI POZHALUYSYA MAMOCHKE, NICHTOZHESTVO",
    "TVOI BATYA LISIY",
    0
};

static char *verbs[] = {
    "SOSI",
    "SOSI HUY",
    "GOVNA POESH",
    "NAVERNI GOVNA",
    "GOVNETSA NAVERNI",
    "VIEBI SVOYU MAMKU",
    "NASSI SEBE V ROT",
    0
};

static char *patterns[] = {
    "%n %e, %e BLYAT",
    "%n %e, %n",
    "%e, %e, %n",
    "%n %a, %n %a",
    "%n, BLYAT, %a. %n!",
    "%n NAHUI",
    "%n %e, NAHUI",
    "%a %n",
    "%a %n, BLYAT",
    "%n %a, %e EPT",
    "%e, %n",
    "%e, %a %n",
    0
};



static int random_lt(unsigned int n)
{
    unsigned int rnd = 0;
    get_random_bytes(&rnd, sizeof(rnd));

    if (n == 0) {
        return 0;
    }
    return rnd % n;
}



static void get_random_from_dict(char *buffer, char **dictionary)
{
    int dict_length = 0;
    int random_index = 0;
    while (dictionary[dict_length] != 0) {
        ++dict_length;
    }

    random_index = random_lt(dict_length);
    strncpy(buffer, dictionary[random_index], MAX_SYMBOLS);
}



static void get_random_word_by_type(char type, char *word)
{
    char **dict = 0;

    switch (type) {
    case 'n':
        dict = nouns;
        break;
    case 'a':
        dict = adjectives;
        break;
    case 'e':
        dict = etc;
        break;
    case 'v':
        dict = verbs;
        break;
    default:
        strcpy(word, "%");
        return;
    }

    get_random_from_dict(word, dict);
}



static void get_random_phrase(char *phrase)
{
    char pattern_buffer[MAX_PHRASE_LENGTH];
    char *pattern = pattern_buffer;

    char word_buffer[MAX_SYMBOLS];
    char *word = word_buffer;

    int i = 0;

    get_random_from_dict(pattern, patterns);

    while (*pattern != '\0') {
        word = word_buffer;
        if (*pattern == '%' && *(pattern + 1) != '\0') {
            ++pattern;

            get_random_word_by_type(*pattern, word);

            i = MAX_SYMBOLS;
            while (*word != '\0' && i--) {
                *phrase++ = *word++;
            }
        }
        else {
            *phrase++ = *pattern;
        }

        ++pattern;
    }
    *phrase = '\0';
}



// int main()
// {
//     char phrase[1000];

//     get_random_phrase(phrase);

//     printf("%s\n", phrase);

//     return 0;
// }
