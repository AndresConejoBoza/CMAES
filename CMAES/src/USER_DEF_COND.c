#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool register_condFunction(USER_DEF_COND* cond)
{
    return true;
}

bool kill_condFunction(USER_DEF_COND* cond)
{
    return true;
}

bool deregister_condFunction(USER_DEF_COND* cond)
{
    return true;
}

bool suspend_condFunction(USER_DEF_COND* cond)
{
    return true;
}

bool resume_condFunction(USER_DEF_COND* cond)
{
    return true;
}

bool restart_condFunction(USER_DEF_COND* cond)
{
    return true;
}

void ConstructorUSER_DEF_COND(USER_DEF_COND* cond) {
    cond->register_cond = &register_condFunction;
    cond->kill_cond = &kill_condFunction;
    cond->deregister_cond = &deregister_condFunction;
    cond->suspend_cond = &suspend_condFunction;
    cond->resume_cond = &resume_condFunction;
    cond->restart_cond = &restart_condFunction;
};