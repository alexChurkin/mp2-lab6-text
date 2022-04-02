#define _CRT_SECURE_NO_WARNINGS
#define HOME  71
#define DOWN  80
#define NEXT  77
#define UP	  72
#define ESC	  27
#define INS	  82
#define DEL	  83
#define ENTER 13

#include <iostream>
#include <conio.h>
#include "TText.h"
#include "ConsoleExt.h"

TMem TNode::mem;

void OnEsc(TText &t)
{

	cout << "\nTText.Print():\n";
	cout << "-------------\n";
	t.Print();
	cout << '\n';

	cout << "Printing via iteration:\n";
	cout << "----------------------\n";
	for (t.Reset(); !t.IsEnd(); t.GoNext())
		cout << t.GetCurrentLine() << '\n';

	t.Save("..\\out.txt");
	cout << "\nOutput saved to out.txt.\n\n";
}

int main()
{
	//Инициализация памяти под TNode'ы для TText
	TNode::InitMem(100);

	TText t;

	t.Load("..\\text.txt");
	t.GoFirstNode();

	char ch;

	do {
		cout << ">,v,^, Home, Ins, Del, Enter, Esc\n";
		cout << "---------------------------------\n";

		t.Print();
		cout << '\n';

		ch = _getch();

		if (ch == 0xE0)
			ch = _getch();
		if (ch == ESC)
		{
			OnEsc(t);
			break;
		}
			
		if (ch != ENTER)
			ch = _getch();

		switch (ch)
		{
		case ENTER: break;
		case HOME:
			t.GoFirstNode();
			break;
		case DOWN:
			t.GoDownNode();
			break;
		case NEXT:
			t.GoNextNode();
			break;
		case UP:
			t.GoUp();
			break;
		case INS:
			cout << "Enter new text item: ";
			char item[256];
			if (fgets(item, sizeof(item), stdin))
			{
				item[strcspn(item, "\r\n")] = 0;

				cout << "Insert:\n1 - Next line, "
					<< "2 - Next section, 3 - Down line, "
					<< "4 - Down section\n";
				int o1;
				cin >> o1;
				if (o1 == 1) t.InsNextLine(item);
				else if (o1 == 2) t.InsNextSection(item);
				else if (o1 == 3) t.InsDownLine(item);
				else if (o1 == 4) t.InsDownSection(item);
			}
			cin.ignore(256, '\n');
			break;
		case DEL:
			int o2;
			cout << "1 - Delete down, 2 - Delete next\n";
			cin >> o2;
			if (o2 == 1) t.DelDown();
			else if (o2 == 2) t.DelNext();
			cin.ignore(256, '\n');
			break;
		}

		ClearScreen();
	} while (ch != ESC);

	cout << ">> Free nodes <<:\n";
	cout << "----------------\n";
	TNode::PrintFreeNodes();
	cout << '\n';

	TNode::CleanMem(t);
	cout << ">> Free nodes after garbage collection <<:\n";
	cout << "-----------------------------------------\n";
	TNode::PrintFreeNodes();

	cout << "\n>> Final output <<:\n";
	cout << "------------------\n";
	t.Print();

	cout << "\nCopy test:\n";
	cout << "---------\n";

	TText* t2 = t.GetCopy();
	t2->Print();
}