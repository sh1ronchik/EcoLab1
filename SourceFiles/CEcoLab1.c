	/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoLab1
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1.h"

/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoLab1
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1_QueryInterface(IEcoLab1Ptr_t me, const UGUID* riid, void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    int16_t result = -1;

    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    if (IsEqualUGUID(riid, &IID_IEcoLab1)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
        result = 0;
    }
    else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        /* Агрегация B */
        if (pCMe->m_pAggregatedCalcX != 0) {
            /* Передача интерфейса B */
            *ppv = pCMe->m_pAggregatedCalcX;
            pCMe->m_pAggregatedCalcX->pVTbl->AddRef(pCMe->m_pAggregatedCalcX);
            result = 0;
        }
        else {
            /* Включение компонентов - возвращаем нашу vtable для IEcoCalculatorX */
            *ppv = &pCMe->m_pVTblIEcoCalculatorX;
            pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
            result = 0;
        }
    }
    else if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        /* Возврат нашей vtable для IEcoCalculatorY */
        *ppv = &pCMe->m_pVTblIEcoCalculatorY;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
        result = 0;
    }
    else if (IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
        pCMe->m_pVTblIEcoLab1->AddRef((IEcoLab1*)pCMe);
        result = 0;
    }
    else {
        *ppv = 0;
        result = ERR_ECO_NOINTERFACE;
    }

    return result;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoLab1
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_AddRef(IEcoLab1Ptr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    if (me == 0) {
        return -1;
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoLab1
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoLab1_Release(IEcoLab1Ptr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)me;

    if (me == 0) {
        return -1;
    }

    --pCMe->m_cRef;

    if (pCMe->m_cRef == 0) {
        deleteCEcoLab1((IEcoLab1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/* QueryInterface для IEcoCalculatorX */
static int16_t ECOCALLMETHOD CEcoLab1_CalcX_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*));
    return CEcoLab1_QueryInterface((IEcoLab1*)pCMe, riid, ppv);
}

/* AddRef для IEcoCalculatorX */
static uint32_t ECOCALLMETHOD CEcoLab1_CalcX_AddRef(IEcoCalculatorXPtr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*));
    return CEcoLab1_AddRef((IEcoLab1*)pCMe);
}

/* Release для IEcoCalculatorX */
static uint32_t ECOCALLMETHOD CEcoLab1_CalcX_Release(IEcoCalculatorXPtr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*));
    return CEcoLab1_Release((IEcoLab1*)pCMe);
}

/* QueryInterface для IEcoCalculatorY */
static int16_t ECOCALLMETHOD CEcoLab1_CalcY_QueryInterface(IEcoCalculatorYPtr_t me, const UGUID* riid, void** ppv) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*) - sizeof(struct IEcoCalculatorX*));
    return CEcoLab1_QueryInterface((IEcoLab1*)pCMe, riid, ppv);
}

/* AddRef для IEcoCalculatorY */
static uint32_t ECOCALLMETHOD CEcoLab1_CalcY_AddRef(IEcoCalculatorYPtr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*) - sizeof(struct IEcoCalculatorX*));
    return CEcoLab1_AddRef((IEcoLab1*)pCMe);
}

/* Release для IEcoCalculatorY */
static uint32_t ECOCALLMETHOD CEcoLab1_CalcY_Release(IEcoCalculatorYPtr_t me) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*) - sizeof(struct IEcoCalculatorX*));
    return CEcoLab1_Release((IEcoLab1*)pCMe);
}

/* Прототипы (только int и string) */
static int16_t ECOCALLMETHOD CEcoLab1_csort(/* in */ IEcoLab1Ptr_t me, void *arrPrt, size_t arrSize, size_t elemSize);
static int16_t ECOCALLMETHOD CEcoLab1_csortInt(/* in */ IEcoLab1Ptr_t me, int32_t *arr, size_t arrSize);

