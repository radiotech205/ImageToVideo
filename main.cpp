#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("argc = %i, must be argc = 5\n", argc);
        printf("argv[1] = <./srcPics>, argv[2] = <.png> \n");
        return -1;
    }
    // argv[1] - директория с файлами
    // argv[2] - расширение файлов
    // argv[3] - количество кадров  в секунду.
    // предполагается, что все файлы одного размера, размер изначально неизвестен.
    // пример: ./tiff .tiff 1

    DIR* d;
    // данные директории.
    struct dirent* entry = NULL;
    // массив данных директории.
    struct dirent* entryArr = (struct dirent*)malloc(sizeof( struct dirent) * 1024 * 1024);
    // открыть директорию.
    d = opendir(argv[1]);
    if (d == NULL)
    {
        printf("Ошибка чтения директории\n");
        return 1;
    }

    // счётчик файлов с нужным расширением.
    uint foundFilesCount = 0;
    char* findRes = NULL;

    // получение списка всех файлов в директории.

    for (uint index = 0; entry = readdir(d), entry != 0; index++)
    {
        // составление имени файла с расширением.
        findRes = strstr(entry->d_name, argv[2]);
        // если файл с нужным расширением найден.
        if (findRes != NULL)
        {
            *(entryArr + foundFilesCount) = *entry;
            foundFilesCount++;
        }
    }

    closedir(d);
    printf("найдено файлов: %i\n", foundFilesCount);
    printf("массив dirent:\n");

    // сортировка файлов по d_ino. Последовательность файлов для сборки видео.
    dirent entryTempNP;
    dirent entryTempJ;
    dirent entryTempJp1;
    for(uint i = 0; i < foundFilesCount - 1; i++)
    {
        for(uint j = 0; j < foundFilesCount - 1 - i; j++)
        {
            entryTempJ = entryArr[j];
            entryTempJp1 = entryArr[j + 1];

            if (entryTempJ.d_ino > entryTempJp1.d_ino)
            {
                entryTempNP = entryArr[j];
                entryArr[j] = entryArr[j + 1];
                entryArr[j + 1] = entryTempNP;
            }
        }
    }

    printf("Проверка сортировки\n");
    for (uint index = 0; index < foundFilesCount; index++)
    {
        printf("%i \t %s \t inode=%li, \t nype = %i\n", index, entryArr[index].d_name, entryArr[index].d_ino, entryArr[index].d_type);
    }

    // составление первого имени файла для получения размера изображения.
    char fillPathToFile[512];
    memset(fillPathToFile, 0, 512);
    strcpy(fillPathToFile, argv[1]);
    strcat(fillPathToFile, "/");
    strcat(fillPathToFile, entryArr[0].d_name);

    printf("argv[1] = %s \n", argv[1]);
    printf("fillPathToFile = %s \n", fillPathToFile);

    // получение размера изображения.
    Mat srcC3 = imread(fillPathToFile, IMREAD_UNCHANGED);

    // подготовка строки к формированию имени файла.
    memset(fillPathToFile, 0, 512);
    strcat(fillPathToFile, argv[1]);
    strcat(fillPathToFile, "/");


    // ИНИЦИАЛИЗАЦИЯ ВЫИДЕОЗАПИСИ
    VideoWriter writer;
    // выбор кодека. Доступны: 'M', 'J', 'P', 'G'       'D', 'I', 'V', 'X'      'M', 'P', '4', 'V'      'X', '2', '6', '4'
    int codec = VideoWriter::fourcc('X', '2', '6', '4');
    // определение количества кадров.
    double fps = static_cast<double>(atoi(argv[3]));
    printf("fps = %lf \n", fps);
    // имя выходного файла
    string filename = "./live.avi";

    // определение цч/б или цветное.
    bool isColor = (srcC3.type() == CV_8UC3);
    printf("isColor = %i \n", isColor);

    // открытие видеозаписи
    writer.open(filename, codec, fps, srcC3.size(), isColor);
    // проверка успешности открытия.
    if (!writer.isOpened()) {
        cerr << "Could not open the output video file for write\n";
        return -1;
    }
    cout << "Writing videofile: " << filename << endl
         << "Press any key to terminate" << endl;

    // ПРОХОД ПО ФАЙЛАМ И ВИДЕОЗАПИСЬ ИЗОБРАЖЕНИЙ.
    for (uint index = 0; index < foundFilesCount; index++)
    {
        // формирование имени файла.
       strcat( fillPathToFile, entryArr[index].d_name);
       printf("Progress is %i persents\n", 100 * index/foundFilesCount);

        // открытие изображения.
       srcC3 = imread(fillPathToFile, IMREAD_UNCHANGED);
       // запись кадра в видеопоток.
       writer.write(srcC3);

       // подготовка строки к формированию имени файла.
       memset(fillPathToFile, 0, 512);
       strcat(fillPathToFile, argv[1]);
       strcat(fillPathToFile, "/");

       // если нажата enter - выходим
       char c = waitKey(1);
       if (c == 10)
           break;
    }
    printf("Видео сделано!\n");
}

