#pragma once
#include "TStack.h"
#include <fstream>

using namespace std;

/* ........... ����� �������������� ������ (������) ........... */
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
    //TODO ����� �������
}

/* .................... ������������� ����� ................... */
class TText
{
private:
    TNode* pFirst, * pCurr;
    //� ����� ����������� ��� ��������� �� �������� (������� �� ��������)
    //��� ������������ ���� ������������
    TStack<TNode*> st;

    //����������� ������ �� �����
    TNode* ReadRec(ifstream& fin);

    //������ �� ����� ������
    //�������� ���� ������, ����� �� �� pDown, ����� �� �� pNext
    //(����������)
    int textLevel = 0;

    void PrintRec(TNode* p);
    void WriteRec(TNode* p, ostream& out);

public:
    //����������� ��������� pCurr �� ��������� �����
    void GoNextNode();
    //����������� ��������� pCurr �� ����� ����������� ��� �����
    void GoDownNode();
    //������� ����� (�����) �� �����������
    void GoUp();
    //������� � �������� (����� ������) ������ ����� ������
    void GoFirstNode();

    //������� ����� ������ �� �������
    void InsNextLine(char* _str);
    //������� ������ ��������� �� ������� ������� �
    //�������������� ���� ������ ����� ������ ���
    void InsNextSection(char* _str);
    //������� ����� ������ � ������ ����������� �����
    void InsDownLine(char* _str);
    //������� ������ ��������� � ����������� �����
    void InsDownSection(char* _str);

    //� ������� ������ (��������� ������ pDel, �� �� ��� ������������)!!!
    //������� ��������� �����
    void DelNext();
    void DelDown();

    //������� pCurr �� ������ �����
    void Reset();
    //������� pCurr �����
    void GoNext();
    //�������� ��������� ������
    bool IsEnd();

    //��������� ������ �������� �����
    char* GetCurrentLine();

    //�������� ������ �� �����
    void Load(string fn);
    //������ ������ �� �����
    void Print();
    //���������� ������ � ����
    void Save(string fn);
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
        else if (strcmp(str, "") == 0)
            continue;
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
    if (p != nullptr)
    {
        for (int i = 0; i < textLevel; i++)
            cout << "   ";

        if (p == pCurr) cout << "*";
        else cout << " ";

        cout << p->str << '\n';

        textLevel++;
        PrintRec(p->pDown);
        textLevel--;
        PrintRec(p->pNext);
    }
}

void TText::WriteRec(TNode* p, ostream& out)
{
    if (p != nullptr)
    {
        out << p->str << '\n';
        if (p->pDown != nullptr)
        {
            out << "{\n";
            WriteRec(p->pDown, out);

            out << "}\n";
        }
        WriteRec(p->pNext, out);
    }
}

void TText::GoNextNode()
{
    if (pCurr != nullptr && pCurr->pNext != nullptr)
    {
        st.Push(pCurr);
        pCurr = pCurr->pNext;
    }
}

void TText::GoDownNode()
{
    if (pCurr != nullptr && pCurr->pDown != nullptr)
    {
        st.Push(pCurr);
        pCurr = pCurr->pDown;
    }
}

void TText::GoUp()
{
    if (!st.IsEmpty())
    {
        TNode* prevNode = st.Pop();
        pCurr = prevNode;
    }
}

void TText::GoFirstNode()
{
    st.Clear();
    pCurr = pFirst;
}

void TText::InsNextLine(char* _str)
{
    if (pCurr != nullptr)
    {
        TNode* newNode = new TNode(_str);
        newNode->pNext = pCurr->pNext;
        pCurr->pNext = newNode;
    }
}

void TText::InsNextSection(char* _str)
{
    if (pCurr != nullptr)
    {
        TNode* newNode = new TNode(_str);
        newNode->pDown = pCurr->pNext;
        pCurr->pNext = newNode;
    }
}

void TText::InsDownLine(char* _str)
{
    if (pCurr != nullptr)
    {
        TNode* newNode = new TNode(_str);
        newNode->pNext = pCurr->pDown;
        pCurr->pDown = newNode;
    }
}

void TText::InsDownSection(char* _str)
{
    if (pCurr != nullptr)
    {
        TNode* newNode = new TNode(_str);
        newNode->pDown = pCurr->pDown;
        pCurr->pDown = newNode;
    }
}

void TText::DelNext()
{
    if (pCurr != nullptr)
    {
        TNode* pDel = pCurr->pNext;
        if (pDel != nullptr)
        {
            pCurr->pNext = pDel->pNext;
            delete pDel;
        }
    }
}

void TText::DelDown()
{
    if (pCurr != nullptr)
    {
        TNode* pDel = pCurr->pDown;
        if (pDel != nullptr)
        {
            pCurr->pDown = pDel->pNext;
            delete pDel;
        }
    }

}

void TText::Reset()
{
    st.Clear();

    if (pFirst != nullptr)
    {
        pCurr = pFirst;

        //������� ������� � ����
        st.Push(pCurr);

        //pDown � pNext � ����, ���� ��� ����
        //(!) ������� ���� ����� � ������
        if (pCurr->pNext != nullptr)
            st.Push(pCurr->pNext);
        if (pCurr->pDown != nullptr)
            st.Push(pCurr->pDown);
    }
}

void TText::GoNext()
{
    pCurr = st.Pop();

    //���� �� ����� �� "��������� ������ ����� �����"
    if (pCurr != pFirst)
    {
        //(!) ������� ���� ����� � ������
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

void TText::Load(string fn)
{
    ifstream ifs(fn);
    pFirst = ReadRec(ifs);
}

void TText::Print()
{
    PrintRec(pFirst);
}

void TText::Save(string fn)
{
    std::ofstream out;
    out.open(fn);

    if (!out.is_open()) throw "Export exception!";

    WriteRec(pFirst, out);
}

//��������
//������ �� �����, ����� �� �����
//������������, ��� � ����� ����� ������� ���:
//����������� ������������ {}
//���� ��� ������������ - ������������������ �����
//������1
//������2
//.......
//������n
//���� �� ����� ����� (while != EOF), ������ ����� ������ � �������������� ����� pNext
//�������� ������� ���, ����� ��� �������� � ������.
//����� �������� ����������� �����
//������ ��������� � ������ ������� ������ (� ������� �� ����� ���� ������)
//������1
//������2
//{
//  ������21
//  ������21
//  {
//  ������211
//  }
//}
//������n
//(�������� ���)