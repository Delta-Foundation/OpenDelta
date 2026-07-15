#include <stdio.h>
#include "lib/colors.h"

void help()
{
    printf("+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+\n");
    printf("| 1.dltinfo: как неофетч, но для нашей ОС.                                      | 11.help - вывод списка команд и их значение. |\n");
    printf("| 2.calc - калькулятор.                                                         | 12.exit - выход из dltsh.                    |\n");
    printf("| 3.ls - выводит список файлов в текущем каталоге.                              | 13.clear - очистка экрана.                   |\n");
    printf("| 4.pwd - показывает путь к текущей директории.                                 | 14.ver - вывод установленной версии dltsh.   |\n");
    printf("| 5.cd - переходит в какую-либо папку.                                          | 15.touch (без аргументов) - создание файла.  |\n");
    printf("| 6.cat - выводит содержимое файла.                                             | 16.mkdir (без аргументов) - создание папки.  |\n");
    printf("| 7.rm - удаление папки или файла.                                              |                                              |\n");
    printf("| 8.add - создание файла или каталога(updated without fixed bug).               |                                              |\n");
    printf("| 9.lsf - вывод всех файлов для работы ОС.                                      |                                              |\n");
    printf("| 10.dex - консольный текстовый редактор.                                       |                                              |\n");
    printf("+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+\n");
}

void write_logo()
{
    printf(T_CYAN " ___________       ___________  ___          _________________            ____ \n" T_RESET);
    printf(T_CYAN " | |      XX       | |     | |  |  |         |______   ______|            //XX  \n" T_RESET);
    printf(T_CYAN " | |       XX      | |     | |  |  |                | |                  //  XX  \n" T_RESET);
    printf(T_CYAN " | |        XX     | |     | |  |  |                | |                 //    XX  \n" T_RESET);
    printf(T_CYAN " | |         XX    |_|_____|_|  |  |                | |                //      XX  \n" T_RESET);
    printf(T_CYAN " | |          XX   | |          |  |                | |               //        XX  \n" T_RESET);
    printf(T_CYAN " | |           XX  | |          |  |                | |              //==========XX  \n" T_RESET);
    printf(T_CYAN " | |           //  | |     ___  |  |      ___       | |             //            XX  \n" T_RESET);
    printf(T_CYAN " | |          //   | |     | |  |  |      | |       | |            //              XX  \n" T_RESET);
    printf(T_CYAN " | |_________//    |_|_____|_|  |__|______|_|       |_|           //                XX  \n" T_RESET);
}

void welcome()
{
    const char frstln[] = "._____________________________.";
    const char newlin[] = "|                             |";
    const char thrdln[] = "|[ Welcome to the OpenDelta! ]|";
    const char endlne[] = "|_____________________________|";

    printf("%s\n", frstln);
    printf("%s\n", newlin);
    printf("%s\n", thrdln);
    printf("%s\n", endlne);
    printf(T_CYAN "[впешите help для того чтобы узнать большинство команд]\n" T_RESET);
}