/* Counting sort для int32_t (устойчивая) */
static int16_t ECOCALLMETHOD CEcoLab1_csortInt(IEcoLab1Ptr_t me, int32_t *arr, size_t arrSize) {
    CEcoLab1* pCMe = (CEcoLab1*)me;
    IEcoMemoryAllocator1* pIMem;
    int32_t *outArr;
    size_t i, idx;
    int32_t minv = 0, maxv = 0;
    unsigned long range = 0;
    uint32_t *counts;

    if (me == 0 || arr == 0) {
        return -1;
    }

    pIMem = pCMe->m_pIMem;
    if (pIMem == 0) {
        return -1;
    }

    if (arrSize == 0) {
        return 0;
    }

    /* Найдём min и max */
    minv = maxv = arr[0];
    for (i = 1; i < arrSize; i++) {
        if (arr[i] < minv) minv = arr[i];
        if (arr[i] > maxv) maxv = arr[i];
    }

    /* Диапазон значений */
    range = (unsigned long)((unsigned long)maxv - (unsigned long)minv + 1UL);
    if (range == 0 || range > (1UL << 28)) {
        return -1;
    }

    /* Выделим массив counts */
    counts = (uint32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(range * sizeof(uint32_t)));
    if (counts == 0) {
        return -1;
    }
    for (idx = 0; idx < range; idx++) counts[idx] = 0;

    /* Подсчёт */
    for (i = 0; i < arrSize; i++) {
        counts[(unsigned long)(arr[i] - minv)]++;
    }

    /* Выделим временный буфер для отсортированного массива */
    outArr = (int32_t*)pIMem->pVTbl->Alloc(pIMem, (uint32_t)(arrSize * sizeof(int32_t)));
    if (outArr == 0) {
        pIMem->pVTbl->Free(pIMem, counts);
        return -1;
    }

    /* Заполним outArr в отсортированном порядке */
    {
        unsigned long k = 0;
        for (idx = 0; idx < range; idx++) {
            uint32_t times = counts[idx];
            int32_t value = (int32_t)((long)idx + (long)minv);
            while (times--) {
                outArr[k++] = value;
            }
        }
    }

    /* Скопируем обратно в исходный массив */
    for (i = 0; i < arrSize; i++) {
        arr[i] = outArr[i];
    }

    /* Освобождение */
    pIMem->pVTbl->Free(pIMem, counts);
    pIMem->pVTbl->Free(pIMem, outArr);

    return 0;
}

/* Обёртка: прежняя сигнатура остаётся — диспетчер только по int32_t и char* */
int16_t ECOCALLMETHOD CEcoLab1_csort(IEcoLab1Ptr_t me, void *arrPrt, size_t arrSize, size_t elemSize) {
    if (me == 0 || arrPrt == 0) return -1;
    if (arrSize == 0) return 0;

    if (elemSize == sizeof(int32_t)) {
        return CEcoLab1_csortInt(me, (int32_t*)arrPrt, arrSize);
    } else {
        return -1;
    }
}

/* ========== Реализация методов IEcoCalculatorX через ВКЛЮЧЕНИЕ ========== */

/* Сложение - используем ВКЛЮЧЕНИЕ компонента A */
int32_t ECOCALLMETHOD CEcoLab1_Addition(IEcoCalculatorXPtr_t me, int16_t a, int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*));
    int32_t result = 0;

    if (me == 0) {
        return -1;
    }

	/* ВКЛЮЧЕНИЕ: вызываем метод внутреннего компонента A */
    if (pCMe->m_pContainedCalcX_A != 0) {
        result = pCMe->m_pContainedCalcX_A->pVTbl->Addition(pCMe->m_pContainedCalcX_A, a, b);
    }

    return result;
}

/* Вычитание - используем ВКЛЮЧЕНИЕ компонента C */
int16_t ECOCALLMETHOD CEcoLab1_Subtraction(IEcoCalculatorXPtr_t me, int16_t a, int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*));
    int16_t result = 0;

    if (me == 0) {
        return -1;
    }

	/* ВКЛЮЧЕНИЕ: вызываем метод внутреннего компонента C */
    if (pCMe->m_pContainedCalcX_C != 0) {
        result = pCMe->m_pContainedCalcX_C->pVTbl->Subtraction(pCMe->m_pContainedCalcX_C, a, b);
    }

    return result;
}

