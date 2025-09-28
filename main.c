/*
 * Programa de Cadastro de Alunos
 *
 * Este programa em C permite cadastrar até cinco alunos, ler seus nomes
 * e três notas, calcular a média para cada aluno, determinar sua situação
 * (Aprovado se média >= 7, Reprovado caso contrário), exibir uma tabela
 * com os dados e identificar o aluno com a maior média. Foi estruturado
 * com vetores para armazenar nomes e notas, uma função dedicada para
 * calcular a média e uso de estruturas de repetição e decisão conforme
 * os requisitos do exercício.
 * Aceitar somente strings no nome do aluno (sem dígitos). Caso haja dígito,
 *    exibe um alerta e solicita novamente.
 * Aceitar decimais com ponto ou vírgula (ex.: 2.5 ou 2,5). A leitura é feita
 *    como string, a vírgula é convertida para ponto e depois ocorre o parse.
 * Autores: Diego Fonseca, Luís, Miguel, Miruna Onofrei, Pedro Kons, Tiago Mendes
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_STUDENTS 5
#define NUM_GRADES   3
#define NAME_LEN     50
#define LINE_LEN     128

/* Remove o '\n' final de fgets, se existir */
static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n > 0 && s[n - 1] == '\n') s[n - 1] = '\0';
}

/* Substitui todos os 'from' por 'to' na string */
static void replace_char(char *s, char from, char to) {
    if (!s) return;
    for (; *s; ++s) {
        if (*s == from) *s = to;
    }
}

/* Lê uma linha segura com fgets e remove newline */
static int read_line(char *buffer, size_t cap) {
    if (!fgets(buffer, (int)cap, stdin)) return 0;
    trim_newline(buffer);
    return 1;
}

/* Valida que o nome não contém dígitos; permite letras, espaços, hífens e apóstrofos */
static int is_valid_name(const char *name) {
    if (!name || !*name) return 0;
    for (const char *p = name; *p; ++p) {
        unsigned char c = (unsigned char)*p;
        if (isdigit(c)) return 0; /* contém número -> inválido */
        /* permite letras, espaço, hífen, apóstrofo e acentos (>=128 considerado ok) */
        if (!(isalpha(c) || isspace(c) || c=='-' || c=='\'' || (c & 0x80))) {
            /* caracteres estranhos também são permitidos em nomes reais; ajuste se desejar */
        }
    }
    return 1;
}

/* Faz o parse de float aceitando vírgula ou ponto como separador decimal */
static int parse_float_loose(const char *text, float *out_value) {
    if (!text || !*text || !out_value) return 0;
    char tmp[LINE_LEN];
    strncpy(tmp, text, sizeof(tmp));
    tmp[sizeof(tmp) - 1] = '\0';
    replace_char(tmp, ',', '.');

    char *endptr = NULL;
    float val = strtof(tmp, &endptr);

    /* Verifica se houve conversão válida e se não sobrou lixo significativo */
    if (endptr == tmp) return 0;                 /* nenhum dígito lido */
    while (*endptr && isspace((unsigned char)*endptr)) endptr++;
    if (*endptr != '\0') return 0;               /* caracteres não numéricos restantes */

    *out_value = val;
    return 1;
}

/* Calcula média aritmética das notas */
static float compute_average(const float grades[], int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; ++i) sum += grades[i];
    return sum / (float)n;
}

int main(void) {
    char names[MAX_STUDENTS][NAME_LEN];
    float grades[MAX_STUDENTS][NUM_GRADES];
    float averages[MAX_STUDENTS];
    char status[MAX_STUDENTS][20];
    int count = 0;

    /* Solicita quantidade de alunos */
    printf("Digite o número de alunos (até %d): ", MAX_STUDENTS);
    char line[LINE_LEN];
    if (!read_line(line, sizeof(line))) {
        printf("Entrada inválida. Encerrando.\n");
        return 1;
    }
    /* parse simples de inteiro */
    char *endp = NULL;
    long tmpCount = strtol(line, &endp, 10);
    while (*endp && isspace((unsigned char)*endp)) endp++;
    if (endp == line || *endp!='\0' || tmpCount < 1 || tmpCount > MAX_STUDENTS) {
        printf("Número inválido de alunos. Encerrando.\n");
        return 1;
    }
    count = (int)tmpCount;

    for (int i = 0; i < count; ++i) {
        /* Leitura e validação do nome (sem dígitos) */
        while (1) {
            printf("Digite o nome do aluno %d: ", i + 1);
            if (!read_line(names[i], sizeof(names[i]))) {
                printf("Entrada inválida. Encerrando.\n");
                return 1;
            }
            if (is_valid_name(names[i])) break;
            /* Alerta se houver números no nome */
            printf("ALERTA: o nome não deve conter números. Tente novamente.\n");
        }

        /* Instruções explícitas para as notas (conforme requisito) */
        printf("Digite a primeira nota e aperte Enter, digite a segunda nota e aperte Enter, assim sucessivamente.\n");

        for (int j = 0; j < NUM_GRADES; ++j) {
            while (1) {
                /* Prompts breves para cada nota, aceitando vírgula ou ponto */
                printf("Nota %d de %s: ", j + 1, names[i]);
                if (!read_line(line, sizeof(line))) {
                    printf("Entrada inválida. Encerrando.\n");
                    return 1;
                }
                float value = 0.0f;
                if (!parse_float_loose(line, &value)) {
                    printf("Entrada de nota inválida. Use números (ex.: 7.5 ou 7,5).\n");
                    continue;
                }
                grades[i][j] = value;
                break;
            }
        }

        /* Média e situação */
        averages[i] = compute_average(grades[i], NUM_GRADES);
        if (averages[i] >= 7.0f) {
            strcpy(status[i], "Aprovado");
        } else {
            strcpy(status[i], "Reprovado");
        }
    }

    /* Maior média */
    int best_idx = 0;
    for (int i = 1; i < count; ++i) {
        if (averages[i] > averages[best_idx]) best_idx = i;
    }

    /* Relatório */
    printf("\n%-20s %6s %6s %6s %7s %12s\n", "ALUNO", "N1", "N2", "N3", "MÉDIA", "SITUAÇÃO");
    for (int i = 0; i < count; ++i) {
        printf("%-20s %6.1f %6.1f %6.1f %7.1f %12s\n",
               names[i], grades[i][0], grades[i][1], grades[i][2], averages[i], status[i]);
    }

    printf("\nAluno com maior média: %s (%.1f)\n", names[best_idx], averages[best_idx]);
    return 0;
}
