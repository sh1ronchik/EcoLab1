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
 *   Инициализация массива float
 * </сводка>
 *
 * <описание>
 *   Выделяет массив float размера size через pIMem и заполняет значениями.
 *   Возвращает указатель на выделенный массив (float*).
 * </описание>
 *
 */
void *initFloatArr(IEcoMemoryAllocator1 *pIMem, size_t size) {
    float *arr = 0;
    size_t i;
    arr = (float*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(size * sizeof(float)));
    if (arr == 0) return 0;
    lcg_srand(12345UL);
    for (i = 0; i < size; ++i) {
        int r = lcg_rand() % 2001; /* 0..2000 */
        arr[i] = ((float)r / 100.0f) - 10.0f; /* диапазон примерно [-10.00, 10.00] */
    }
    return arr;
}

/*
 *
 * <сводка>
 *   Инициализация массива double
 * </сводка>
 *
 * <описание>
 *   Выделяет массив double размера size через pIMem и заполняет значениями.
 *   Возвращает указатель на выделенный массив (double*).
 * </описание>
 *
 */
void *initDoubleArr(IEcoMemoryAllocator1 *pIMem, size_t size) {
    double *arr = 0;
    size_t i;
    arr = (double*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(size * sizeof(double)));
    if (arr == 0) return 0;
    lcg_srand(54321UL);
    for (i = 0; i < size; ++i) {
        int r = lcg_rand() % 200001; /* 0..200000 */
        arr[i] = ((double)r / 10000.0) - 10.0; /* диапазон примерно [-10.0000, 10.0000] */
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
static void printFloatArr(const float *arr, size_t n) {
    size_t i;
    printf("[");
    for (i = 0; i < n; ++i) {
        if (i) printf(", ");
        printf("%.6f", arr[i]);
    }
    printf("]\n");
}
static void printDoubleArr(const double *arr, size_t n) {
    size_t i;
    printf("[");
    for (i = 0; i < n; ++i) {
        if (i) printf(", ");
        printf("%.9f", arr[i]);
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
        size_t n = 500000;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (int32_t*)initIntArr(pIMem, n);
        if (src == 0) { printf("ERROR: initIntArr failed\n"); goto Release; }

        printf("TEST INT: source array (n=%u):\n", (unsigned) n);
        printIntArr(src, n);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortInt(pIEcoLab1, src, n);
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST INT: csortInt returned error %d\n", result);
        } else {
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (src[i-1] > src[i]) { ok = 0; break; }
            }
            printf("TEST INT: result:\n");
            printIntArr(src, n);
            printf("TEST INT: %s (time = %.6f s)\n\n", ok ? "PASS" : "FAIL", elapsed);
        }

        pIMem->pVTbl->Free(pIMem, src);
    }

    /* ---------- TEST 2: float ---------- */
    {
        float *src = 0;
        size_t n = 500000;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (float*)initFloatArr(pIMem, n);
        if (src == 0) { printf("ERROR: initFloatArr failed\n"); goto Release; }

        printf("TEST float: source array (n=%u):\n", (unsigned) n);
        printFloatArr(src, n);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortFloat(pIEcoLab1, src, n);
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST float: csortFloat returned error %d\n", result);
        } else {
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (src[i-1] > src[i]) { ok = 0; break; }
            }
            printf("TEST float: result:\n");
            printFloatArr(src, n);
            printf("TEST float: %s (time = %.6f s)\n\n", ok ? "PASS" : "FAIL", elapsed);
        }

        pIMem->pVTbl->Free(pIMem, src);
    }

    /* ---------- TEST 3: double ---------- */
    {
        double *src = 0;
        size_t n = 500000;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (double*)initDoubleArr(pIMem, n);
        if (src == 0) { printf("ERROR: initDoubleArr failed\n"); goto Release; }

        printf("TEST double: source array (n=%u):\n", (unsigned) n);
        printDoubleArr(src, n);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortDouble(pIEcoLab1, src, n);
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST double: csortDouble returned error %d\n", result);
        } else {
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (src[i-1] > src[i]) { ok = 0; break; }
            }
            printf("TEST double: result:\n");
            printDoubleArr(src, n);
            printf("TEST double: %s (time = %.6f s)\n\n", ok ? "PASS" : "FAIL", elapsed);
        }

        pIMem->pVTbl->Free(pIMem, src);
    }

    /* ---------- TEST 4: string ---------- */
    {
        char **src = 0;
        size_t n = 500000;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (char**)initStringArr(pIMem, n);
        if (src == 0) { printf("ERROR: initStringArr failed\n"); goto Release; }

        printf("TEST string: source array (n=%u):\n", (unsigned) n);
        printStringArr(src, n);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csortString(pIEcoLab1, src, n);
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("TEST string: csortString returned error %d\n", result);
        } else {
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (strcmp(src[i-1], src[i]) > 0) { ok = 0; break; }
            }
            printf("TEST string: result:\n");
            printStringArr(src, n);
            printf("TEST string: %s (time = %.6f s)\n\n", ok ? "PASS" : "FAIL", elapsed);
        }

        for (i = 0; i < n; ++i) pIMem->pVTbl->Free(pIMem, src[i]);
        pIMem->pVTbl->Free(pIMem, src);
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
