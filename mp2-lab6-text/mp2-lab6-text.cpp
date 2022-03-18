#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "TText.h"

int main()
{
    TText t;
    t.Load("..\\text.txt");
    t.Print();
    
    cout << "\n\n";

    for (t.Reset(); !t.IsEnd(); t.GoNext())
        cout << t.GetCurrentLine() << '\n';

    cout << "\n\n";

    t.Export("..\\out.txt");
}