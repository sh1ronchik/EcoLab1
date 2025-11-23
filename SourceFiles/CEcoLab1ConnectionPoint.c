/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoLab1ConnectionPoint
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoLab1ConnectionPoint
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2016 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoLab1ConnectionPoint.h"
#include "CEcoLab1EnumConnections.h"

#define INITIAL_SINK_CAPACITY 4

/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1ConnectionPoint_QueryInterface(/* in */ struct IEcoConnectionPoint* me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;

    if (me == 0 || ppv == 0) {
        return -1;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoConnectionPoint) ) {
        *ppv = &pCMe->m_pVTblICP;
        pCMe->m_pVTblICP->AddRef((IEcoConnectionPoint*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblICP;
        pCMe->m_pVTblICP->AddRef((IEcoConnectionPoint*)pCMe);
    }
    else {
        *ppv = 0;
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoLab1ConnectionPoint_AddRef(/* in */ struct IEcoConnectionPoint* me) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;

    if (me == 0 ) {
        return -1;
    }

    return pCMe->m_pICPC->pVTbl->AddRef(pCMe->m_pICPC);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
uint32_t ECOCALLMETHOD CEcoLab1ConnectionPoint_Release(/* in */ struct IEcoConnectionPoint* me) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;

    if (me == 0 ) {
        return -1;
    }

    return pCMe->m_pICPC->pVTbl->Release(pCMe->m_pICPC);
}

/*
 *
 * <сводка>
 *   Функция GetConnectionInterface
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1ConnectionPoint_GetConnectionInterface(/* in */ struct IEcoConnectionPoint* me, /* out */ UGUID *pIID) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;

    if (me == 0 || pIID == 0) {
        return -1;
    }

    *pIID = *pCMe->m_piid;
    return 0;
}

/*
 *
 * <сводка>
 *   Функция GetConnectionPointContainer
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1ConnectionPoint_GetConnectionPointContainer(/* in */ struct IEcoConnectionPoint* me, /* out */ struct IEcoConnectionPointContainer **ppICPC) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;

    if (me == 0 || ppICPC == 0) {
        return -1;
    }

    *ppICPC = pCMe->m_pICPC;
    pCMe->m_pICPC->pVTbl->AddRef(pCMe->m_pICPC);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Advise
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1ConnectionPoint_Advise(/* in */ struct IEcoConnectionPoint* me, /* in */ struct IEcoUnknown *pUnkSink, /* out */ uint32_t *pcCookie) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;
    int16_t result = 0;
    EcoConnectionData* pCD = 0;

    if (me == 0 || pUnkSink == 0 || pcCookie == 0) {
        return -1;
    }

    pCD = (EcoConnectionData*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, sizeof(EcoConnectionData));

    result = pUnkSink->pVTbl->QueryInterface(pUnkSink, pCMe->m_piid, (void**)&pCD->pUnk);
    if (result == 0 && pCD->pUnk != 0) {
        pCD->cCookie = ++pCMe->m_cNextCookie;

        /* Проверка и увеличение ёмкости массива при необходимости */
        if (pCMe->m_cSinks >= pCMe->m_cSinksCapacity) {
            uint32_t newCapacity = pCMe->m_cSinksCapacity * 2;
            EcoConnectionData** pNewArray = (EcoConnectionData**)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, sizeof(EcoConnectionData*) * newCapacity);
            
            if (pNewArray == 0) {
                pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCD);
                return -1;
            }

            /* Копирование существующих элементов */
            if (pCMe->m_pSinkArray != 0) {
                uint32_t i;
                for (i = 0; i < pCMe->m_cSinks; i++) {
                    pNewArray[i] = pCMe->m_pSinkArray[i];
                }
                pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCMe->m_pSinkArray);
            }

            pCMe->m_pSinkArray = pNewArray;
            pCMe->m_cSinksCapacity = newCapacity;
        }

        /* Добавление нового sink'а */
        pCMe->m_pSinkArray[pCMe->m_cSinks] = pCD;
        pCMe->m_cSinks++;

        /* Возвращение куки */
        *pcCookie = pCD->cCookie;
        return 0;
    }

    pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCD);
    return -1;
}

/*
 *
 * <сводка>
 *   Функция Unadvise
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1ConnectionPoint_Unadvise(/* in */ struct IEcoConnectionPoint* me, /* in */ uint32_t cCookie) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;
    EcoConnectionData* pCD = 0;
    IEcoUnknown* pSink = 0;
    uint32_t indx = 0;

    if (me == 0 ) {
        return -1;
    }

    for (indx = 0; indx < pCMe->m_cSinks; indx++) {
        pCD = pCMe->m_pSinkArray[indx];
        if (pCD->cCookie == cCookie) {
            pSink = pCD->pUnk;

            /* Удаление из массива (сдвиг элементов) */
            {
                uint32_t i;
                for (i = indx; i < pCMe->m_cSinks - 1; i++) {
                    pCMe->m_pSinkArray[i] = pCMe->m_pSinkArray[i + 1];
                }
            }
            pCMe->m_cSinks--;

            pSink->pVTbl->Release(pSink);
            pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCD);
            return 0;
        }
    }
    return -1;
}

