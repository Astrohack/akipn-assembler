#include <stdio.h>
#include <iostream.h>;
#include <conio.h>;
#include <string.h>;
#include <stdlib.h>;
#include <float.h>


#define upper (char)201
#define upper_right (char)187
#define vertical (char)186
#define lower (char)200
#define lower_right (char)188
#define horizontal (char)205


extern "C" float first(float * array, int size); 
extern "C" int second(char * str, char character); 

void print_char(char c, int n) {
    for (int i = 0; i < n; ++i)
        putch(c); 
}

int get_numbers(float *numbers) {
    int i = 0;
    float input;
    cout << "Input array elements:" << endl;
    while (1) {
        cin >> input;
        if(cin.fail()) {
            break;
        }
        numbers[i] = input;
        i++;
    }
    cin.clear();
    cin.ignore(10, '\n');
    return i;
}

void get_string(char *string) {
    cout << "Input string:" << endl;
    cin >> string;
    string[strlen(string)] = '$';
}
char get_character() {
    cout << "Input character you want to count:" << endl;
    char c;
    cin >> c;
    return c;
}

int main() {
    clrscr();
    
    float numbers[100] = {0};  
    int size = get_numbers(numbers);

    clrscr();

    char string[100] = {0};
    get_string(string);
    char character = get_character();

    clrscr();

    /// Print title bar
    const int width = 76;
    char title[] = " Hybrid program ";
    int title_len = strlen(title); 
    cout << upper;
    print_char(205, (width - title_len)/2);
    cout << title;
    print_char(205, (width - title_len)/2);
    cout << upper_right << endl;

    /// First function
    cout << vertical << "(1) average of array: [ " << numbers[0];
    for (int i = 1; i < size; i ++)
        cout << ", " << numbers[i];
    cout << " ] is " << first(numbers, size) << endl;

    /// Second function
    cout << vertical << "(2) word '" << string << "' contains " << second(string, character) << " '" << character << "' characters " << endl;

    /// Print footer bar
    cout << lower;
    print_char(205, width);
    cout << lower_right << endl;

    /// Print right vertical lines
    gotoxy(width + 2, 2);
    putch(vertical);
    gotoxy(width + 2, 3);
    putch(vertical);
    
    return 0;
}