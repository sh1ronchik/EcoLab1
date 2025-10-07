/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#if defined(_MSC_VER)
 #define CDECL __cdecl
#else
 #define CDECL
#endif

/*
 *
 * <сводка>
 *   Простая LCG для детерминированных "случайных" значений
 * </сводка>
 *
 * <описание>
 *   Линейный конгруэнтный генератор — используется внутри init-функций,
 *   чтобы не зависеть от внешних заголовков/реализаций rand/srand.
 * </описание>
 *
 */
static unsigned long lcg_seed = 1;
static void lcg_srand(unsigned long s) {
    lcg_seed = s;
}
static int lcg_rand(void) {
    lcg_seed = lcg_seed * 1103515245UL + 12345UL;
    return (int)((lcg_seed >> 16) & 0x7FFF);
}

/*
 *
 * <сводка>
 *   Инициализация массива int32_t
 * </сводка>
 *
 * <описание>
 *   Выделяет массив int32_t размера size через pIMem и заполняет некоторыми значениями.
 *   Возвращает указатель на выделенный массив (int32_t*).
 * </описание>
 *
 */
void *initIntArr(IEcoMemoryAllocator1 *pIMem, size_t size) {
    int32_t *arr = 0;
    size_t i;
    arr = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(size * sizeof(int32_t)));
    if (arr == 0) return 0;
    for (i = 0; i < size; ++i) {
        arr[i] = (int32_t)(((i * 37) % 21) - 10); /* значения в диапазоне примерно [-10,10] */
    }
    return arr;
}

/*
 *
 * <сводка>
 *   Инициализация массива строк
 * </сводка>
 *
 * <описание>
 *   Возвращает char** — массив указателей на строки. Каждая строка выделяется
 *   через pIMem и заполняется случайными буквами (LCG).
 * </описание>
 *
 */
void *initStringArr(IEcoMemoryAllocator1 *pIMem, size_t size) {
    size_t i, j, str_size;
    char **arr = (char **) pIMem->pVTbl->Alloc(pIMem, (uint32_t)(size * sizeof(char *)));
    if (arr == 0) return 0;
    lcg_srand(20241004UL);
    for (i = 0; i < size; i++) {
        arr[i] = (char *) pIMem->pVTbl->Alloc(pIMem, (uint32_t)(30 * sizeof(char)));
        if (arr[i] == 0) {
            /* в случае ошибки — освободим уже выделенные и вернём 0 */
            size_t k;
            for (k = 0; k < i; ++k) pIMem->pVTbl->Free(pIMem, arr[k]);
            pIMem->pVTbl->Free(pIMem, arr);
            return 0;
        }
        str_size = (size_t)(lcg_rand() % 15 + 5); /* длина 5..19 */
        for (j = 0; j < str_size; ++j) {
            int r = lcg_rand() % ('z' - 'a' + 1);
            arr[i][j] = (char)(r + 'a');
        }
        arr[i][str_size] = 0;
    }
    return arr;
}

/*
 *
 * <сводка>
 *   Вспомогательные функции для вывода массивов
 * </сводка>
 *
 */
static void printIntArr(const int32_t *arr, size_t n) {
    size_t i;
    printf("[");
    for (i = 0; i < n; ++i) {
        if (i) printf(", ");
        printf("%d", arr[i]);
    }
    printf("]\n");
}
static void printStringArr(char **arr, size_t n) {
    size_t i;
    printf("[");
    for (i = 0; i < n; ++i) {
        if (i) printf(", ");
        printf("\"%s\"", arr[i]);
    }
    printf("]\n");
}

/* comparators for qsort */
static int CDECL compareIntQ(const void *a, const void *b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
}
static int CDECL compareStringQ(const void *a, const void *b) {
    const char * const *sa = (const char * const *)a;
    const char * const *sb = (const char * const *)b;
    return strcmp(*sa, *sb);
}


/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
	/* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
	/* Указатель на интерфейс работы с системной шиной */
    IEcoInterfaceBus1* pIBus = 0;
	/* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
	/* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;

	/* Проверка и создание системного интрефейса */
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
    if (result != 0 || pISys == 0) {
        printf("ERROR: failed to obtain IEcoSystem1\n");
		/* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

	/* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
		/* Освобождение в случае ошибки */
        printf("ERROR: failed to obtain IEcoInterfaceBus1\n");
        goto Release;
    }

#ifdef ECO_LIB
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0) {
        printf("ERROR: component registration failed\n");
        goto Release;
    }
