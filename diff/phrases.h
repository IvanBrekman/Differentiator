//
// Created by IvanBrekman on 01.12.2021
//

#include <cstdlib>

const char* TRANSITIONAL_PHRASES[] = {
    "Преобразуя получаем ",
    "Не трудно заметить, что ",
    "Очевидно, что ",
    "Обратим внимание, что ",
    "Не требует объяснений тот факт, что ",
    "Никто не будет ШоКиРоВаН, если мы скажем, что ",
    "Не сложно заметить, что ",
    "По известным фактам ",
    "Если данный кусок вызывавает сомнения, то обратитесь к доказательтсву великой теоремы Ферма ",
    "Можно увидеть, что ",
    "Легко привести к виду "
};

const char* get_random_trans_phrase() {
    int index = rand() % (sizeof(TRANSITIONAL_PHRASES) / sizeof(TRANSITIONAL_PHRASES[0]));

    return TRANSITIONAL_PHRASES[index];
}