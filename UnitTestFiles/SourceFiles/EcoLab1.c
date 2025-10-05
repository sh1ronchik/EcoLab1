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
 *   Вычисление рангов для массива float
 * </сводка>
 *
 * <описание>
 *   Для каждого элемента arr[i] вычисляет его ранг в упорядоченном наборе (0..n-1).
 *   Ранжирование стабильное: при равных значениях порядок определяется меньшим индексом.
 * </описание>
 *
 */
int computeRanksFloat(IEcoMemoryAllocator1 *pIMem, float *arr, size_t n, int32_t *ranks) {
    int32_t *idx = 0;
    size_t i, j;
    if (n == 0) return -1;
    idx = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
    if (idx == 0) return -1;
    for (i = 0; i < n; ++i) idx[i] = (int32_t)i;
    /* Стабильная сортировка индексов вставками по значению arr[idx[..]] */
    for (i = 1; i < n; ++i) {
        int32_t key = idx[i];
        j = i;
        while (j > 0 && (arr[(size_t)idx[j-1]] > arr[(size_t)key] ||
               (arr[(size_t)idx[j-1]] == arr[(size_t)key] && idx[j-1] > key))) {
            idx[j] = idx[j-1];
            --j;
        }
        idx[j] = key;
    }
    for (i = 0; i < n; ++i) {
        ranks[(size_t)idx[i]] = (int32_t)i;
    }
    pIMem->pVTbl->Free(pIMem, idx);
    return 0;
}

/*
 *
 * <сводка>
 *   Вычисление рангов для массива double
 * </сводка>
 *
 * <описание>
 *   Аналогично computeRanksFloat, но для double.
 * </описание>
 *
 */
int computeRanksDouble(IEcoMemoryAllocator1 *pIMem, double *arr, size_t n, int32_t *ranks) {
    int32_t *idx = 0;
    size_t i, j;
    if (n == 0) return -1;
    idx = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
    if (idx == 0) return -1;
    for (i = 0; i < n; ++i) idx[i] = (int32_t)i;
    for (i = 1; i < n; ++i) {
        int32_t key = idx[i];
        j = i;
        while (j > 0 && (arr[(size_t)idx[j-1]] > arr[(size_t)key] ||
               (arr[(size_t)idx[j-1]] == arr[(size_t)key] && idx[j-1] > key))) {
            idx[j] = idx[j-1];
            --j;
        }
        idx[j] = key;
    }
    for (i = 0; i < n; ++i) {
        ranks[(size_t)idx[i]] = (int32_t)i;
    }
    pIMem->pVTbl->Free(pIMem, idx);
    return 0;
}

/*
 *
 * <сводка>
 *   Вычисление рангов для массива строк (лексикографически)
 * </сводка>
 *
 * <описание>
 *   Стабильное ранжирование строк. Используется strcmp из рантайма.
 * </описание>
 *
 */
int computeRanksString(IEcoMemoryAllocator1 *pIMem, char **arr, size_t n, int32_t *ranks) {
    int32_t *idx = 0;
    size_t i, j;
    if (n == 0) return -1;
    idx = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
    if (idx == 0) return -1;
    for (i = 0; i < n; ++i) idx[i] = (int32_t)i;
    for (i = 1; i < n; ++i) {
        int32_t key = idx[i];
        j = i;
        while (j > 0) {
            int cmp = strcmp(arr[idx[j-1]], arr[key]);
            if (cmp > 0 || (cmp == 0 && idx[j-1] > key)) {
                idx[j] = idx[j-1];
                --j;
            } else break;
        }
        idx[j] = key;
    }
    for (i = 0; i < n; ++i) {
        ranks[(size_t)idx[i]] = (int32_t)i;
    }
    pIMem->pVTbl->Free(pIMem, idx);
    return 0;
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
	/* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
	/* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
	/* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;

	/* Проверка и создание системного интрефейса */
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
    if (result != 0 || pISys == 0) {
        printf("ОШИБКА: не удалось получить IEcoSystem1\n");
		/* Освобождение системного интерфейса в случае ошибки */
        goto Release;
    }

	/* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
		/* Освобождение в случае ошибки */
        printf("ОШИБКА: не удалось получить IEcoInterfaceBus1\n");
        goto Release;
    }

#ifdef ECO_LIB
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0) {
        printf("ОШИБКА: регистрация компонента не удалась\n");
        goto Release;
    }