/*
 *
 * <сводка>
 *   Функция EnumConnections
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
int16_t ECOCALLMETHOD CEcoLab1ConnectionPoint_EnumConnections(/* in */ struct IEcoConnectionPoint* me, /* out */ struct IEcoEnumConnections **ppEnum) {
    CEcoLab1ConnectionPoint* pCMe = (CEcoLab1ConnectionPoint*)me;

    if (me == 0 || ppEnum == 0 ) {
        return -1;
    }

    /* Создание нумератора подключений */
    return createCEcoLab1EnumConnections((IEcoUnknown*)pCMe->m_pISys, pCMe->m_pSinkArray, pCMe->m_cSinks, ppEnum);
}

/* Create Virtual Table IEcoConnectionPointVTbl */
IEcoConnectionPointVTbl g_x0000000300000000C000000000000046VTblCP = {
    CEcoLab1ConnectionPoint_QueryInterface,
    CEcoLab1ConnectionPoint_AddRef,
    CEcoLab1ConnectionPoint_Release,
    CEcoLab1ConnectionPoint_GetConnectionInterface,
    CEcoLab1ConnectionPoint_GetConnectionPointContainer,
    CEcoLab1ConnectionPoint_Advise,
    CEcoLab1ConnectionPoint_Unadvise,
    CEcoLab1ConnectionPoint_EnumConnections
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
int16_t ECOCALLMETHOD createCEcoLab1ConnectionPoint(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoConnectionPointContainer* pICPC, /* in */ const UGUID* riid, /* out */ IEcoConnectionPoint** ppICP) {
    CEcoLab1ConnectionPoint* pCMe = 0;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;	
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;		
    int16_t result = -1;

    /* Проверка указателей */
    if (ppICP == 0 || pIUnkSystem == 0 || pICPC == 0) {
        return result;
    }

    /* Получение системного интерфейса приложения */
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void **)&pISys);

    /* Проверка */
    if (result != 0 && pISys == 0) {
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

    /* Проверка */
    if (result != 0 && pIMem == 0) {
        /* Освобождение системного интерфейса в случае ошибки */
        pISys->pVTbl->Release(pISys);
        return result;
    }

    /* Выделение памяти для данных экземпляра */
    pCMe = (CEcoLab1ConnectionPoint*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoLab1ConnectionPoint));

    /* Сохранение указателя на системный интерфейс */
    pCMe->m_pISys = pISys;

    /* Сохранение указателя на интерфейс для работы с памятью */
    pCMe->m_pIMem = pIMem;
    pCMe->m_cNextCookie = 0;

    /* Внешний компонент */
    pCMe->m_pICPC = pICPC;
    pCMe->m_piid = (UGUID*)riid;

    /* Создание таблицы функций интерфейса IEcoConnectionPoint */
    pCMe->m_pVTblICP = &g_x0000000300000000C000000000000046VTblCP;
	
    /* Инициализация массива sink'ов */
    pCMe->m_cSinks = 0;
    pCMe->m_cSinksCapacity = INITIAL_SINK_CAPACITY;
    pCMe->m_pSinkArray = (EcoConnectionData**)pIMem->pVTbl->Alloc(pIMem, sizeof(EcoConnectionData*) * INITIAL_SINK_CAPACITY);
    
    if (pCMe->m_pSinkArray == 0) {
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
        pISys->pVTbl->Release(pISys);
        return -1;
    }
    
    /* Возврат указателя на интерфейс */
    *ppICP = (IEcoConnectionPoint*)&pCMe->m_pVTblICP;

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
void ECOCALLMETHOD deleteCEcoLab1ConnectionPoint(/* in */ IEcoConnectionPoint* pICP) {
    CEcoLab1ConnectionPoint* pCMe = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    EcoConnectionData* pCD = 0;
    uint32_t index = 0;

    if (pICP != 0 ) {
        /* Преобразование */
        pCMe = (CEcoLab1ConnectionPoint*)pICP;
        pIMem = pCMe->m_pIMem;
        
        /* Освобождение массива sink'ов */
        if (pCMe->m_pSinkArray != 0) {
            for (index = 0; index < pCMe->m_cSinks; index++) {
                pCD = pCMe->m_pSinkArray[index];
                if (pCD != 0) {
                    if (pCD->pUnk != 0) {
                        pCD->pUnk->pVTbl->Release(pCD->pUnk);
                    }
                    pIMem->pVTbl->Free(pIMem, pCD);
                }
            }
            pIMem->pVTbl->Free(pIMem, pCMe->m_pSinkArray);
        }
        
        if (pCMe->m_pISys != 0) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
            pCMe->m_pISys = 0;
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}