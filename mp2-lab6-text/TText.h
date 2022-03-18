#pragma once
#include "TStack.h"
#include <fstream>

using namespace std;

/* ........... Звено иерархического списка (текста) ........... */
struct TNode
{
    char str[81];
    TNode* pNext;
    TNode* pDown;

    TNode(char* _str = nullptr,
        TNode* _pNext = nullptr,
        TNode* _pDown = nullptr);

    ~TNode();
};

TNode::TNode(
    char* _str,
    TNode* _pNext,
    TNode* _pDown)
{
    if (_str == nullptr)
        str[0] = '\0';
    else
        strcpy(str, _str);
    pNext = _pNext;
    pDown = _pDown;
}

TNode::~TNode()
{
    //TODO позже напишем
}

/* .................... Иерархический текст ................... */
class TText
{
private:
    TNode* pFirst, * pCurr;
    //В стеке хранятся все указатели, до текущего (текущий не хранится)
    //При перемещениях стек модифицируем
    TStack<TNode*> st;

    //Рекурсивное чтение из файла
    TNode* ReadRec(ifstream& fin);

    //Печать на экран текста
    //печатает саму строку, потом всё по pDown, потом всё по pNext
    //(рекурсивно)
    int textLevel = 0;

    void PrintRec(TNode* p);

public:
    //Перемещение указателя pCurr на следующее звено
    void GoNextNode();
    //Перемещение указателя pCurr на звено подчинённой ему части
    void GoDownNode();
    //Переход назад (вверх) по вложенности
    void GoUp();
    //Возврат в корневую (самую первую) строку всего текста
    void GoFirstNode();

    //Вставка новой строки за текущей
    void InsNextLine(char* _str);
    //Вставка нового заголовка за текущей строкой и
    //переподчинение всей нижней части списка ему
    void InsNextSection(char* _str);
    //Вставка новой строки в начало подчинённой части
    void InsDownLine(char* _str);
    //Вставка нового заголовка в подчинённую часть
    void InsDownSection(char* _str);

    //С утечкой памяти (удаляется только pDel, но не его внутренности)!!!
    //Удалить следующее звено
    void DelNext();
    void DelDown();

    //Возврат pCurr на первое звено
    void Reset();
    //извлекается что-то из стека
    //и потомков извлечённой вершины кладём в стек
    //переводит pCurr на новую вершину стека
    void GoNext();
    bool IsEnd();

    char* GetCurrentLine();

    //Загрузка текста из файла
    void Load(string s);
    void Print();
};

TNode* TText::ReadRec(ifstream& fin)
{
    TNode* pTemp = nullptr, * pHead = nullptr;
    char str[81];

    while (!fin.eof())
    {
        fin.getline(str, 81, '\n');
        if (str[0] == '{')
            pTemp->pDown = ReadRec(fin);
        else if (str[0] == '}')
            break;
        else
        {
            TNode* newNode = new TNode(str);
            if (pHead == nullptr)
                pTemp = pHead = newNode;
            else
                pTemp->pNext = newNode;
            pTemp = newNode;
        }
    }
    return pHead;
}

void TText::PrintRec(TNode* p)
{
    //можно ещё скобки печатать (если pDown существует, печатать скобки сверху, снизу)
    //можно даже смещение текста делать
    if (p != nullptr)
    {
        for (int i = 0; i < textLevel; i++)
            cout << "   ";
        cout << " " << p->str << '\n';

        textLevel++;
        PrintRec(p->pDown);
        textLevel--;
        PrintRec(p->pNext);
    }
}

void TText::GoNextNode()
{
    if (pCurr == nullptr)
        throw "Can't reach the next node: current node doesn't exist";
    {
        st.Push(pCurr);
        pCurr = pCurr->pNext;
    }
}

void TText::GoDownNode()
{
    if (pCurr == nullptr)
        throw "Can't reach down node: current node doesn't exist";
    {
        st.Push(pCurr);
        pCurr = pCurr->pDown;
    }
}

