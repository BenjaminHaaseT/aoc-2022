//
// Created by benjamin haase on 3/30/24.
//
#include <stdio.h>
#include <stdlib.h>

int compute_game_outcome(const char *opponent, const char *self);
int compute_game_score(const char *opponent, const char *self);
char get_turn(const char *opponent, const char *outcome);

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "usage: day2 <FILE>");
        exit(1);
    }

    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");
    char opponent;
    char self;
    int score = 0;

    while (fscanf(fp, "%c %c\n", &opponent, &self) == 2)
    {
        score += compute_game_outcome(&opponent, &self);
    }

    if (!feof(fp) || ferror(fp))
    {
        fprintf(stderr, "error reading from file %s\n", fname);
        exit(1);
    }

    printf("1st strategy: %d\n", score);

    rewind(fp);
    score = 0;
    char outcome;

    while (fscanf(fp, "%c %c\n", &opponent, &outcome) == 2)
    {
        self = get_turn(&opponent, &outcome);
        score += compute_game_score(&opponent, &self);
    }

    if (!feof(fp) || ferror(fp))
    {
        fprintf(stderr, "error reading from file %s\n", fname);
        exit(1);
    }

    printf("2nd strategy: %d\n", score);

    fclose(fp);

    return EXIT_SUCCESS;
}

int compute_game_outcome(const char *opponent, const char *self)
{
    int score = 0;
    // we draw
    if (
            (*opponent == 'A' && *self == 'X') ||
            (*opponent == 'B' && *self == 'Y') ||
            (*opponent == 'C' && *self == 'Z')
            )
    {
        score += 3;
    }
    else if (
            (*opponent == 'A' && *self == 'Y') ||
            (*opponent == 'B' && *self == 'Z') ||
            (*opponent == 'C' && *self == 'X')
            )
    {
        score += 6;
    }

//    printf("%c %d\n", *self, (*self % 'X'));

    return score + ((*self % 'X') + 1);
}

int compute_game_score(const char *opponent, const char *self)
{
    int score = 0;
    // we draw
    if (
            (*opponent == 'A' && *self == 'A') ||
            (*opponent == 'B' && *self == 'B') ||
            (*opponent == 'C' && *self == 'C')
            )
    {
        score += 3;
    }
    else if (
            (*opponent == 'A' && *self == 'B') ||
            (*opponent == 'B' && *self == 'C') ||
            (*opponent == 'C' && *self == 'A')
            )
    {
        score += 6;
    }

//    printf("%c %d\n", *self, (*self % 'X'));

    return score + ((*self % 'A') + 1);
}

char get_turn(const char *opponent, const char *outcome)
{
    if (*opponent == 'A')
    {
        switch (*outcome)
        {
            case 'X':
                return 'C';
            case 'Y':
                return 'A';
            default:
                return 'B';
        }
    }
    else if (*opponent == 'B')
    {
        switch (*outcome)
        {
            case 'X':
                return 'A';
            case 'Y':
                return 'B';
            default:
                return 'C';
        }
    }
    else
    {
        switch (*outcome)
        {
            case 'X':
                return 'B';
            case 'Y':
                return 'C';
            default:
                return 'A';
        }
    }
}
