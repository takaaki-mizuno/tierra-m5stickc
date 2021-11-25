//
// Created by Takaaki Mizuno on 2021/11/06.
//

#ifndef CONCEPTUAL_VIRUS_CLIENT_H
#define CONCEPTUAL_VIRUS_CLIENT_H

#include "vm.h"

class DashboardClient {
    private:
        VM *vm;
    public:
        DashboardClient(VM *_vm);
        ~DashboardClient();
        void SendInfo();
        void BuildJson(char *position, int size);
        char* GetStatusJSON();
};

#endif //CONCEPTUAL_VIRUS_CLIENT_H