void TText::GoUp()
{
    if (st.IsEmpty())
        throw "Can't go up: transitions history is empty";

    TNode* prevNode = st.Pop();
    pCurr = prevNode;
}

void TText::GoFirstNode()
{
    st.Clear();
    pCurr = pFirst;
}

void TText::InsNextLine(char* _str)
{
    if (pCurr == nullptr)
        throw "Can't insert next line: current node doesn't exist";
    TNode* newNode = new TNode(_str);
    newNode->pNext = pCurr->pNext;
    pCurr->pNext = newNode;
}

void TText::InsNextSection(char* _str)
{
    if (pCurr == nullptr)
        throw "Can't insert next section: current node doesn't exist";
    TNode* newNode = new TNode(_str);
    newNode->pDown = pCurr->pNext;
    pCurr->pNext = newNode;
}

void TText::InsDownLine(char* _str)
{
    if (pCurr == nullptr)
        throw "Can't insert down line: current node doesn't exist";
    TNode* newNode = new TNode(_str);
    newNode->pNext = pCurr->pDown;
    pCurr->pDown = newNode;
}

void TText::InsDownSection(char* _str)
{
    if (pCurr == nullptr)
        throw "Can't insert down section: current node doesn't exist";
    TNode* newNode = new TNode(_str);
    newNode->pDown = pCurr->pDown;
    pCurr->pDown = newNode;
}

void TText::DelNext()
{
    if (pCurr == nullptr)
        throw "Can't delete next line/section: current node doesn't exist";
    TNode* pDel = pCurr->pNext;

    if (pDel == nullptr)
        throw "Nothing to delete: next line/section is nullptr";
    pCurr->pNext = pDel->pNext;
    delete pDel;
}

void TText::DelDown()
{
    if (pCurr == nullptr)
        throw "Can't delete next line/section: current node doesn't exist";
    TNode* pDel = pCurr->pDown;

    if (pDel == nullptr)
        throw "Nothing to delete: next line/section is nullptr";
    pCurr->pDown = pDel->pNext;
    delete pDel;
}

void TText::Reset()
{
    st.Clear();

    if (pFirst != nullptr)
    {
        pCurr = pFirst;

        //Текущий элемент в стек
        st.Push(pCurr);

        //pDown и pNext в стек, если они есть
        //(!) порядок ифов можно и менять
        if (pCurr->pNext != nullptr)
            st.Push(pCurr->pNext);
        if (pCurr->pDown != nullptr)
            st.Push(pCurr->pDown);
    }
}

void TText::GoNext()
{
    pCurr = st.Pop();

    //Если не дошли до "фиктивной строки снизу стека"
    if (pCurr != pFirst)
    {
        //(!) порядок ифов можно и менять
        if (pCurr->pNext != nullptr)
            st.Push(pCurr->pNext);
        if (pCurr->pDown != nullptr)
            st.Push(pCurr->pDown);
    }
}

bool TText::IsEnd()
{
    return st.IsEmpty();
}

char* TText::GetCurrentLine()
{
    return pCurr->str;
}

void TText::Load(string s)
{
    ifstream ifs(s);
    pFirst = ReadRec(ifs);
}

void TText::Print()
{
    PrintRec(pFirst);
}


//Описание
//Чтение из файла, вывод на экран
//Предполагаем, что в файле текст записан так:
//вложенность регулируется {}
//файл без вложенностей - последовательность строк
//строка1
//строка2
//.......
//строкаn

//Пока не конец файла (while != EOF), читаем новую строку и пристыковываем через pNext
//отступов никаких нет, текст без пробелов в начале.
//затем началась подчинённая часть
//скобки находятся в первом символе строки (в строках не может быть скобок)

//строка1
//строка2
//{
//  строка21
//  строка21
//  {
//  строка211
//  }
//}
//строкаn

///(отступов нет)