#endif

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    if (result != 0 || pIMem == 0) {
        printf("ОШИБКА: не удалось получить IEcoMemoryAllocator1\n");
        goto Release;
    }

    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        printf("ОШИБКА: не удалось получить IEcoLab1\n");
        goto Release;
    }

    /* ---------- ТЕСТ 1: int32_t (n = 50) ---------- */
    {
        int32_t *src = 0;
        size_t n = 50;
        int32_t *keys = 0;
        int32_t *out_sorted = 0;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (int32_t*)initIntArr(pIMem, n);
        if (src == 0) { printf("ОШИБКА: initIntArr не выполнен\n"); goto Release; }

        printf("ТЕСТ INT: исходный массив (n=%u):\n", (unsigned) n);
        printIntArr(src, n);

        /* Для int просто сформируем ранги по значению (вместо масштабирования) */
        {
            int32_t *ranks = 0;
            ranks = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
            if (ranks == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
            for (i = 0; i < n; ++i) ranks[i] = 0;
            for (i = 0; i < n; ++i) {
                size_t j;
                for (j = 0; j < n; ++j) {
                    if (src[j] < src[i] || (src[j] == src[i] && (int32_t)j < (int32_t)i)) {
                        ranks[i]++;
                    }
                }
            }
            keys = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
            if (keys == 0) { pIMem->pVTbl->Free(pIMem, ranks); printf("ОШИБКА: выделение памяти\n"); goto Release; }
            for (i = 0; i < n; ++i) {
                keys[i] = (int32_t)((ranks[i] << 16) | (i & 0xFFFF));
            }
            pIMem->pVTbl->Free(pIMem, ranks);
        }

        /* call csort and measure time */
        t0 = clock();
        result = pIEcoLab1->pVTbl->csort(pIEcoLab1, keys, n, sizeof(int32_t));
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("ТЕСТ INT: csort вернул ошибку %d\n", result);
        } else {
            out_sorted = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
            if (out_sorted == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
            for (i = 0; i < n; ++i) {
                uint32_t idx = (uint32_t)keys[i] & 0xFFFF;
                out_sorted[i] = src[idx];
            }
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (out_sorted[i-1] > out_sorted[i]) { ok = 0; break; }
            }
            printf("ТЕСТ INT: результат:\n");
            printIntArr(out_sorted, n);
            printf("ТЕСТ INT: %s (время = %.6f с)\n\n", ok ? "PASS" : "FAIL", elapsed);
            pIMem->pVTbl->Free(pIMem, out_sorted);
        }

        pIMem->pVTbl->Free(pIMem, keys);
        pIMem->pVTbl->Free(pIMem, src);
    }

    /* ---------- ТЕСТ 2: float (n = 50) ---------- */
    {
        float *src = 0;
        size_t n = 50;
        int32_t *ranks = 0;
        int32_t *keys = 0;
        float *out_sorted = 0;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (float*)initFloatArr(pIMem, n);
        if (src == 0) { printf("ОШИБКА: initFloatArr не выполнен\n"); goto Release; }

        printf("ТЕСТ float: исходный массив (n=%u):\n", (unsigned) n);
        printFloatArr(src, n);

        ranks = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (ranks == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
        if (computeRanksFloat(pIMem, src, n, ranks) != 0) { printf("ОШИБКА: computeRanksFloat\n"); goto Release; }

        keys = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (keys == 0) { pIMem->pVTbl->Free(pIMem, ranks); printf("ОШИБКА: выделение памяти\n"); goto Release; }
        for (i = 0; i < n; ++i) keys[i] = (int32_t)((ranks[i] << 16) | (i & 0xFFFF));

        pIMem->pVTbl->Free(pIMem, ranks);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csort(pIEcoLab1, keys, n, sizeof(int32_t));
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("ТЕСТ float: csort вернул ошибку %d\n", result);
        } else {
            out_sorted = (float*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(float)));
            if (out_sorted == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
            for (i = 0; i < n; ++i) {
                uint32_t idx = (uint32_t)keys[i] & 0xFFFF;
                out_sorted[i] = src[idx];
            }
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (out_sorted[i-1] > out_sorted[i]) { ok = 0; break; }
            }
            printf("ТЕСТ float: результат:\n");
            printFloatArr(out_sorted, n);
            printf("ТЕСТ float: %s (время = %.6f с)\n\n", ok ? "PASS" : "FAIL", elapsed);
            pIMem->pVTbl->Free(pIMem, out_sorted);
        }

        pIMem->pVTbl->Free(pIMem, keys);
        pIMem->pVTbl->Free(pIMem, src);
    }

    /* ---------- ТЕСТ 3: double (n = 50) ---------- */
    {
        double *src = 0;
        size_t n = 50;
        int32_t *ranks = 0;
        int32_t *keys = 0;
        double *out_sorted = 0;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (double*)initDoubleArr(pIMem, n);
        if (src == 0) { printf("ОШИБКА: initDoubleArr не выполнен\n"); goto Release; }

        printf("ТЕСТ double: исходный массив (n=%u):\n", (unsigned) n);
        printDoubleArr(src, n);

        ranks = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (ranks == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
        if (computeRanksDouble(pIMem, src, n, ranks) != 0) { printf("ОШИБКА: computeRanksDouble\n"); goto Release; }

        keys = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (keys == 0) { pIMem->pVTbl->Free(pIMem, ranks); printf("ОШИБКА: выделение памяти\n"); goto Release; }
        for (i = 0; i < n; ++i) keys[i] = (int32_t)((ranks[i] << 16) | (i & 0xFFFF));

        pIMem->pVTbl->Free(pIMem, ranks);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csort(pIEcoLab1, keys, n, sizeof(int32_t));
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("ТЕСТ double: csort вернул ошибку %d\n", result);
        } else {
            out_sorted = (double*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(double)));
            if (out_sorted == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
            for (i = 0; i < n; ++i) {
                uint32_t idx = (uint32_t)keys[i] & 0xFFFF;
                out_sorted[i] = src[idx];
            }
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (out_sorted[i-1] > out_sorted[i]) { ok = 0; break; }
            }
            printf("ТЕСТ double: результат:\n");
            printDoubleArr(out_sorted, n);
            printf("ТЕСТ double: %s (время = %.6f с)\n\n", ok ? "PASS" : "FAIL", elapsed);
            pIMem->pVTbl->Free(pIMem, out_sorted);
        }

        pIMem->pVTbl->Free(pIMem, keys);
        pIMem->pVTbl->Free(pIMem, src);
    }

    /* ---------- ТЕСТ 4: string (n = 50) ---------- */
    {
        char **src = 0;
        size_t n = 50;
        int32_t *ranks = 0;
        int32_t *keys = 0;
        char **out_sorted = 0;
        size_t i;
        int ok;
        clock_t t0, t1;
        double elapsed;

        src = (char**)initStringArr(pIMem, n);

        printf("ТЕСТ строк: исходный массив (n=%u):\n", (unsigned) n);
        printStringArr(src, n);

        ranks = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (ranks == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
        if (computeRanksString(pIMem, src, n, ranks) != 0) { printf("ОШИБКА: computeRanksString\n"); goto Release; }

        keys = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(int32_t)));
        if (keys == 0) { pIMem->pVTbl->Free(pIMem, ranks); printf("ОШИБКА: выделение памяти\n"); goto Release; }
        for (i = 0; i < n; ++i) keys[i] = (int32_t)((ranks[i] << 16) | (i & 0xFFFF));

        pIMem->pVTbl->Free(pIMem, ranks);

        t0 = clock();
        result = pIEcoLab1->pVTbl->csort(pIEcoLab1, keys, n, sizeof(int32_t));
        t1 = clock();
        elapsed = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;

        if (result != 0) {
            printf("ТЕСТ строк: csort вернул ошибку %d\n", result);
        } else {
            out_sorted = (char**)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(n * sizeof(char*)));
            if (out_sorted == 0) { printf("ОШИБКА: выделение памяти\n"); goto Release; }
            for (i = 0; i < n; ++i) {
                uint32_t idx = (uint32_t)keys[i] & 0xFFFF;
                out_sorted[i] = src[idx];
            }
            ok = 1;
            for (i = 1; i < n; ++i) {
                if (strcmp(out_sorted[i-1], out_sorted[i]) > 0) { ok = 0; break; }
            }
            printf("ТЕСТ строк: результат:\n");
            printStringArr(out_sorted, n);
            printf("ТЕСТ строк: %s (время = %.6f с)\n\n", ok ? "PASS" : "FAIL", elapsed);
            pIMem->pVTbl->Free(pIMem, out_sorted);
        }

        /* Освобождение памяти */
        for (i = 0; i < n; ++i) pIMem->pVTbl->Free(pIMem, src[i]);
        pIMem->pVTbl->Free(pIMem, src);
        pIMem->pVTbl->Free(pIMem, keys);
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