/* ========== Реализация методов IEcoCalculatorY через ВКЛЮЧЕНИЕ ========== */

/* Умножение - используем ВКЛЮЧЕНИЕ компонента D */
int32_t ECOCALLMETHOD CEcoLab1_Multiplication(IEcoCalculatorYPtr_t me, int16_t a, int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*) - sizeof(struct IEcoCalculatorX*));
    int32_t result = 0;

    if (me == 0) {
        return -1;
    }

	/* ВКЛЮЧЕНИЕ: вызываем метод внутреннего компонента D */
    if (pCMe->m_pContainedCalcY_D != 0) {
        result = pCMe->m_pContainedCalcY_D->pVTbl->Multiplication(pCMe->m_pContainedCalcY_D, a, b);
    }

    return result;
}

/* Деление - используем ВКЛЮЧЕНИЕ компонента E */
int16_t ECOCALLMETHOD CEcoLab1_Division(IEcoCalculatorYPtr_t me, int16_t a, int16_t b) {
    CEcoLab1* pCMe = (CEcoLab1*)((uint64_t)me - sizeof(struct IEcoLab1*) - sizeof(struct IEcoCalculatorX*));
    int16_t result = 0;

    if (me == 0) {
        return -1;
    }

	/* ВКЛЮЧЕНИЕ: вызываем метод внутреннего компонента E */
    if (pCMe->m_pContainedCalcY_E != 0) {
        result = pCMe->m_pContainedCalcY_E->pVTbl->Division(pCMe->m_pContainedCalcY_E, a, b);
    }

    return result;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция инициализации экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD initCEcoLab1(IEcoLab1Ptr_t me, struct IEcoUnknown *pIUnkSystem) {
    IEcoUnknown* pOuterUnknown = (IEcoUnknown*)me;
    CEcoLab1* pCMe = (CEcoLab1*)me;
    IEcoInterfaceBus1* pIBus = 0;
    int16_t result = -1;

    if (me == 0) {
        return result;
    }

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = (IEcoSystem1*)pIUnkSystem;

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    if (result != 0 || pIBus == 0) {
        return result;
    }

    /* Инициализируем указатели */
    pCMe->m_pAggregatedCalcX = 0;
    pCMe->m_pContainedCalcX_A = 0;
    pCMe->m_pContainedCalcX_C = 0;
    pCMe->m_pContainedCalcY_D = 0;
    pCMe->m_pContainedCalcY_E = 0;

    /* АГРЕГИРОВАНИЕ: Пытаемся получить компонент B */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorB, pOuterUnknown, &IID_IEcoCalculatorX, (void**)&pCMe->m_pAggregatedCalcX);
    /* Если не получилось - не критично, будем использовать включение A и C */

    /* ВКЛЮЧЕНИЕ: Получаем компонент A для операции Addition */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorA, 0, &IID_IEcoCalculatorX, (void**)&pCMe->m_pContainedCalcX_A);

    /* ВКЛЮЧЕНИЕ: Получаем компонент C для операции Subtraction */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorC, 0, &IID_IEcoCalculatorX, (void**)&pCMe->m_pContainedCalcX_C);

    /* ВКЛЮЧЕНИЕ: Получаем компонент D для операции Multiplication */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorD, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pContainedCalcY_D);

    /* ВКЛЮЧЕНИЕ: Получаем компонент E для операции Division */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoCalculatorE, 0, &IID_IEcoCalculatorY, (void**)&pCMe->m_pContainedCalcY_E);

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/* Создание таблиц виртуальных функций */

/* Create Virtual Table IEcoLab1 */
IEcoLab1VTbl g_x277FC00C35624096AFCFC125B94EEC90VTbl = {
    CEcoLab1_QueryInterface,
    CEcoLab1_AddRef,
    CEcoLab1_Release,
    CEcoLab1_csort,        /* общий (совместимость) */
    CEcoLab1_csortInt,     /* специализированный для int32_t */
};


