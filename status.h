//
// Created by Takaaki Mizuno on 2021/11/06.
//

#ifndef CONCEPTUAL_VIRUS_STATUS_H
#define CONCEPTUAL_VIRUS_STATUS_H

typedef struct {
    int index;
    int life_time;
    int length;
    char hash[33];
} Status;

#endif //CONCEPTUAL_VIRUS_STATUS_H