#endif

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    if (result != 0 || pIMem == 0) {
        printf("ERROR: failed to obtain IEcoMemoryAllocator1\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        printf("ERROR: failed to obtain IEcoLab1\n");
        goto Release;
    }

    /* ---------- TEST 1: int32_t ---------- */
    {
        int32_t *src = 0;
        int32_t *arr2 = 0;
        size_t n = 100000;
        size_t i;
        int ok_csort = 0;
        int ok_qsort = 0;
        clock_t t0, t1, tq0, tq1;
        double elapsed_csort, elapsed_qsort;

        src = (int32_t*)initIntArr(pIMem, n);
        if (src == 0) { printf("ERROR: initIntArr failed\n"); goto Release; }

        /* копия для qsort (делаем перед сортировкой pIEcoLab1) */
        arr2 = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (arr2 == 0) { pIMem->pVTbl->Free(pIMem, src); printf("ERROR: alloc arr2 failed\n"); goto Release; }
        for (i = 0; i < n; ++i) arr2[i] = src[i];

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortInt(pIEcoLab1, src, n);
        t1 = clock();
        elapsed_csort = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST INT: csortInt returned error %d\n", result);
        } else {
            ok_csort = 1;
            for (i = 1; i < n; ++i) {
                if (src[i-1] > src[i]) { ok_csort = 0; break; }
            }
            printf("TEST INT: csort %s (time = %.6f s)\n", ok_csort ? "PASS" : "FAIL", elapsed_csort);
        }

        /* qsort on arr2 */
        tq0 = clock();
        qsort(arr2, n, sizeof(int32_t), compareIntQ);
        tq1 = clock();
        elapsed_qsort = (double)(tq1 - tq0) / (double)CLOCKS_PER_SEC;

        ok_qsort = 1;
        for (i = 1; i < n; ++i) {
            if (arr2[i-1] > arr2[i]) { ok_qsort = 0; break; }
        }
        printf("TEST INT: qsort %s (time = %.6f s)\n\n", ok_qsort ? "PASS" : "FAIL", elapsed_qsort);

        pIMem->pVTbl->Free(pIMem, src);
        pIMem->pVTbl->Free(pIMem, arr2);
    }

    /* ---------- TEST 4: string ---------- */
    {
        char **src = 0;
        char **arr2 = 0;
        size_t n = 100000;
        size_t i;
        int ok_csort = 0;
        int ok_qsort = 0;
        clock_t t0, t1, tq0, tq1;
        double elapsed_csort, elapsed_qsort;

        src = (char**)initStringArr(pIMem, n);
        if (src == 0) { printf("ERROR: initStringArr failed\n"); goto Release; }

        /* копия указателей для qsort */
        arr2 = (char**)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(char*)));
        if (arr2 == 0) {
            for (i = 0; i < n; ++i) pIMem->pVTbl->Free(pIMem, src[i]);
            pIMem->pVTbl->Free(pIMem, src);
            printf("ERROR: alloc arr2 failed\n");
            goto Release;
        }
        for (i = 0; i < n; ++i) arr2[i] = src[i];

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortString(pIEcoLab1, src, n);
        t1 = clock();
        elapsed_csort = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST string: csortString returned error %d\n", result);
        } else {
            ok_csort = 1;
            for (i = 1; i < n; ++i) {
                if (strcmp(src[i-1], src[i]) > 0) { ok_csort = 0; break; }
            }
            printf("TEST string: csort %s (time = %.6f s)\n", ok_csort ? "PASS" : "FAIL", elapsed_csort);
        }

        tq0 = clock();
        qsort(arr2, n, sizeof(char*), compareStringQ);
        tq1 = clock();
        elapsed_qsort = (double)(tq1 - tq0) / (double)CLOCKS_PER_SEC;

        ok_qsort = 1;
        for (i = 1; i < n; ++i) {
            if (strcmp(arr2[i-1], arr2[i]) > 0) { ok_qsort = 0; break; }
        }
        printf("TEST string: qsort %s (time = %.6f s)\n\n", ok_qsort ? "PASS" : "FAIL", elapsed_qsort);

        for (i = 0; i < n; ++i) pIMem->pVTbl->Free(pIMem, src[i]);
        pIMem->pVTbl->Free(pIMem, src);
        pIMem->pVTbl->Free(pIMem, arr2);
    }

Release:

	/* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) pIBus->pVTbl->Release(pIBus);

	/* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) pIMem->pVTbl->Release(pIMem);

	/* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) pIEcoLab1->pVTbl->Release(pIEcoLab1);

	/* Освобождение системного интерфейса */
    if (pISys != 0) pISys->pVTbl->Release(pISys);

    return result;
}