/* Virtual Table для IEcoCalculatorX */
IEcoCalculatorXVTbl g_xCalcXVTbl = {
    CEcoLab1_CalcX_QueryInterface,
    CEcoLab1_CalcX_AddRef,
    CEcoLab1_CalcX_Release,
    CEcoLab1_Addition,
    CEcoLab1_Subtraction,
};

/* Virtual Table для IEcoCalculatorY */
IEcoCalculatorYVTbl g_xCalcYVTbl = {
    CEcoLab1_CalcY_QueryInterface,
    CEcoLab1_CalcY_AddRef,
    CEcoLab1_CalcY_Release,
    CEcoLab1_Multiplication,
    CEcoLab1_Division,
};

/*
 *
 * <сводка>
 *   Функция Create
 * </сводка>
 *
 * <описание>
 *   Функция создания экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD createCEcoLab1(IEcoUnknown* pIUnkSystem, IEcoUnknown* pIUnkOuter, IEcoLab1** ppIEcoLab1) {
    int16_t result = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoLab1* pCMe = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    if (ppIEcoLab1 == 0 || pIUnkSystem == 0) {
        return result;
    }

    /* Получение системного интерфейса приложения */
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void **)&pISys);

    if (result != 0 || pISys == 0) {
        return result;
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);

    /* Получение идентификатора компонента для работы с памятью */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Получение интерфейса распределителя памяти */
    pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    if (result != 0 || pIMem == 0) {
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Выделение памяти для данных экземпляра */
    pCMe = (CEcoLab1*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab1));

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = pISys;

    /* Сохранение указателя на интерфейс для работы с памятью */
    pCMe->m_pIMem = pIMem;

    /* Установка счетчика ссылок на компонент */
    pCMe->m_cRef = 1;

    /* Создание таблиц функций интерфейсов */
	pCMe->m_pVTblIEcoLab1 = &g_x277FC00C35624096AFCFC125B94EEC90VTbl;
    pCMe->m_pVTblIEcoCalculatorX = &g_xCalcXVTbl;
    pCMe->m_pVTblIEcoCalculatorY = &g_xCalcYVTbl;

    /* Инициализация данных */
    pCMe->m_Name = 0;

    /* Возврат указателя на интерфейс */
    *ppIEcoLab1 = (IEcoLab1*)pCMe;

    /* Освобождение */
    pIBus->pVTbl->Release(pIBus);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 *
 * <описание>
 *   Функция освобождения экземпляра
 * </описание>
 *
 */
void ECOCALLMETHOD deleteCEcoLab1(IEcoLab1* pIEcoLab1) {
    CEcoLab1* pCMe = (CEcoLab1*)pIEcoLab1;
    IEcoMemoryAllocator1* pIMem = 0;

    if (pIEcoLab1 != 0) {
        pIMem = pCMe->m_pIMem;
        
        /* Освобождение */
        if (pCMe->m_Name != 0) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        
        if (pCMe->m_pISys != 0) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }

        /* Освобождаем агрегированный компонент */
        if (pCMe->m_pAggregatedCalcX != 0) {
            pCMe->m_pAggregatedCalcX->pVTbl->Release(pCMe->m_pAggregatedCalcX);
        }

        /* Освобождаем включенные компоненты */
        if (pCMe->m_pContainedCalcX_A != 0) {
            pCMe->m_pContainedCalcX_A->pVTbl->Release(pCMe->m_pContainedCalcX_A);
        }
        if (pCMe->m_pContainedCalcX_C != 0) {
            pCMe->m_pContainedCalcX_C->pVTbl->Release(pCMe->m_pContainedCalcX_C);
        }
        if (pCMe->m_pContainedCalcY_D != 0) {
            pCMe->m_pContainedCalcY_D->pVTbl->Release(pCMe->m_pContainedCalcY_D);
        }
        if (pCMe->m_pContainedCalcY_E != 0) {
            pCMe->m_pContainedCalcY_E->pVTbl->Release(pCMe->m_pContainedCalcY_E);
        }

        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}