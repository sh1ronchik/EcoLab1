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

#include "IEcoCalculatorY.h"
#include "IEcoCalculatorX.h"

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
static void lcg_srand(unsigned long s) { lcg_seed = s; }
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
    if (pIMem == 0) return 0;
    arr = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(size * sizeof(int32_t)));
    if (arr == 0) return 0;
    for (i = 0; i < size; ++i) {
        arr[i] = (int32_t)(((i * 37) % 21) - 10);
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

/* comparators for qsort */
static int CDECL compareIntQ(const void *a, const void *b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    if (va < vb) return -1;
    if (va > vb) return 1;
    return 0;
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

	IEcoCalculatorX* pIX = 0;
	IEcoCalculatorY* pIY = 0;


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

	/* ========== ДЕМОНСТРАЦИЯ ВКЛЮЧЕНИЯ/АГРЕГИРОВАНИЯ ВСЕХ КОМПОНЕНТОВ ========== */
	{
		IEcoCalculatorX* pICalcX = 0;
		IEcoCalculatorY* pICalcY = 0;
		IEcoLab1* pILab_back = 0;
		int32_t add_res = 0;
		int16_t sub_res = 0;

		printf("\n=== TEST: Demonstration of inclusion and aggregation mechanisms ===\n\n");

		/* Test 1: AGGREGATION of component B */
		printf("Test 1: Aggregation of component B (IEcoCalculatorX)\n");
		if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pICalcX) == 0 && pICalcX != 0) {
			printf("\n%p\n\n", pICalcX);
			add_res = pICalcX->pVTbl->Addition(pICalcX, 15, 7);
			printf("\n%d\n\n", add_res);
			sub_res = pICalcX->pVTbl->Subtraction(pICalcX, 15, 7);
			printf("  Addition(15, 7) = %d (component B - aggregation)\n", add_res);
			printf("  Subtraction(15, 7) = %d (component B - aggregation)\n", sub_res);
			printf("  Mechanism: component B's interface is passed directly to the client\n\n");
			pICalcX->pVTbl->Release(pICalcX);
			pICalcX = 0;
		}

		/* Test 2: INCLUSION of components D and E */
		printf("Test 2: Inclusion of components D and E (IEcoCalculatorY)\n");
		if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorY, (void**)&pICalcY) == 0 && pICalcY != 0) {
			int32_t mul_res = pICalcY->pVTbl->Multiplication(pICalcY, 8, 4);
			int16_t div_res = pICalcY->pVTbl->Division(pICalcY, 20, 5);
			printf("  Multiplication(8, 4) = %d (component D - inclusion)\n", mul_res);
			printf("  Division(20, 5) = %d (component E - inclusion)\n", div_res);
			printf("  Mechanism: EcoLab1 calls methods of internal components\n\n");
			pICalcY->pVTbl->Release(pICalcY);
			pICalcY = 0;
		}

		/* Test 3: Demonstration that any interface can produce another */
		printf("Test 3: QueryInterface property - obtaining interfaces\n");
		if (pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pICalcX) == 0 && pICalcX != 0) {
			printf("  IEcoLab1 -> IEcoCalculatorX: OK\n");
        
			if (pICalcX->pVTbl->QueryInterface(pICalcX, &IID_IEcoCalculatorY, (void**)&pICalcY) == 0 && pICalcY != 0) {
				printf("  IEcoCalculatorX -> IEcoCalculatorY: OK\n");
            
				pILab_back = 0;
				if (pICalcY->pVTbl->QueryInterface(pICalcY, &IID_IEcoLab1, (void**)&pILab_back) == 0 && pILab_back != 0) {
					printf("  IEcoCalculatorY -> IEcoLab1: OK\n");
					printf("  Conclusion: any interface can obtain any other interface!\n\n");
					pILab_back->pVTbl->Release(pILab_back);
				}
				pICalcY->pVTbl->Release(pICalcY);
			}
			pICalcX->pVTbl->Release(pICalcX);
		}

		printf("=== SUMMARY: All 5 components are engaged ===\n");
		printf("Component A: included (used in Addition via component B)\n");
		printf("Component B: AGGREGATED (IEcoCalculatorX passed directly)\n");
		printf("Component C: included (used in Subtraction via component B)\n");
		printf("Component D: INCLUDED (Multiplication calls its method)\n");
		printf("Component E: INCLUDED (Division calls its method)\n\n");
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
