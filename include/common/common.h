#pragma once

#include <cstddef>

typedef int GLCCint;

enum GLCCError 
{
    GLCCError_Ok = 0,
    GLCCError_MissingRequiredParameter,
    GLCCError_FileNotFound,
    GLCCError_InvalidSyntax
};