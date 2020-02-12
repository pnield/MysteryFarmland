#include "text.h"

Text::Text(char* letter, int x, int y)
{
	letterSprite(letter);
	spriteSheetRect.w = 4;
	spriteSheetRect.h = 5;
	this->x = x;
	this->y = y;
}

Text::~Text()
{
}

void Text::letterSprite(char* letter)
{
	spriteSheetRect.x = 0;
	spriteSheetRect.y = 0;

	switch (*letter)
	{
	case 'A':
	case 'a':
		break;
	case 'B':
	case 'b':
		spriteSheetRect.x = 4;
		break;
	case 'C':
	case 'c':
		spriteSheetRect.x = 8;
		break;
	case 'D':
	case 'd':
		spriteSheetRect.x = 12;
		break;
	case 'E':
	case 'e':
		spriteSheetRect.x = 16;
		break;
	case 'F':
	case 'f':
		spriteSheetRect.x = 20;
		break;
	case 'G':
	case 'g':
		spriteSheetRect.x = 24;
		break;
	case 'H':
	case 'h':
		spriteSheetRect.x = 28;
		break;
	case 'I':
	case 'i':
		spriteSheetRect.x = 32;
		break;
	case 'J':
	case 'j':
		spriteSheetRect.x = 36;
		break;
	case 'K':
	case 'k':
		spriteSheetRect.x = 40;
		break;
	case 'L':
	case 'l':
		spriteSheetRect.x = 44;
		break;
	case 'M':
	case 'm':
		spriteSheetRect.x = 48;
		break;
	case 'N':
	case 'n':
		spriteSheetRect.x = 52;
		break;
	case 'O':
	case 'o':
		spriteSheetRect.x = 56;
		break;
	case 'P':
	case 'p':
		spriteSheetRect.x = 60;
		break;
	case 'Q':
	case 'q':
		spriteSheetRect.x = 64;
		break;
	case 'R':
	case 'r':
		spriteSheetRect.x = 68;
		break;
	case 'S':
	case 's':
		spriteSheetRect.x = 72;
		break;
	case 'T':
	case 't':
		spriteSheetRect.x = 76;
		break;
	case 'U':
	case 'u':
		spriteSheetRect.x = 80;
		break;
	case 'V':
	case 'v':
		spriteSheetRect.x = 84;
		break;
	case 'W':
	case 'w':
		spriteSheetRect.x = 88;
		break;
	case 'X':
	case 'x':
		spriteSheetRect.x = 92;
		break;
	case 'Y':
	case 'y':
		spriteSheetRect.x = 96;
		break;
	case 'Z':
	case 'z':
		spriteSheetRect.x = 100;
		break;
	//second line
	case ' ':
		spriteSheetRect.x = 100;
		spriteSheetRect.y = 5;
		break;
	case '0':
		spriteSheetRect.y = 5;
		break;
	case '1':
		spriteSheetRect.x = 4;
		spriteSheetRect.y = 5;
		break;
	case '2':
		spriteSheetRect.x = 8;
		spriteSheetRect.y = 5;
		break;
	case '3':
		spriteSheetRect.x = 12;
		spriteSheetRect.y = 5;
		break;
	case '4':
		spriteSheetRect.x = 16;
		spriteSheetRect.y = 5;
		break;
	case '5':
		spriteSheetRect.x = 20;
		spriteSheetRect.y = 5;
		break;
	case '6':
		spriteSheetRect.x = 24;
		spriteSheetRect.y = 5;
		break;
	case '7':
		spriteSheetRect.x = 28;
		spriteSheetRect.y = 5;
		break;
	case '8':
		spriteSheetRect.x = 32;
		spriteSheetRect.y = 5;
		break;
	case '9':
		spriteSheetRect.x = 36;
		spriteSheetRect.y = 5;
		break;
	case '.':
		spriteSheetRect.x = 40;
		spriteSheetRect.y = 5;
		break;
	case ',':
		spriteSheetRect.x = 44;
		spriteSheetRect.y = 5;
		break;
	case '\'':
		spriteSheetRect.x = 48;
		spriteSheetRect.y = 5;
		break;
	case '!':
		spriteSheetRect.x = 52;
		spriteSheetRect.y = 5;
		break;
	case '?':
		spriteSheetRect.x = 56;
		spriteSheetRect.y = 5;
		break;
	case ':':
		spriteSheetRect.x = 60;
		spriteSheetRect.y = 5;
		break;
	case '/':
		spriteSheetRect.x = 64;
		spriteSheetRect.y = 5;
		break;
	case '[':
		spriteSheetRect.x = 68;
		spriteSheetRect.y = 5;
		break;
	case ']':
		spriteSheetRect.x = 72;
		spriteSheetRect.y = 5;
		break;
	case '>':
		spriteSheetRect.x = 76;
		spriteSheetRect.y = 5;
		break;
	}
}

void Text::draw(SDL_Renderer* gRenderer, LTexture* txGameText)
{
	txGameText->renderClip(x, y, gRenderer, spriteSheetRect);
}